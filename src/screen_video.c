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

#include "omc.h"
#include "widget.h"
#include "screen.h"
#include "filter.h"
#include "screen_audio.h"
#include "screen_video.h"
#include "screen_image.h"

struct video_t {
  Evas_Object *infobox;
  struct browser_t *browser;
  struct notifier_t *notifier;
};

static struct video_t *
video_new (void)
{
  struct video_t *video = NULL;

  video = (struct video_t *) malloc (sizeof (struct video_t));
  video->infobox = NULL;
  video->browser = NULL;
  video->notifier = NULL;
  
  return video;
}

static void
video_free (struct video_t *video)
{
  if (!video)
    return;

  if (video->infobox)
    evas_object_del (video->infobox);
  if (video->browser)
    browser_free (video->browser);
  if (video->notifier)
    notifier_free (video->notifier);
  free (video);
}

void
screen_video_update_infos (struct screen_t *screen, char *infos, int display)
{
  struct video_t *video = NULL;

  if (!screen || screen->type != SCREEN_TYPE_VIDEO)
    return;

  video = (struct video_t *) screen->private;
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
video_infos_setup (struct screen_t *screen)
{
  struct video_t *video = NULL;
  struct font_t *font = NULL;

  font = get_font (omc->cfg->fonts, "infos");
  if (!font)
    return;

  video = (struct video_t *) screen->private;
  if (!video)
    return;
  
  video->infobox =
    text_block_new (omc, 0, "72%", "20%", "27%", "30%", 0, font,
                    BLK_ALIGN_LEFT, BLK_ALIGN_TOP);
}

static void
video_browser_setup (struct screen_t *screen)
{
  struct video_t *video = NULL;
  struct font_t *font = NULL;

  font = get_font (omc->cfg->fonts, "browser");
  if (!font)
    return;

  video = (struct video_t *) screen->private;
  if (!video)
    return;
  
  video->browser =
    browser_new (omc->screen, font,
                 FILTER_TYPE_VIDEO, "13%", "7%", "50%", "85%");
}

void
screen_video_update_notifier (struct screen_t *screen,
                              char *cover, char *infos)
{
  struct video_t *video = NULL;
  struct notifier_t *notifier = NULL;
  
  if (!screen || screen->type != SCREEN_TYPE_VIDEO)
    return;

  video = (struct video_t *) screen->private;
  if (!video)
    return;

  notifier = video->notifier;
  if (!notifier)
    return;

  notifier_update (notifier, cover, infos);
}

static void
video_notifier_setup (struct screen_t *screen)
{
  struct video_t *video = NULL;
  struct font_t *font = NULL;

  font = get_font (omc->cfg->fonts, "infos");
  if (!font)
    return;
  
  video = (struct video_t *) screen->private;
  if (!video)
    return;

  video->notifier = notifier_new (omc, font, "40%", "83%", "100%", "10%");
}

void
screen_video_setup (struct screen_t *screen)
{
  if (!screen || screen->type != SCREEN_TYPE_VIDEO)
    return;

  screen->private = (struct video_t *) video_new ();
  
  widget_background_setup (screen);
  widget_common_toolbar_setup (screen);
  browser_filter_toolbar_setup (screen);
  
  video_infos_setup (screen);
  video_browser_setup (screen);
  video_notifier_setup (screen);
}

void
screen_video_display (struct screen_t *screen)
{
  struct video_t *video = (struct video_t *) screen->private;

  if (!video)
    return;
  
  if (video->infobox)
    evas_object_show (video->infobox);
}

void
screen_video_free (struct screen_t *screen)
{
  struct video_t *video = NULL;
  
  if (!screen || screen->type != SCREEN_TYPE_VIDEO)
    return;

  video = (struct video_t *) screen->private;
  if (video)
    video_free (video);
}
