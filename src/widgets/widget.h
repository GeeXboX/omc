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

#ifndef _WIDGET_H_
#define _WIDGET_H_

typedef enum widget_type {
  WIDGET_TYPE_UNKNOWN,
  WIDGET_TYPE_TEXT
} widget_type_t;

typedef enum widget_flags {
  WIDGET_FLAG_NONE                  = 0x00,
  WIDGET_FLAG_SHOW                  = 0x01,
  WIDGET_FLAG_FOCUSABLE             = 0x02,
  WIDGET_FLAG_FOCUSED               = 0x04
} widget_flags_t;

typedef enum action_event_type {
  ACTION_EVENT_GO_UP,
  ACTION_EVENT_GO_DOWN,
  ACTION_EVENT_GO_LEFT,
  ACTION_EVENT_GO_RIGHT,
  ACTION_EVENT_CANCEL,
  ACTION_EVENT_OK
} action_event_type_t;

typedef struct widget_focus_s widget_focus_t;

typedef struct widget_s {
  char *id; /* unique identifier */
  widget_type_t type;
  int flags;
  
  /* position and common display properties */
  uint32_t x;
  uint32_t y;
  uint32_t w;
  uint32_t h;
  uint32_t layer;

  /* when focused description */
  widget_focus_t *focus;
  
  /* widget type specific data */
  void *priv;
  
  int (*show) (struct widget_s *widget); /* called to show widget */
  int (*hide) (struct widget_s *widget); /* called to hide widget */
  int (*set_focus) (struct widget_s *widget); /* called to set/unset focus */
  int (*action) (struct widget_s *widget, action_event_type_t ev);
  void (*free) (struct widget_s *widget); /* called to free widget */
} widget_t;

widget_t *widget_new (char *id, widget_type_t type,
                      uint32_t x, uint32_t y, uint32_t w, uint32_t h,
                      uint32_t layer, uint32_t show, uint32_t focusable);

int widget_show (widget_t *widget);
int widget_hide (widget_t *widget);
int widget_set_focus (widget_t *widget, uint32_t state);
int widget_action (widget_t *widget, action_event_type_t ev);
void widget_free (widget_t *widget);

widget_t *get_widget_by_id (Evas_List *list, char *id);

typedef struct neighbours_s {
  widget_t *up;
  widget_t *down;
  widget_t *left;
  widget_t *right;
} neighbours_t;

typedef enum neighbours_type {
  NEIGHBOURS_UP,
  NEIGHBOURS_DOWN,
  NEIGHBOURS_LEFT,
  NEIGHBOURS_RIGHT
} neighbours_type_t;

neighbours_t *neighbours_new (void);
void neighbours_set (neighbours_t *nb, widget_t *widget,
                     neighbours_type_t type);
void neighbours_free (neighbours_t *nb);

struct widget_focus_s {
  neighbours_t *neighbours;
};

widget_focus_t *widget_focus_new (void);
void widget_focus_free (widget_focus_t *focus);

#include "image.h"
#include "color.h"
#include "text.h"
#include "textblock.h"
#include "notifier.h"
#include "border.h"
#include "menu.h"
#include "cover.h"
#include "clipper.h"
#include "browser.h"
#include "item.h"
#include "cwd.h"

int omc_compute_coord (char *coord, int max);

void object_add_default_cb (Evas_Object *obj);

char *getExtension (char *filename);

#endif /* _WIDGET_H_ */
