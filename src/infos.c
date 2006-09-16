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
#include <ctype.h>
#include <fcntl.h>
#include <xine.h>
#include <pthread.h>
#include <player.h>

#include "screen.h"
#include "omc.h"
#include "widget.h"
#include "avplayer.h"

#define MAX_TAG_SIZE 1024

static void
grab_file_title (item_t *item, xine_stream_t *stream)
{
  const char *title = NULL;
  
  if (!item || !stream || !item->mrl)
    return;

  /* Title or name */
  title = xine_get_meta_info (stream, XINE_META_INFO_TITLE);
  if (title)
    strcat (item->infos, (char *) title);
  else
    strcat (item->infos, basename (item->mrl->name));
  strcat (item->infos, "\n");
}

static void
grab_file_size (item_t *item)
{
  struct stat st;
  char tag[MAX_TAG_SIZE];
  
  if (!item || !item->mrl)
    return;

  stat (item->mrl->name, &st);
  memset (tag, '\0', MAX_TAG_SIZE);
  sprintf (tag, "Size : %.2f MB\n", (float) st.st_size / 1024 / 1024);
  strcat (item->infos, tag);
  strcat (item->infos, "\n");
}

static void
grab_audio_file_info (item_t *item, xine_stream_t *stream)
{
  char tag[MAX_TAG_SIZE];
  
  if (!item || !stream || !item->mrl)
    return;

  /* Title or name */
  grab_file_title (item, stream);
  
  if (xine_get_stream_info (stream, XINE_STREAM_INFO_HAS_AUDIO))
  {
    const char *artist = NULL, *album = NULL;
    const char *year = NULL, *track = NULL, *codec = NULL;
    uint32_t b = 0;

    /* ID3 tags info */
    memset (tag, '\0', MAX_TAG_SIZE);
    artist = xine_get_meta_info (stream, XINE_META_INFO_ARTIST);
    if (artist)
    {
      sprintf (tag, "%s", artist);
      item->artist = strdup (artist);
    }

    album = xine_get_meta_info (stream, XINE_META_INFO_ALBUM);
    if (album)
    {
      if (artist)
        strcat (tag, " - ");
      strcat (tag, album);
      strcat (tag, " ");
      item->album = strdup (album);
    }

    year = xine_get_meta_info (stream, XINE_META_INFO_YEAR);
    if (year)
    {
      if (album || artist)
        strcat (tag, "(");
      strcat (tag, year);
      if (album || artist)
        strcat (tag, ") ");
    }

    track = xine_get_meta_info (stream, XINE_META_INFO_TRACK_NUMBER);
    if (track)
    {
      if (year || album || artist)
        strcat (tag, "- ");
      strcat (tag, "Track #");
      strcat (tag, track);
    }
    if (artist || album || year || track)
    {
       strcat (item->infos, tag);
       strcat (item->infos, "\n");
    }
    
    memset (tag, '\0', MAX_TAG_SIZE);
    codec = xine_get_meta_info (stream, XINE_META_INFO_AUDIOCODEC);
    if (codec)
      sprintf (tag, "%s", codec);

    b = xine_get_stream_info (stream, XINE_STREAM_INFO_AUDIO_BITRATE);
    {
      char bit[128];
      
      if (codec)
        strcat (tag, " - ");
      sprintf (bit, "%d kbps", (int) (b / 1000));
      strcat (tag, bit);
    }
    strcat (item->infos, tag);
    strcat (item->infos, "\n");
  }

  grab_file_size (item);
}

static void
grab_video_file_info (item_t *item, xine_stream_t *stream)
{
  char tag[MAX_TAG_SIZE];
  
  if (!item || !stream || !item->mrl)
    return;

  /* Title or name */
  grab_file_title (item, stream);

  if (xine_get_stream_info (stream, XINE_STREAM_INFO_HAS_VIDEO))
  {
    const char *codec = NULL;
    uint32_t w, h;

    w = xine_get_stream_info (stream, XINE_STREAM_INFO_VIDEO_WIDTH);
    h = xine_get_stream_info (stream, XINE_STREAM_INFO_VIDEO_HEIGHT);
    memset (tag, '\0', MAX_TAG_SIZE);
    sprintf (tag, "Res. : %d x %d", w, h);
    strcat (item->infos, tag);
    strcat (item->infos, "\n");

    codec =  xine_get_meta_info (stream, XINE_META_INFO_VIDEOCODEC);
    memset (tag, '\0', MAX_TAG_SIZE);
    sprintf (tag, "Video : %s", codec);
    strcat (item->infos, tag);
    strcat (item->infos, "\n");
  }

  if (xine_get_stream_info (stream, XINE_STREAM_INFO_HAS_AUDIO))
  {
    const char *codec = NULL;

    codec = xine_get_meta_info (stream, XINE_META_INFO_AUDIOCODEC);
    memset (tag, '\0', MAX_TAG_SIZE);
    sprintf (tag, "Audio : %s", codec);
    strcat (item->infos, tag);
    strcat (item->infos, "\n");
  }

  grab_file_size (item);
}

static void *
th_info_grabber (void *data)
{
  item_t *item = NULL;
  xine_stream_t *stream = NULL;
  
  item = (item_t *) data;
  if (!item)
    return NULL;

  stream = xine_stream_new (omc->player->xine, NULL, NULL);
  if (!stream)
    return NULL;

  if (!item->infos)
  {
    item->infos = (char *) malloc (1024 * sizeof (char));
    memset (item->infos, '\0', 1024);
  }
  
  xine_open (stream, item->mrl->name);

  switch (item->mrl->type)
  {
  case PLAYER_MRL_TYPE_AUDIO:
    grab_audio_file_info (item, stream);
    break;
  case PLAYER_MRL_TYPE_VIDEO:
    grab_video_file_info (item, stream);
    break;
  case PLAYER_MRL_TYPE_IMAGE:
    break;
  default:
    break;
  }

  xine_close (stream);
  xine_dispose (stream);
  
  return NULL;
}

void
grab_file_infos (item_t *item)
{
  pthread_t th;
  
  if (!item)
    return;

  if (item->type != ITEM_TYPE_FILE)
    return;

  pthread_create (&th, NULL, th_info_grabber, (void *) item);
  pthread_detach (th);
}
