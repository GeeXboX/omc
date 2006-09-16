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

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <Evas.h>
#include <Ecore.h>
#include <Ecore_Evas.h>
#include <player.h>

#include "screen.h"
#include "omc.h"
#include "widgets/widget.h"
#include "filter.h"
#include "screen_audio.h"
#include "screen_video.h"
#include "screen_image.h"

typedef struct video_s {
  Evas_Object *infobox;
  cover_t *cover;
  browser_t *browser;
  notifier_t *notifier;
} video_t;

static video_t *
video_new (void)
{
  video_t *video = NULL;

  video = (video_t *) malloc (sizeof (video_t));
  video->infobox = NULL;
  video->cover = NULL;
  video->browser = NULL;
  video->notifier = NULL;
  
  return video;
}

static void
video_free (video_t *video)
{
  if (!video)
    return;

  if (video->infobox)
    evas_object_del (video->infobox);
  if (video->cover)
    cover_free (video->cover);
  if (video->browser)
    browser_free (video->browser);
  if (video->notifier)
    notifier_free (video->notifier);
  free (video);
}

void
screen_video_update_infos (screen_t *screen, char *infos, int display)
{
  video_t *video = NULL;

  if (!screen || screen->type != SCREEN_TYPE_VIDEO)
    return;

  video = (video_t *) screen->private;
  if (!video)
    return;

  if (!video->infobox)
    return;

  if (display && infos)
    text_block_add_text (video->infobox, infos);
  else
    evas_object_textblock_clear (video->infobox);
}

static void
video_infos_setup (screen_t *screen)
{
  video_t *video = NULL;
  font_t *font = NULL;

  font = get_font (omc->cfg->fonts, "infos");
  if (!font)
    return;

  video = (video_t *) screen->private;
  if (!video)
    return;
  
  video->infobox =
    text_block_new (omc, 0, "68%", "7%", "30%", "25%", 0, font,
                    BLK_ALIGN_LEFT, BLK_ALIGN_TOP);
}

void
screen_video_update_cover (screen_t *screen, char *img, int display)
{
  video_t *video = NULL;
  cover_t *cover = NULL;
  
  if (!screen || screen->type != SCREEN_TYPE_VIDEO)
    return;

  video = (video_t *) screen->private;
  if (!video)
    return;

  cover = video->cover;
  if (!cover || !cover->cover)
    return;

  if (display && img)
  {
    Evas_List *list;

    if (cover->border)
    {
      for (list = cover->border; list; list = list->next)
      {
        Evas_Object *obj;
        
        obj = (Evas_Object *) list->data;
        evas_object_show (obj);
      }
    }

    if (cover->cover)
    {
      evas_object_image_file_set (cover->cover, img, NULL);
      evas_object_show (cover->cover);
    }
  }
  else
  {
    Evas_List *list;

    if (cover->border)
    {
      for (list = cover->border; list; list = list->next)
      {
        Evas_Object *obj;
        
        obj = (Evas_Object *) list->data;
        evas_object_hide (obj);
      }
    }

    if (cover->cover)
      evas_object_hide (cover->cover);
  }
}

static void
video_cover_setup (screen_t *screen)
{
  video_t *video = NULL;
  Evas_Object *dummy = NULL;
  
  video = (video_t *) screen->private;
  if (!video)
    return;

  video->cover = cover_new ();
  if (!video->cover)
    return;

  dummy = evas_object_image_add (omc->evas);
  video->cover->border = evas_list_append (video->cover->border, dummy);

  border_new (omc, video->cover->border,
              BORDER_TYPE_COVER, "68%", "35%", "30%", "58%");

  video->cover->cover =
    image_new (omc, 0, NULL, NULL, 0, "68%", "35%", "30%", "58%");
}

static void
video_browser_setup (screen_t *screen)
{
  video_t *video = NULL;
  font_t *font = NULL;

  font = get_font (omc->cfg->fonts, "browser");
  if (!font)
    return;

  video = (video_t *) screen->private;
  if (!video)
    return;
  
  video->browser =
    browser_new (omc->screen, font,
                 FILTER_TYPE_VIDEO, "13%", "7%", "50%", "85%");
}

void
screen_video_update_notifier (screen_t *screen,
                              char *cover, char *infos)
{
  video_t *video = NULL;
  notifier_t *notifier = NULL;
  
  if (!screen || screen->type != SCREEN_TYPE_VIDEO)
    return;

  video = (video_t *) screen->private;
  if (!video)
    return;

  notifier = video->notifier;
  if (!notifier)
    return;

  notifier_update (notifier, cover, infos);
}

static void
video_notifier_setup (screen_t *screen)
{
  video_t *video = NULL;
  font_t *font = NULL;

  font = get_font (omc->cfg->fonts, "infos");
  if (!font)
    return;
  
  video = (video_t *) screen->private;
  if (!video)
    return;

  video->notifier = notifier_new (omc, font, "40%", "83%", "100%", "10%");
}

void
screen_video_setup (screen_t *screen, char *data)
{
  if (!screen || screen->type != SCREEN_TYPE_VIDEO)
    return;

  screen->private = (video_t *) video_new ();
  
  widget_background_setup (screen);
  widget_common_toolbar_setup (screen);
  browser_filter_toolbar_setup (screen);
  
  video_infos_setup (screen);
  video_cover_setup (screen);
  video_browser_setup (screen);
  video_notifier_setup (screen);
}

void
screen_video_display (screen_t *screen)
{
  video_t *video = (video_t *) screen->private;

  if (!video)
    return;
  
  if (video->infobox)
    evas_object_show (video->infobox);
}

void
screen_video_free (screen_t *screen)
{
  video_t *video = NULL;
  
  if (!screen || screen->type != SCREEN_TYPE_VIDEO)
    return;

  video = (video_t *) screen->private;
  if (video)
    video_free (video);
}
