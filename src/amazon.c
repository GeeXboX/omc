/* GeeXboX Open Media Center.
 * Copyright (C) 2006 Benjamin Zores <ben@geexbox.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Library General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 */

#define _GNU_SOURCE
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <ctype.h>
#include <fcntl.h>
#include <curl/curl.h>
#include <curl/types.h>
#include <curl/easy.h>
#include <pthread.h>

#include "exml/parser.h"
#include "omc.h"
#include "avplayer.h"

#define AMAZON_DEVEL_ID "0HV80J2YXZPNRRPKVTG2"
#define AMAZON_MEDIA_TYPE_MUSIC "music"
#define AMAZON_MEDIA_TYPE_DVD "dvd"
#define AMAZON_COUNTRY_SEARCH_US "us"
#define AMAZON_COUNTRY_SEARCH_FR "fr"

#define URL_THUMB_SMALL  "ImageUrlSmall"
#define URL_THUMB_MEDIUM "ImageUrlMedium"
#define URL_THUMB_LARGE  "ImageUrlLarge"

#define DEFAULT_AUDIO_COVER_FILE ".cover.png"

#define SAVE_PATH "/tmp"
#define MAX_INFO_LEN 65535

static int
curl_http_info_get (void *buffer, size_t size, size_t nmemb, void *stream)
{
  char *msg = (char *) stream;

  if (!msg)
    return -1;

  strncat (msg, buffer, (size * nmemb));

  return (size * nmemb);
}

static int
curl_http_file_get (void *buffer, size_t size, size_t nmemb, void *stream)
{
  FILE *f = (FILE *) stream;

  if (!f)
    return -1;

  return fwrite (buffer, size, nmemb, f);
}

static exml_node_t *
exml_find_node (exml_node_t *root, char *name)
{
  exml_node_t *n = NULL;

  if (!root || !root->name || !name)
    return NULL;

  if (!strcmp (root->name, name))
    return root;

  n = exml_find_node (root->child, name);
  if (n)
    return n;

  n = exml_find_node (root->next, name);
  if (n)
    return n;
  
  return NULL;
}

static char *
amazon_info_request (CURL *curl, char *country, char *type, char *keywords)
{
  char *info = NULL;
  char url[1024];

  sprintf (url, "http://xml.amazon.com/onca/xml3?t=webservices-20&dev-t=%s&KeywordSearch=%s&mode=%s&type=lite&locale=%s&page=1&f=xml", AMAZON_DEVEL_ID, keywords, type, country);

  info = (char *) malloc (MAX_INFO_LEN * sizeof (char));
  bzero (info, MAX_INFO_LEN);
  
  curl_easy_setopt (curl, CURLOPT_URL, url);
  curl_easy_setopt (curl, CURLOPT_WRITEFUNCTION, curl_http_info_get);
  curl_easy_setopt (curl, CURLOPT_WRITEDATA, info);
  
  if (curl_easy_perform (curl) != CURLE_OK)
    return NULL;

  return info;
}

static int
amazon_image_get (CURL *curl, char *url)
{
  FILE *f = NULL;
  char file[1024];
  
  sprintf (file, "%s/%s", SAVE_PATH, basename (url)); 
  f = fopen (file, "w+");
  if (!f)
    return 0;
  
  curl_easy_setopt (curl, CURLOPT_URL, url);
  curl_easy_setopt (curl, CURLOPT_WRITEFUNCTION, curl_http_file_get);
  curl_easy_setopt (curl, CURLOPT_WRITEDATA, f);
  
  if (curl_easy_perform (curl) != CURLE_OK)
  {
    fclose (f);
    return 0;
  }

  fclose (f);
  return 1;
}

static void
url_escape_string (char *outbuf, const char *inbuf)
{
  unsigned char c, c1, c2;
  int i, len;

  len = strlen (inbuf);

  for  (i = 0; i < len; i++)
  {
    c = inbuf[i];
    if ((c == '%') && i < len - 2)
    { /* need 2 more characters */
      c1 = toupper (inbuf[i + 1]);
      c2 = toupper (inbuf[i + 2]);
      /* need uppercase chars */
    }
    else
    {
      c1 = 129;
      c2 = 129;
      /* not escaped chars */
    }

    if ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z')
        || (c >= '0' && c <= '9') || (c >= 0x7f))
      *outbuf++ = c;
    else if (c =='%'
             && ((c1 >= '0' && c1 <= '9') || (c1 >= 'A' && c1 <= 'F'))
             && ((c2 >= '0' && c2 <= '9') || (c2 >= 'A' && c2 <= 'F')))
    {
      /* check if part of an escape sequence */
      *outbuf++ = c; /* already escaped */
    }
    else
    {
      /* all others will be escaped */
      c1 = ((c & 0xf0) >> 4);
      c2 = (c & 0x0f);
      if (c1 < 10)
        c1 += '0';
      else
        c1 += 'A' - 10;
      if (c2 < 10)
        c2 += '0';
      else
        c2 += 'A' - 10;
      *outbuf++ = '%';
      *outbuf++ = c1;
      *outbuf++ = c2;
    }
  }
  *outbuf++='\0';
}

static void
amazon_get_cover (item_t *item, char *country, char *type)
{
  CURL *curl;
  char *info = NULL;
  exml_node_t *root_node, *node;
  char keywords[1024];
  char escaped_keywords[2048];
  char *x = NULL, *item_name = NULL;
  
  if (!item || !country || !type || item->cover)
    return;

  /* fill in keywords */
  switch (item->mrl->type)
  {
  case PLAYER_MRL_TYPE_AUDIO:
    memset (keywords, '\0', 1024);
    if (item->artist)
      strcat (keywords, item->artist);
    if (item->album)
    {
      strcat (keywords, " ");
      strcat (keywords, item->album);
    }
    if (!item->artist && !item->album)
    {
      item_name = strdup (basename (item->mrl->name));
      x = strrchr (item_name, '.');
      if (x)
        *x = '\0';
      strcat (keywords, item_name);
    }
    break;
  case PLAYER_MRL_TYPE_VIDEO:
    memset (keywords, '\0', 1024);
    item_name = strdup (basename (item->mrl->name));
    x = strrchr (item_name, '.');
    if (x)
      *x = '\0';
    strcat (keywords, item_name);
    break;
  default:
    break;
  }

  curl_global_init (CURL_GLOBAL_DEFAULT);
  curl = curl_easy_init ();

  url_escape_string (escaped_keywords, keywords);
  info = amazon_info_request (curl, country, type, escaped_keywords);
  if (!info)
  {
    curl_easy_cleanup (curl);
    curl_global_cleanup ();
    return;
  }

  /* parse XML answer */
  exml_parser_init (info, (int) strlen (info), XML_PARSER_CASE_SENSITIVE);
  exml_parser_build_tree (&root_node);

  node = exml_find_node (root_node, URL_THUMB_LARGE);
  if (node)
  {
    char file[1024];
    if (amazon_image_get (curl, node->data))
    {
      sprintf (file, "%s/%s", SAVE_PATH, basename (node->data));
      item->cover = strdup (file);
    }
  }

  exml_parser_free_tree (root_node);

  /* try to prevent searching again on amazon next time : save cover */
  if (item->cover && omc->cfg->save_cover)
  {
    char cover_file_name[1024];
    struct stat st;

    if (item->mrl->type == PLAYER_MRL_TYPE_AUDIO)
      sprintf (cover_file_name, "%s/%s", omc->cwd, DEFAULT_AUDIO_COVER_FILE);
    else if (item->mrl->type == PLAYER_MRL_TYPE_VIDEO)
      sprintf (cover_file_name, "%s/%s.png", omc->cwd, item_name);

    stat (item->cover, &st);

    /* check for file size : sometimes Amazon cover files are dummies
       only containing the "gif89a" reference. If the size is too small
       to be an image, there's no need to keep is saved */
    if (st.st_size > 10)
    {
      FILE *cover_file = NULL;
      
      /* a cover has been found : copy it to cwd */
      cover_file = fopen (cover_file_name, "w+");
      if (cover_file)
      {
        char *buf = NULL;
        int fd;
        
        fd = open (item->cover, O_RDONLY);
        buf = (char *) malloc ((size_t) ((st.st_size + 1) * sizeof (char)));
        read (fd, buf, (size_t) st.st_size);
        close (fd);
        fwrite (buf, (size_t) st.st_size, 1, cover_file);
        fclose (cover_file);
        free (buf);
      }
    }
  }
   
  curl_easy_cleanup (curl);
  curl_global_cleanup ();
}

static void *
th_cover_grabber (void *data)
{
  struct dirent **namelist;
  item_t *item = NULL;
  char *x = NULL, *item_name = NULL;
  int n, i;

  item = (item_t *) data;
  if (!item)
    return NULL;

  switch (item->mrl->type)
  {
  case PLAYER_MRL_TYPE_AUDIO:
    /* 1. Look for a FRONT or COVER file in current path. */
    n = scandir (omc->cwd, &namelist, 0, alphasort);
    if (n > 0)
    {
      for (i = 0; i < n; i++)
      {
        char *path = NULL;
        size_t len;

        /* already have a cover : stop looking for */
        if (item->cover)
        {
          free (namelist[i]);
          continue;
        }

        len = strlen (omc->cwd) + 1 + strlen (namelist[i]->d_name) + 1;
        path = (char *) malloc (len);
        if (!strcmp (omc->cwd, "/"))
          sprintf (path, "/%s", namelist[i]->d_name);
        else
          sprintf (path, "%s/%s", omc->cwd, namelist[i]->d_name);

        /* look for existing OMC cover file */
        if (!strcmp (namelist[i]->d_name, DEFAULT_AUDIO_COVER_FILE))
        {
          item->cover = strdup (path);
          free (namelist[i]);
          continue;
        }

        /* do not consider hidden files */
        if (namelist[i]->d_name[0] == '.')
        {
          free (namelist[i]);
          continue;
        }

        /* look for another potential cover file */
        if ((strcasestr (namelist[i]->d_name, "cover")
             || strcasestr (namelist[i]->d_name, "front"))
            && (strcasestr (namelist[i]->d_name, ".jpg")
                || strcasestr (namelist[i]->d_name, ".jpeg")
                || strcasestr (namelist[i]->d_name, ".png")))
        {
          item->cover = strdup (path);
        }
        free (namelist[i]);
      }
      free (namelist);
    }

    if (item->cover)
      break;
    
    /* 2. Look on Amazon for CD cover according to Artist and Album tags. */
    amazon_get_cover (item, AMAZON_COUNTRY_SEARCH_US, AMAZON_MEDIA_TYPE_MUSIC);
    break;
    
  case PLAYER_MRL_TYPE_VIDEO:
    /* 1. Look for a picture file with same name than movie file. */
    item_name = strdup (basename (item->mrl->name));
    x = strrchr (item_name, '.');
    if (x)
      *x = '\0';

    n = scandir (omc->cwd, &namelist, 0, alphasort);
    if (n > 0)
    {
      for (i = 0; i < n; i++)
      {
        char *path = NULL;
        size_t len;

        /* already have a cover : stop looking for */
        if (item->cover)
        {
          free (namelist[i]);
          continue;
        }

        len = strlen (omc->cwd) + 1 + strlen (namelist[i]->d_name) + 1;
        path = (char *) malloc (len);
        if (!strcmp (omc->cwd, "/"))
          sprintf (path, "/%s", namelist[i]->d_name);
        else
          sprintf (path, "%s/%s", omc->cwd, namelist[i]->d_name);

        /* do not consider hidden files */
        if (namelist[i]->d_name[0] == '.')
        {
          free (namelist[i]);
          continue;
        }

        /* look for a picture file with the same file name */
        if (strcasestr (namelist[i]->d_name, item_name))
        {
          if (strcasestr (namelist[i]->d_name, ".jpg")
              || strcasestr (namelist[i]->d_name, ".jpeg")
              || strcasestr (namelist[i]->d_name, ".png"))
          {
            item->cover = strdup (path);
          }
        }
        free (namelist[i]);
      }
      free (namelist);
    }

    if (item->cover)
    {
      free (item_name);
      break;
    }

    /* 2. Look on Amazon for DVD cover according to file name. */
    amazon_get_cover (item, AMAZON_COUNTRY_SEARCH_US, AMAZON_MEDIA_TYPE_DVD);
    free (item_name);
    break;
  case PLAYER_MRL_TYPE_IMAGE:
    /* no covers available for images */
    break;
  default:
    break;
  }

  if (item->cover)
  {
    struct stat st;
    stat (item->cover, &st);
    /* do not consider invalid cover files (too small to be consistent or
       dummy files retrieved from amazon) */
    if (st.st_size < 1024)
    {
      free (item->cover);
      item->cover = NULL;
    }
  }
  
  return NULL;
}

void
grab_file_covers (item_t *item)
{
  pthread_t th;
  
  if (!item)
    return;

  if (item->type != ITEM_TYPE_FILE)
    return;

  /* already have a cover : do not create a thread */
  if (item->cover)
    return;

  pthread_create (&th, NULL, th_cover_grabber, (void *) item);
  pthread_detach (th);
}
