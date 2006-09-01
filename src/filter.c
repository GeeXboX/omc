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

#include <Evas.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "filter.h"

// Xine Supported Extensions : anx axa axv spx wvx wax mng wve cin mve mv8 vqa roq str iki ik2 dps xas xap cpk cak 4xm vmd nsf voc trp pes pva nsv iff svx 8svx 16sv ilbm ham ham6 ham8 anim anim3 anim5 anim7 anim8 dif mp+ shn mpv asc txt sub srt smi ssa

static void
filter_add_extension (filter_t *filter, char *extension)
{
  if (!filter || !extension)
    return;
  
  filter->exts = evas_list_append (filter->exts, strdup (extension));
}

static void
filter_audio_create (filter_t *filter)
{
  if (!filter)
    return;

  filter_add_extension (filter, "aac");
  filter_add_extension (filter, "ac3");
  filter_add_extension (filter, "aif");
  filter_add_extension (filter, "aiff");
  filter_add_extension (filter, "au");
  filter_add_extension (filter, "aud");
  filter_add_extension (filter, "asx");
  filter_add_extension (filter, "dts");
  filter_add_extension (filter, "flac");
  filter_add_extension (filter, "m4a");
  filter_add_extension (filter, "mp2");
  filter_add_extension (filter, "mp3");
  filter_add_extension (filter, "mpa");
  filter_add_extension (filter, "mpega");
  filter_add_extension (filter, "mpc");
  filter_add_extension (filter, "ogg");
  filter_add_extension (filter, "ra");
  filter_add_extension (filter, "ram");
  filter_add_extension (filter, "snd");
  filter_add_extension (filter, "vox");
  filter_add_extension (filter, "wav");
  filter_add_extension (filter, "wma");
  filter_add_extension (filter, "xa");
  filter_add_extension (filter, "xa1");
  filter_add_extension (filter, "xa2");
}

static void
filter_video_create (filter_t *filter)
{
  if (!filter)
    return;

  filter_add_extension (filter, "asf");
  filter_add_extension (filter, "avi");
  filter_add_extension (filter, "dat");
  filter_add_extension (filter, "dv");
  filter_add_extension (filter, "film");
  filter_add_extension (filter, "fli");
  filter_add_extension (filter, "flc");
  filter_add_extension (filter, "flv");
  filter_add_extension (filter, "m2t");
  filter_add_extension (filter, "mjpg");
  filter_add_extension (filter, "mkv");
  filter_add_extension (filter, "mov");
  filter_add_extension (filter, "mp4");
  filter_add_extension (filter, "mpeg");
  filter_add_extension (filter, "mpg");
  filter_add_extension (filter, "ogm");
  filter_add_extension (filter, "qt");
  filter_add_extension (filter, "rm");
  filter_add_extension (filter, "rmvb");
  filter_add_extension (filter, "ts");
  filter_add_extension (filter, "vob");
  filter_add_extension (filter, "wmv");
  filter_add_extension (filter, "y4m");
}

static void
filter_image_create (filter_t *filter)
{
  if (!filter)
    return;

  filter_add_extension (filter, "bmp");
  filter_add_extension (filter, "gif");
  filter_add_extension (filter, "jpeg");
  filter_add_extension (filter, "jpg");
  filter_add_extension (filter, "png");
}

filter_t *
filter_new (int type)
{
  filter_t *filter = NULL;

  if (type == FILTER_TYPE_NONE)
    return NULL;
  
  filter = (filter_t *) malloc (sizeof (filter_t));
  filter->type = type;

  switch (filter->type)
  {
  case FILTER_TYPE_AUDIO:
    filter_audio_create (filter);
    break;
  case FILTER_TYPE_VIDEO:
    filter_video_create (filter);
    break;
  case FILTER_TYPE_IMAGE:
    filter_image_create (filter);
    break;
  }

  return filter;
}

void
filter_free (filter_t *filter)
{
  Evas_List *list = NULL;

  if (!filter)
    return;

  for (list = filter->exts; list; list = list->next)
  {
    char *ext = NULL;

    ext = (char *) list->data;
    if (ext)
      free (ext);
    filter->exts = evas_list_remove (filter->exts, filter->exts);
  }

  free (filter);
}

int
filter_supports_extension (filter_t *filter, char *extension)
{
  Evas_List *list = NULL;

  if (!filter || !extension)
    return -1;

  for (list = filter->exts; list; list = list->next)
  {
    char *ext = NULL;

    ext = (char *) list->data;
    if (ext && strcasecmp (ext, extension) == 0)
      return 1;
  }
  
  return 0;
}

filter_t *
filter_get (Evas_List *filter_list, int type)
{
  Evas_List *list = NULL;
 
  for (list = filter_list; list; list = list->next)
  {
    filter_t *filter = NULL;

    filter = (filter_t *) list->data;
    if (filter && filter->type == type)
      return filter;
  }
  
  return NULL;
}

