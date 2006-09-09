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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

#include "exmlparser.h"
#include "config.h"
#include "langinfo.h"

static char *
get_lang_file (char *lang)
{
  char path[1024];
  struct stat st;

  if (!lang)
    return NULL;
  
  memset (path, '\0', 1024);
  sprintf (path, "%s/langs/%s/langinfo.xml", CFG_DIR, lang);
  printf ("Looking for language information in %s\n", path);
  if ((stat (path, &st) == 0) && S_ISREG (st.st_mode))
    return strdup (path);

  /* no lang file can be found : drop it */
  return NULL;
}

static char *
get_strings_file (char *lang)
{
  char path[1024];
  struct stat st;

  if (!lang)
    return NULL;
  
  memset (path, '\0', 1024);
  sprintf (path, "%s/langs/%s/strings.xml", CFG_DIR, lang);
  printf ("Looking for language strings definitions in %s\n", path);
  if ((stat (path, &st) == 0) && S_ISREG (st.st_mode))
    return strdup (path);

  /* no lang file can be found : drop it */
  return NULL;
}

static lang_country_t *
lang_country_new (void)
{
  lang_country_t *country = NULL;

  country = (lang_country_t *) malloc (sizeof (lang_country_t));
  country->code = NULL;
  country->description = NULL;

  return country;
}

static void
lang_country_free (lang_country_t *country)
{
  if (!country)
    return;

  if (country->code)
    free (country->code);
  if (country->description)
    free (country->description);
  free (country);
}

static lang_charset_t *
lang_charset_new (void)
{
  lang_charset_t *set = NULL;

  set = (lang_charset_t *) malloc (sizeof (lang_charset_t));
  set->gui = NULL;
  set->subs = NULL;

  return set;
}

static void
lang_charset_free (lang_charset_t *set)
{
  if (!set)
    return;

  if (set->gui)
    free (set->gui);
  if (set->subs)
    free (set->subs);
  free (set);
}

static lang_dvd_t *
lang_dvd_new (void)
{
  lang_dvd_t *dvd = NULL;

  dvd = (lang_dvd_t *) malloc (sizeof (lang_dvd_t));
  dvd->menu = NULL;
  dvd->audio = NULL;
  dvd->subs = NULL;

  return dvd;
}

static void
lang_dvd_free (lang_dvd_t *dvd)
{
  if (!dvd)
    return;

  if (dvd->menu)
    free (dvd->menu);
  if (dvd->audio)
    free (dvd->audio);
  if (dvd->subs)
    free (dvd->subs);
  free (dvd);
}

static region_date_t *
region_date_new (void)
{
  region_date_t *date = NULL;

  date = (region_date_t *) malloc (sizeof (region_date_t));
  date->first = -1;
  date->second = -1;
  date->third = -1;
  date->fourth = -1;
  
  return date;
}

static void
region_date_free (region_date_t *date)
{
  if (date)
    free (date);
}

static lang_region_t *
lang_region_new (void)
{
  lang_region_t *region = NULL;

  region = (lang_region_t *) malloc (sizeof (lang_region_t));
  region->name = NULL;
  region->date_short = NULL;
  region->date_long = NULL;
  region->time = TIME_24H;
  region->temperature = TEMPERATURE_CELSIUS;
  region->speed = SPEED_KMH;
  
  return region;
}

static void
lang_region_free (lang_region_t *region)
{
  if (!region)
    return;

  if (region->name)
    free (region->name);
  if (region->date_short)
    region_date_free (region->date_short);
  if (region->date_long)
    region_date_free (region->date_long);
  
  free (region);
}

static int
get_list_length (void *list)
{
  void **l = list;
  int n = 0;

  while (*l++)
    n++;

  return n;
}

static lang_regions_t *
lang_regions_new (void)
{
  lang_regions_t *regions = NULL;

  regions = (lang_regions_t *) malloc (sizeof (lang_regions_t));

  regions->list = (lang_region_t **) malloc (sizeof (lang_region_t *));
  *(regions->list) = NULL;

  return regions;
}

static void
lang_regions_free (lang_regions_t *regions)
{
  lang_region_t **list;

  if (!regions)
    return;

  for (list = regions->list; *list; *list++)
    lang_region_free (*list);

  free (regions->list);
}

static void
lang_regions_add (lang_regions_t *regions, lang_region_t *child)
{
  lang_region_t **list;
  int n;

  if (!regions || !child)
    return;

  for (list = regions->list; *list; *list++)
    if (*list == child)
      return;

  n = get_list_length ((void *) regions->list) + 1;
  regions->list = (lang_region_t **)
    realloc (regions->list, (n + 1) * sizeof (*(regions->list)));
  regions->list[n] = NULL;
  regions->list[n - 1] = child;
}

static lang_string_t *
lang_string_new (void)
{
  lang_string_t *string = NULL;

  string = (lang_string_t *) malloc (sizeof (lang_string_t));
  string->id = NULL;
  string->str = NULL;
  
  return string;
}

static void
lang_string_free (lang_string_t *string)
{
  if (!string)
    return;

  if (string->id)
    free (string->id);
  if (string->str)
    free (string->str);
  
  free (string);
}

static lang_strings_t *
lang_strings_new (void)
{
  lang_strings_t *strings = NULL;

  strings = (lang_strings_t *) malloc (sizeof (lang_strings_t));
  strings->list = (lang_string_t **) malloc (sizeof (lang_string_t *));
  *(strings->list) = NULL;

  return strings;
}

static void
lang_strings_free (lang_strings_t *strings)
{
  lang_string_t **list;

  if (!strings)
    return;

  for (list = strings->list; *list; *list++)
    lang_string_free (*list);

  free (strings->list);
}

static void
lang_strings_add (lang_strings_t *strings, lang_string_t *child)
{
  lang_string_t **list;
  int n;

  if (!strings || !child)
    return;

  for (list = strings->list; *list; *list++)
    if (*list == child)
      return;

  n = get_list_length ((void *) strings->list) + 1;
  strings->list = (lang_string_t **)
    realloc (strings->list, (n + 1) * sizeof (*(strings->list)));
  strings->list[n] = NULL;
  strings->list[n - 1] = child;
}

static lang_info_t *
lang_info_new (void)
{
  lang_info_t *info = NULL;

  info = (lang_info_t *) malloc (sizeof (lang_info_t));
  info->country = NULL;
  info->charset = NULL;
  info->dvd = NULL;
  info->regions = NULL;
  info->strings = NULL;
  
  return info;
}

void
lang_info_free (lang_info_t *info)
{
  if (!info)
    return;

  if (info->country)
    lang_country_free (info->country);
  if (info->charset)
    lang_charset_free (info->charset);
  if (info->dvd)
    lang_dvd_free (info->dvd);
  if (info->regions)
    lang_regions_free (info->regions);
  if (info->strings)
    lang_strings_free (info->strings);
  free (info);
}

#define NODE_COUNTRY_NAME "country"
#define NODE_COUNTRY_CODE "code"
#define NODE_COUNTRY_DESCRIPTION "description"

static void
parse_node_country (lang_info_t *info, xml_node_t *node)
{
  lang_country_t *country;
  xml_node_t *n;
  
  if (!info || !node)
    return;

  country = lang_country_new ();
  n = node->child;
  while (n)
  {
    if (!n->name || !n->data)
    {
      n = n->next;
      continue;
    }
    
    if (!strcmp (n->name, NODE_COUNTRY_CODE))
      country->code = strdup (n->data);
    else if (!strcmp (n->name, NODE_COUNTRY_DESCRIPTION))
      country->description = strdup (n->data);
    n = n->next;
  }

  info->country = country;
}

#define NODE_CHARSET_NAME "charset"
#define NODE_CHARSET_GUI "gui"
#define NODE_CHARSET_SUBTITLE "subtitle"

static void
parse_node_charset (lang_info_t *info, xml_node_t *node)
{
  lang_charset_t *set;
  xml_node_t *n;
  
  if (!info || !node)
    return;

  set = lang_charset_new ();
  n = node->child;
  while (n)
  {
    if (!n->name || !n->data)
    {
      n = n->next;
      continue;
    }
    
    if (!strcmp (n->name, NODE_CHARSET_GUI))
      set->gui = strdup (n->data);
    else if (!strcmp (n->name, NODE_CHARSET_SUBTITLE))
      set->subs = strdup (n->data);
    n = n->next;
  }

  info->charset = set;
}

#define NODE_DVD_NAME "dvd"
#define NODE_DVD_MENU "gui"
#define NODE_DVD_AUDIO "audio"
#define NODE_DVD_SUBTITLE "subtitle"

static void
parse_node_dvd (lang_info_t *info, xml_node_t *node)
{
  lang_dvd_t *dvd;
  xml_node_t *n;
  
  if (!info || !node)
    return;

  dvd = lang_dvd_new ();
  n = node->child;
  while (n)
  {
    if (!n->name || !n->data)
    {
      n = n->next;
      continue;
    }
    
    if (!strcmp (n->name, NODE_DVD_MENU))
      dvd->menu = strdup (n->data);
    else if (!strcmp (n->name, NODE_DVD_AUDIO))
      dvd->audio = strdup (n->data);
    else if (!strcmp (n->name, NODE_DVD_SUBTITLE))
      dvd->subs = strdup (n->data);
    n = n->next;
  }

  info->dvd = dvd;
}

#define NODE_REGION_NAME "region"
#define NODE_REGION_PROP_NAME "name"
#define NODE_REGION_DATE_SHORT "dateshort"
#define NODE_REGION_DATE_LONG "datelong"
#define NODE_REGION_TIME "timeunit"
#define NODE_REGION_TEMPERATURE "tempunit"
#define NODE_REGION_SPEED "speedunit"

static void
parse_node_region (lang_regions_t *regions, xml_node_t *node)
{
  lang_region_t *r;
  xml_node_t *n;
  struct xml_property_s *props;
  
  if (!regions || !node)
    return;

  r = lang_region_new ();
  
  props = node->props;
  while (props)
  {
    if (!props->name || !props->value)
      props = props->next;

    if (!strcmp (props->name, NODE_REGION_PROP_NAME))
    {
      r->name = strdup (props->value);
      break;
    }
    
    props = props->next;
  }

  if (!r->name)
  {
    lang_region_free (r);
    return;
  }
  
  n = node->child;
  while (n)
  {
    if (!n->name || !n->data)
    {
      n = n->next;
      continue;
    }
    
    if (!strcmp (n->name, NODE_REGION_DATE_SHORT))
    {
      region_date_t *date;
      date = region_date_new ();
      /* parsing not yet implemented */
      r->date_short = date;
    }
    else if (!strcmp (n->name, NODE_REGION_DATE_LONG))
    {
      region_date_t *date;
      date = region_date_new ();
      /* parsing not yet implemented */
      r->date_long = date;
    }
    else if (!strcmp (n->name, NODE_REGION_TIME))
    {
      if (!strcmp (n->data, "12"))
        r->time = TIME_12H;
      else if (!strcmp (n->data, "24"))
        r->time = TIME_24H;
    }
    else if (!strcmp (n->name, NODE_REGION_TEMPERATURE))
    {
      if (!strcmp (n->data, "C"))
        r->temperature = TEMPERATURE_CELSIUS;
      else if (!strcmp (n->data, "F"))
        r->temperature = TEMPERATURE_FARENHEIT;
    }
    else if (!strcmp (n->name, NODE_REGION_SPEED))
    {
      if (!strcmp (n->data, "kmh"))
        r->speed = SPEED_KMH;
      else if (!strcmp (n->data, "F"))
        r->speed = SPEED_MPH;
    }

    n = n->next;
  }
  
  lang_regions_add (regions, r);
}

#define NODE_REGIONS_NAME "regions"

static void
parse_node_regions (lang_info_t *info, xml_node_t *node)
{
  lang_regions_t *regions;
  xml_node_t *n;
  
  if (!info || !node)
    return;

  regions = lang_regions_new ();
  n = node->child;
  while (n)
  {
    if (!n->name || !n->data)
    {
      n = n->next;
      continue;
    }
    
    if (!strcmp (n->name, NODE_REGION_NAME))
      parse_node_region (regions, n);
    n = n->next;
  }

  info->regions = regions;
}

#define ROOT_NODE_NAME "language"

lang_info_t *
lang_info_parse (char *lang)
{
  lang_info_t *info = NULL;
  xml_node_t *root, *node;
  char *file, *buffer;
  struct stat st;
  int fd;
  
  file = get_lang_file (lang);
  if (!file)
    return NULL;

  stat (file, &st);
  fd = open (file, O_RDONLY);
  if (fd < 0)
    return NULL;

  buffer = (char *) malloc (st.st_size + 1);
  memset (buffer, '\0', (size_t) (st.st_size + 1));
  read (fd, buffer, (size_t) st.st_size);
  close (fd);
  free (file);
  
  exml_parser_init (buffer, (int) strlen (buffer), XML_PARSER_CASE_SENSITIVE);
  exml_parser_build_tree (&root);
  free (buffer);
  
  if (strcmp (root->name, ROOT_NODE_NAME) != 0)
  {
    exml_parser_free_tree (root);
    return NULL;
  }

  info = lang_info_new ();
  node = root->child;
  while (node)
  {
    if (!node->name)
    {
      node = node->next;
      continue;
    }
    
    if (!strcmp (node->name, NODE_COUNTRY_NAME))
      parse_node_country (info, node);
    else if (!strcmp (node->name, NODE_CHARSET_NAME))
      parse_node_charset (info, node);
    else if (!strcmp (node->name, NODE_DVD_NAME))
      parse_node_dvd (info, node);
    else if (!strcmp (node->name, NODE_REGIONS_NAME))
      parse_node_regions (info, node);
            
    node = node->next;
  }

  exml_parser_free_tree (root);

  return info;
}

#define ROOT_NODE_STRINGS_NAME "strings"
#define NODE_STRING_NAME "string"
#define NODE_STRING_ID "id"

void
lang_strings_parse (lang_info_t *lang)
{
  xml_node_t *root, *node;
  lang_strings_t *strings;
  char *file, *buffer;
  struct stat st;
  int fd;
  
  if (!lang || !lang->country || !lang->country->code)
    return;
  file = get_strings_file (lang->country->code);
  if (!file)
    return;

  stat (file, &st);
  fd = open (file, O_RDONLY);
  if (fd < 0)
    return;

  buffer = (char *) malloc (st.st_size + 1);
  memset (buffer, '\0', (size_t) (st.st_size + 1));
  read (fd, buffer, (size_t) st.st_size);
  close (fd);
  free (file);
  
  exml_parser_init (buffer, (int) strlen (buffer), XML_PARSER_CASE_SENSITIVE);
  exml_parser_build_tree (&root);
  free (buffer);
  
  if (strcmp (root->name, ROOT_NODE_STRINGS_NAME) != 0)
  {
    exml_parser_free_tree (root);
    return;
  }

  strings = lang_strings_new ();
  node = root->child;
  while (node)
  {
    struct xml_property_s *props;
    lang_string_t *string;
    
    if (!node->name || !node->data)
    {
      node = node->next;
      continue;
    }

    if (strcmp (node->name, NODE_STRING_NAME) != 0)
    {
      node = node->next;
      continue;
    }

    string = lang_string_new ();
    string->str = strdup (node->data);
    
    props = node->props;
    while (props)
    {
      if (!props->name || !props->value)
      {
        props = props->next;
        continue;
      }

      if (!strcmp (props->name, NODE_STRING_ID))
      {
        string->id = strdup (props->value);
        break;
      }
    
      props = props->next;
    }

    if (!string->id)
    {
      lang_string_free (string);
      node = node->next;
      continue;
    }
    
    lang_strings_add (strings, string);
    node = node->next;
  }

  exml_parser_free_tree (root);
  lang->strings = strings;
}

const char *
lang_get_string (lang_info_t *lang, char *id)
{
  lang_string_t **list;

  if (!lang || !id)
    return NULL;
  
  for (list = lang->strings->list; *list; *list++)
  {
    lang_string_t *string = (lang_string_t *) *list;
    if (!strcmp (string->id, id))
      return string->str;
  }

  return NULL;
}
