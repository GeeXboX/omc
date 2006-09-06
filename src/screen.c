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

#include "screen.h"
#include "omc.h"
#include "widget.h"
#include "screen_main.h"
#include "screen_video.h"
#include "screen_audio.h"
#include "screen_tv.h"
#include "screen_image.h"
#include "screen_settings.h"
#include "screen_aplayer.h"
#include "screen_viewer.h"

void
signal_quit_cb (void *data, Evas_Object *o,
                const char *emission, const char *source)
{
  ecore_main_loop_quit ();
}

#define IMG_ICON_MOVIE OMC_DATA_DIR"/film.png"
#define IMG_ICON_MOVIE_FOCUS OMC_DATA_DIR"/film_focus.png"
#define IMG_ICON_AUDIO OMC_DATA_DIR"/music.png"
#define IMG_ICON_AUDIO_FOCUS OMC_DATA_DIR"/music_focus.png"
#define IMG_ICON_IMAGE OMC_DATA_DIR"/picture.png"
#define IMG_ICON_IMAGE_FOCUS OMC_DATA_DIR"/picture_focus.png"

void
browser_filter_toolbar_setup (screen_t *screen)
{
  Evas_Object *obj = NULL;
  
  obj = image_new (omc, 1, IMG_ICON_MOVIE, IMG_ICON_MOVIE_FOCUS,
                   0, "0%", "2%", "65", "62");
  screen->objects = evas_list_append (screen->objects, obj);
  object_add_default_cb (obj);
  evas_object_event_callback_add (obj, EVAS_CALLBACK_MOUSE_DOWN,
                                  cb_switch_screen, SCREEN_VIDEO_TITLE);

  obj = image_new (omc, 1, IMG_ICON_AUDIO, IMG_ICON_AUDIO_FOCUS,
                   0, "1%", "13%", "50", "62");
  screen->objects = evas_list_append (screen->objects, obj);
  object_add_default_cb (obj);
  evas_object_event_callback_add (obj, EVAS_CALLBACK_MOUSE_DOWN,
                                  cb_switch_screen, SCREEN_AUDIO_TITLE);
  
  obj = image_new (omc, 1, IMG_ICON_IMAGE, IMG_ICON_IMAGE_FOCUS,
                   0, "1%", "24%", "50", "62");
  screen->objects = evas_list_append (screen->objects, obj);
  object_add_default_cb (obj);
  evas_object_event_callback_add (obj, EVAS_CALLBACK_MOUSE_DOWN,
                                  cb_switch_screen, SCREEN_IMAGE_TITLE);
}

#define SCREEN_IMG_LEFT_BORDER OMC_DATA_DIR"/border.png"
#define IMG_ICON_BACK OMC_DATA_DIR"/back.png"
#define IMG_ICON_BACK_FOCUS OMC_DATA_DIR"/back_focus.png"

void
widget_common_toolbar_setup (screen_t *screen)
{
  Evas_Object *obj = NULL;
  
  obj = image_new (omc, 0, SCREEN_IMG_LEFT_BORDER,
                   NULL, 0, "0", "0", "50", "100%");
  screen->objects = evas_list_append (screen->objects, obj);

  obj = image_new (omc, 1, IMG_ICON_BACK, IMG_ICON_BACK_FOCUS,
                   0, "1%", "91%", "48", "49");
  screen->objects = evas_list_append (screen->objects, obj);
  object_add_default_cb (obj);
  evas_object_event_callback_add (obj, EVAS_CALLBACK_MOUSE_DOWN,
                                  cb_switch_screen, SCREEN_MAIN_TITLE);
}

#define SCREEN_IMG_BACKGROUND OMC_DATA_DIR"/background.png"

void
widget_background_setup (screen_t *screen)
{
  Evas_Object *obj = NULL;
  
  obj = image_new (omc, 0, SCREEN_IMG_BACKGROUND,
                   NULL, 0, "0", "0", "100%", "100%");
  screen->objects = evas_list_append (screen->objects, obj);
}

void
screen_uninit (screen_t *screen)
{
  Evas_List *list;
  
  if (!screen)
    return;

  for (list = screen->objects; list; list = list->next)
  {
    Evas_Object *obj;
    
    obj = (Evas_Object *) list->data;
    evas_object_del (obj);
    screen->objects = evas_list_remove_list (screen->objects, screen->objects);
  }
  free (screen->objects);

  switch (screen->type)
  {
  case SCREEN_TYPE_MAIN:
    screen_main_free (screen);
    break;
  case SCREEN_TYPE_AUDIO:
    screen_audio_free (screen);
    break;
  case SCREEN_TYPE_VIDEO:
    screen_video_free (screen);
    break;
  case SCREEN_TYPE_IMAGE:
    screen_image_free (screen);
    break;
  case SCREEN_TYPE_APLAYER:
    screen_aplayer_free (screen);
    break;
  case SCREEN_TYPE_VIEWER:
    screen_viewer_free (screen);
    break;
  default:
    break;
  }
  
  free (screen);
}

static void
screen_display (screen_t *screen)
{
  Evas_List *list;
  
  if (!screen)
    return;

  for (list = screen->objects; list; list = list->next)
  {
    Evas_Object *obj;
    
    obj = (Evas_Object *) list->data;
    evas_object_show (obj);
  }

  switch (screen->type)
  {
  case SCREEN_TYPE_MAIN:
    screen_main_display (screen);
    break;
  case SCREEN_TYPE_AUDIO:
    screen_audio_display (screen);
    break;
  case SCREEN_TYPE_VIDEO:
    screen_video_display (screen);
    break;
  case SCREEN_TYPE_IMAGE:
    screen_image_display (screen);
    break;
  case SCREEN_TYPE_APLAYER:
    screen_aplayer_display (screen);
    break;
  case SCREEN_TYPE_VIEWER:
    screen_viewer_display (screen);
    break;
  default:
    break;
  }
}

screen_def_t screen_def [] = {
  {SCREEN_MAIN_TITLE, SCREEN_TYPE_MAIN, screen_main_setup},
  {SCREEN_VIDEO_TITLE, SCREEN_TYPE_VIDEO, screen_video_setup},
  {SCREEN_AUDIO_TITLE, SCREEN_TYPE_AUDIO, screen_audio_setup},
  {SCREEN_TV_TITLE, SCREEN_TYPE_TV, screen_tv_setup},
  {SCREEN_IMAGE_TITLE, SCREEN_TYPE_IMAGE, screen_image_setup},
  {SCREEN_SETTINGS_TITLE, SCREEN_TYPE_SETTINGS, screen_settings_setup},
  {SCREEN_APLAYER_TITLE, SCREEN_TYPE_APLAYER, screen_aplayer_setup},
  {SCREEN_VIEWER_TITLE, SCREEN_TYPE_VIEWER, screen_viewer_setup},
  {NULL, 0, NULL},
};

void
screen_init (char *id, void *data)
{
  int i;

  if (omc->screen)
    screen_uninit (omc->screen);

  omc->screen = (screen_t *) malloc (sizeof (screen_t));
  omc->screen->objects = NULL;
  omc->screen->private = NULL;

  for (i=0 ; screen_def[i].id ; i++)
  {
    if (!strncmp (id, screen_def[i].id, strlen (screen_def[i].id)))
    {
      omc->screen->type = screen_def[i].type;
      screen_def[i].setup (omc->screen, data);
      break;
    }
  }
  if (!screen_def[i].id)
  {
    omc->screen->type = screen_def[0].type;
    screen_def[0].setup (omc->screen, data);
  }

  screen_display (omc->screen);
}

void
switch_screen (char *id, void *data)
{
  int i;
  int ret = 1;

  for (i=0 ; ret && screen_def[i].id ; i++)
  {
    ret = ret && strncmp (id, screen_def[i].id, strlen (screen_def[i].id));
  }
  if (ret)
    return;

  screen_init (id, data);
}

void
cb_switch_screen (void *data, Evas *e, Evas_Object *obj, void *event_info)
{
  char *id = (char *) data;
  switch_screen (id, NULL);
}

void
screen_update_notifier (screen_t *screen, char *cover, char *infos)
{
  if (!screen)
    return;

  if (screen->type == SCREEN_TYPE_MAIN)
    screen_main_update_notifier (screen, cover, infos);
  else if (screen->type == SCREEN_TYPE_VIDEO)
    screen_video_update_notifier (screen, cover, infos);
  if (screen->type == SCREEN_TYPE_AUDIO)
    screen_audio_update_notifier (screen, cover, infos);
}

void
screen_update_cwd (screen_t *screen)
{
  if (!screen)
    return;

  if (screen->type == SCREEN_TYPE_IMAGE)
    screen_image_update_cwd (screen);
}
