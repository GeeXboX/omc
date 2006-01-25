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
#include <Ecore.h>
#include <Ecore_Evas.h>

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "omc.h"
#include "filter.h"
#include "widget.h"
#include "screen.h"
#include "screen_audio.h"
#include "screen_video.h"
#include "screen_image.h"
#include "screen_aplayer.h"

struct audio_t {
  Evas_Object *infobox;
  struct cover_t *cover;
  struct browser_t *browser;
  struct notifier_t *notifier;
};

static struct audio_t *
audio_new (void)
{
  struct audio_t *audio = NULL;

  audio = (struct audio_t *) malloc (sizeof (struct audio_t));
  audio->infobox = NULL;
  audio->cover = NULL;
  audio->browser = NULL;
  audio->notifier = NULL;
  
  return audio;
}

static void
audio_free (struct audio_t *audio)
{
  if (!audio)
    return;

  if (audio->infobox)
    evas_object_del (audio->infobox);
  if (audio->cover)
    cover_free (audio->cover);
  if (audio->browser)
    browser_free (audio->browser);
  if (audio->notifier)
    notifier_free (audio->notifier);
  free (audio);
}

#define IMG_ICON_PLAYER_AUDIO OMC_DATA_DIR"/player_audio.png"
#define IMG_ICON_PLAYER_AUDIO_FOCUS OMC_DATA_DIR"/player_audio_focus.png"

static void
audio_toolbar_setup (struct screen_t *screen)
{
  Evas_Object *obj = NULL;
  
  obj = image_new (omc, 1, IMG_ICON_PLAYER_AUDIO, IMG_ICON_PLAYER_AUDIO_FOCUS,
                   0, "0%", "75%", "65", "65");
  screen->objects = evas_list_append (screen->objects, obj);
  object_add_default_cb (obj);
  evas_object_event_callback_add (obj, EVAS_CALLBACK_MOUSE_DOWN,
                                  cb_switch_screen, SCREEN_APLAYER_TITLE);
}

void
screen_audio_update_infos (struct screen_t *screen, char *infos, int display)
{
  struct audio_t *audio = NULL;
  
  if (!screen || screen->type != SCREEN_TYPE_AUDIO)
    return;

  audio = (struct audio_t *) screen->private;
  if (!audio)
    return;

  if (!audio->infobox)
    return;

  if (display && infos)
    text_block_add_text (audio->infobox, infos);
  else
    evas_object_textblock_clear (audio->infobox);
}

static void
audio_infos_setup (struct screen_t *screen)
{
  struct audio_t *audio = NULL;
  struct font_t *font = NULL;

  font = get_font (omc->cfg->fonts, "infos");
  if (!font)
    return;

  audio = (struct audio_t *) screen->private;
  if (!audio)
    return;
  
  audio->infobox =
    text_block_new (omc, 0, "12%", "69%", "56%", "20%", 0, font,
                    BLK_ALIGN_LEFT, BLK_ALIGN_TOP);
}

void
screen_audio_update_cover (struct screen_t *screen, char *img, int display)
{
  struct audio_t *audio = NULL;
  struct cover_t *cover = NULL;
  
  if (!screen || screen->type != SCREEN_TYPE_AUDIO)
    return;

  audio = (struct audio_t *) screen->private;
  if (!audio)
    return;

  cover = audio->cover;
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
audio_cover_setup (struct screen_t *screen)
{
  struct audio_t *audio = NULL;
  Evas_Object *dummy = NULL;
  
  audio = (struct audio_t *) screen->private;
  if (!audio)
    return;

  audio->cover = cover_new ();
  if (!audio->cover)
    return;

  dummy = evas_object_image_add (omc->evas);
  audio->cover->border = evas_list_append (audio->cover->border, dummy);

  border_new (omc, audio->cover->border,
              BORDER_TYPE_COVER, "70%", "69%", "25%", "25%");

  audio->cover->cover =
    image_new (omc, 0, NULL, NULL, 0, "70%", "69%", "25%", "25%");
}

static void
audio_browser_setup (struct screen_t *screen)
{
  struct audio_t *audio = NULL;
  struct font_t *font = NULL;

  font = get_font (omc->cfg->fonts, "browser");
  if (!font)
    return;
  
  audio = (struct audio_t *) screen->private;
  if (!audio)
    return;

  audio->browser =
    browser_new (omc->screen, font,
                 FILTER_TYPE_AUDIO, "14%", "7%", "80%", "55%");
}

void
screen_audio_update_notifier (struct screen_t *screen,
                              char *cover, char *infos)
{
  struct audio_t *audio = NULL;
  struct notifier_t *notifier = NULL;
  
  if (!screen || screen->type != SCREEN_TYPE_AUDIO)
    return;

  audio = (struct audio_t *) screen->private;
  if (!audio)
    return;

  notifier = audio->notifier;
  if (!notifier)
    return;

  notifier_update (notifier, cover, infos);
}

static void
audio_notifier_setup (struct screen_t *screen)
{
  struct audio_t *audio = NULL;
  struct font_t *font = NULL;

  font = get_font (omc->cfg->fonts, "infos");
  if (!font)
    return;
  
  audio = (struct audio_t *) screen->private;
  if (!audio)
    return;

  audio->notifier = notifier_new (omc, font, "40%", "83%", "100%", "10%");
}

void
screen_audio_setup (struct screen_t *screen)
{
  if (!screen || screen->type != SCREEN_TYPE_AUDIO)
    return;

  screen->private = (struct audio_t *) audio_new ();
  
  widget_background_setup (screen);
  widget_common_toolbar_setup (screen);
  browser_filter_toolbar_setup (screen);
  
  audio_toolbar_setup (screen);
  audio_infos_setup (screen);
  audio_cover_setup (screen);
  audio_browser_setup (screen);
  audio_notifier_setup (screen);
}

void
screen_audio_display (struct screen_t *screen)
{
  struct audio_t *audio = (struct audio_t *) screen->private;

  if (!audio)
    return;
  
  if (audio->infobox)
    evas_object_show (audio->infobox);
}

void
screen_audio_free (struct screen_t *screen)
{
  struct audio_t *audio = NULL;
  
  if (!screen || screen->type != SCREEN_TYPE_AUDIO)
    return;

  audio = (struct audio_t *) screen->private;
  if (audio)
    audio_free (audio);
}
