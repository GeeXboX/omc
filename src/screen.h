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

enum {
  SCREEN_TYPE_MAIN,
  SCREEN_TYPE_VIDEO,
  SCREEN_TYPE_AUDIO,
  SCREEN_TYPE_IMAGE,
  SCREEN_TYPE_TV,
  SCREEN_TYPE_SETTINGS,
  SCREEN_TYPE_APLAYER
};

struct screen_t {
  int type;
  Evas_List *objects;
  void *private;
};

#define SCREEN_CB_KEY_ESCAPE "Escape"

void signal_switch_screen_cb (void *data, Evas_Object *o,
                              const char *emission, const char *source);
void signal_quit_cb (void *data, Evas_Object *o,
                     const char *emission, const char *source);

void screen_object_set_cb (Evas_Object *obj);

void browser_filter_toolbar_setup (struct screen_t *screen);
void widget_background_setup (struct screen_t *screen);
void widget_common_toolbar_setup (struct screen_t *screen);

void screen_init (char *id);
void screen_uninit (struct screen_t *screen);

void switch_screen (char *id);
void cb_switch_screen (void *data, Evas *e,
                       Evas_Object *obj, void *event_info);

void screen_update_notifier (struct screen_t *screen,
                             char *cover, char *infos);
void screen_update_cwd (struct screen_t *screen);

#endif /* _SCREEN_H_ */
