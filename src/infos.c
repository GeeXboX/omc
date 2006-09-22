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
#include <string.h>
#include <stdlib.h>
#include <libgen.h>

#include "omc.h"

#define MAX_TAG_SIZE 1024
#define MAX_INFO_SIZE 2048

void
compute_file_infos (item_t *item)
{
  mrl_t *mrl;
  char tag[MAX_TAG_SIZE];
  
  if (!item || item->type != ITEM_TYPE_FILE || !item->updated || item->infos)
    return;

  mrl = item->mrl;
  item->infos = malloc (MAX_INFO_SIZE);
  memset (item->infos, '\0', MAX_INFO_SIZE);

  if (mrl->meta && mrl->meta->title)
    strcat (item->infos, mrl->meta->title);
  else
    strcat (item->infos, basename (item->mrl->name));
  strcat (item->infos, "\n");

  memset (tag, '\0', MAX_TAG_SIZE);
  sprintf (tag, "Size : %.2f MB\n", (float) mrl->prop->size / 1024 / 1024);
  strcat (item->infos, tag);
  strcat (item->infos, "\n");

  switch (item->mrl->type)
   {
   case PLAYER_MRL_TYPE_AUDIO:
   {
    uint32_t prev = 0;
    char bit[128];
    
    if (mrl->meta && mrl->meta->artist)
    {
      strcat (item->infos, mrl->meta->artist);
      prev = 1;
    }

    if (mrl->meta && mrl->meta->album)
    {
      if (prev)
        strcat (item->infos, " - ");
      strcat (item->infos, mrl->meta->album);
      strcat (item->infos, " ");
      prev = 1;
    }

    if (mrl->meta && mrl->meta->year)
    {
      if (prev)
        strcat (item->infos, "(");
      strcat (item->infos, mrl->meta->year);
      if (prev)
        strcat (tag, ") ");
      prev = 1;
    }

    if (mrl->meta && mrl->meta->track)
    {
      if (prev)
        strcat (item->infos, "- ");
      strcat (item->infos, "Track #");
      strcat (item->infos, mrl->meta->track);
      prev = 1;
    }
    
    if (prev)
      strcat (item->infos, "\n");
    
    if (mrl->prop && mrl->prop->audio && mrl->prop->audio->codec)
    {
      strcat (item->infos, mrl->prop->audio->codec);
      prev = 1;
    }

    if (mrl->prop && mrl->prop->audio && mrl->prop->audio->bitrate)
    { 
      if (prev)
        strcat (item->infos, " - ");
      sprintf (bit, "%d kbps", (int) (mrl->prop->audio->bitrate / 1000));
      strcat (item->infos, bit);
    }
    strcat (item->infos, "\n");
   }
   break;
   case PLAYER_MRL_TYPE_VIDEO:
   {
     if (mrl->prop && mrl->prop->video)
     {
       if (mrl->prop->video->width && mrl->prop->video->height)
       {
         memset (tag, '\0', MAX_TAG_SIZE);
         sprintf (tag, "Res. : %d x %d",
                  mrl->prop->video->width, mrl->prop->video->height);
         strcat (item->infos, tag);
         strcat (item->infos, "\n");
       }

       if (mrl->prop->video->codec)
       {
         memset (tag, '\0', MAX_TAG_SIZE);
         sprintf (tag, "Video : %s", mrl->prop->video->codec);
         strcat (item->infos, tag);
         strcat (item->infos, "\n");
       }
     }
     
     if (mrl->prop && mrl->prop->audio && mrl->prop->audio->codec)
     {
       memset (tag, '\0', MAX_TAG_SIZE);
       sprintf (tag, "Audio : %s", mrl->prop->audio->codec);
       strcat (item->infos, tag);
       strcat (item->infos, "\n");
     }
   }
   break;
   default:
     break;
   }
}
