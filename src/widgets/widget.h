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

#include <Ecore.h>

#include "image.h"
#include "color.h"
#include "text.h"
#include "textblock.h"
#include "notifier.h"
#include "border.h"

typedef struct menu_s {
  int x, y;
  int w, h;
  int align;
  font_t *font;
  Evas_Object *select;
  Evas_List *items;
  Evas_List *hidden;
} menu_t;

enum {
  MENU_ALIGN_LEFT,
  MENU_ALIGN_CENTER,
  MENU_ALIGN_RIGHT
};

int omc_compute_coord (char *coord, int max);

menu_t *menu_new (omc_t *omc, font_t *font, int align,
                         char *select, char *select_w, char *select_h,
                         char *x, char *y, char *w, char *h);

Evas_Object *
menu_add_menuitem (omc_t *omc, menu_t *menu,
                   char *str, char *logo,
                   char *logo_x, char *logo_y,
                   char *logo_w, char *logo_h);

void menu_compute_items (menu_t *menu);
void menu_free (menu_t *menu);

void object_add_default_cb (Evas_Object *obj);

#define ITEM_TYPE_DIRECTORY 0
#define ITEM_TYPE_FILE 1

typedef struct browser_s {
  int x, y;
  int w, h;
  int pos;
  int capacity_w;
  int capacity_h;
  int filter_type;
  Evas_List *entries;
  Evas_Object *clip;
  font_t *font;
} browser_t;

typedef struct item_s {
  browser_t *browser;
  Evas_Object *icon;
  Evas_Object *text;
  Evas_Object *clip;
  mrl_t *mrl;
  int type;
  //char *mrl;
  //int mrl_type;
  char *infos;
  char *artist;
  char *album;
  char *cover;
  int updated;
} item_t;

browser_t *
browser_new (screen_t *screen, font_t *font, int filter_type,
             char *x, char *y, char *w, char *h);
void browser_free (browser_t *browser);

Evas_Object *
object_clipper (omc_t *omc, char *x, char *y, char *w, char *h);

typedef struct cover_s {
  Evas_List *border;
  Evas_Object *cover;
} cover_t;

cover_t *cover_new (void);
void cover_free (cover_t *cover);

typedef struct cwd_s {
  Evas_List *border;
  Evas_Object *path;
} cwd_t;

cwd_t *cwd_new (void);
void cwd_free (cwd_t *cwd);

char *getExtension (char *filename);

#endif /* _WIDGET_H_ */
