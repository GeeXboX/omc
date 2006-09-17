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

#ifndef _SCREEN_H_
#define _SCREEN_H_

#include <Evas.h>

typedef enum {
  SCREEN_TYPE_MAIN,
  SCREEN_TYPE_VIDEO,
  SCREEN_TYPE_AUDIO,
  SCREEN_TYPE_IMAGE,
  SCREEN_TYPE_TV,
  SCREEN_TYPE_SETTINGS,
  SCREEN_TYPE_VIEWER,
  SCREEN_TYPE_APLAYER
} screen_type_t;

typedef struct screen_s {
  screen_type_t type;
  Evas_List *objects;
  void *private;
} screen_t;

typedef struct screen_def_s {
  char *id;
  screen_type_t type;
  void (*setup) (screen_t *screen, char *data);
} screen_def_t;

#define SCREEN_CB_KEY_ESCAPE "Escape"

void signal_switch_screen_cb (void *data, Evas_Object *o,
                              const char *emission, const char *source);
void signal_quit_cb (void *data, Evas_Object *o,
                     const char *emission, const char *source);

void screen_object_set_cb (Evas_Object *obj);

void browser_filter_toolbar_setup (screen_t *screen);
void widget_background_setup (screen_t *screen);
void widget_common_toolbar_setup (screen_t *screen);

void screen_init (char *id, void *data);
void screen_uninit (screen_t *screen);

void switch_screen (char *id, void *data);
void cb_switch_screen (void *data, Evas *e,
                       Evas_Object *obj, void *event_info);

void screen_update_notifier (screen_t *screen,
                             char *cover, char *infos);
void screen_update_cwd (screen_t *screen);

#include "screen_aplayer.h"
#include "screen_audio.h"
#include "screen_image.h"
#include "screen_main.h"
#include "screen_settings.h"
#include "screen_tv.h"
#include "screen_video.h"
#include "screen_viewer.h"

#endif /* _SCREEN_H_ */
