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
#include "cfgparser.h"

struct menu_t {
  int x, y;
  int w, h;
  int align;
  struct font_t *font;
  Evas_Object *select;
  Evas_List *items;
  Evas_List *hidden;
};

enum {
  MENU_ALIGN_LEFT,
  MENU_ALIGN_CENTER,
  MENU_ALIGN_RIGHT
};

enum {
  BORDER_TYPE_PLAIN,
  BORDER_TYPE_MENU,
  BORDER_TYPE_COVER
};

int omc_compute_coord (char *coord, int max);

Evas_Object *
border_new (struct omc_t *omc, Evas_List *list,
            int type, char *cx, char *cy, char *cw, char *ch);

struct menu_t *menu_new (struct omc_t *omc, struct font_t *font, int align,
                         char *select, char *select_w, char *select_h,
                         char *x, char *y, char *w, char *h);

Evas_Object *
menu_add_menuitem (struct omc_t *omc, struct menu_t *menu,
                   char *str, char *logo,
                   char *logo_x, char *logo_y,
                   char *logo_w, char *logo_h);

void menu_compute_items (struct menu_t *menu);
void menu_free (struct menu_t *menu);

Evas_Object *
image_new (struct omc_t *omc, int focusable, char *name, char *fname,
           int layer, char *x, char *y, char *w, char *h);

Evas_Object *
text_new (struct omc_t *omc, int focusable, struct font_t *font, char *str,
          int alpha, int layer, char *x, char *y);

void object_add_default_cb (Evas_Object *obj);

#define ITEM_TYPE_DIRECTORY 0
#define ITEM_TYPE_FILE 1

struct browser_t {
  int x, y;
  int w, h;
  int pos;
  int capacity_w;
  int capacity_h;
  int filter_type;
  Evas_List *entries;
  Evas_Object *clip;
  struct font_t *font;
};

struct item_t {
  struct browser_t *browser;
  Evas_Object *icon;
  Evas_Object *text;
  Evas_Object *clip;
  int type;
  char *mrl;
  int mrl_type;
  char *infos;
  char *artist;
  char *album;
  char *cover;
  int updated;
};

struct browser_t *
browser_new (struct screen_t *screen, struct font_t *font, int filter_type,
             char *x, char *y, char *w, char *h);
void browser_free (struct browser_t *browser);

enum {
  BLK_NEW_LINE,
  BLK_NEW_TAB
};

#define BLK_ALIGN_LEFT "left"
#define BLK_ALIGN_CENTER "center"
#define BLK_ALIGN_RIGHT "right"
#define BLK_ALIGN_TOP "top"
#define BLK_ALIGN_BOTTOM "bottom"

void text_block_format (Evas_Object *blk, int format);
void text_block_add_text (Evas_Object *blk, char *text);
Evas_Object *
text_block_new (struct omc_t *omc, int editable, char *x, char *y,
                char *w, char *h, int layer, struct font_t *font,
                char *align_h, char *align_v);

Evas_Object *
object_clipper (struct omc_t *omc, char *x, char *y, char *w, char *h);

struct cover_t {
  Evas_List *border;
  Evas_Object *cover;
};

struct cover_t *cover_new (void);
void cover_free (struct cover_t *cover);

struct cwd_t {
  Evas_List *border;
  Evas_Object *path;
};

struct cwd_t *cwd_new (void);
void cwd_free (struct cwd_t *cwd);

struct notifier_t {
  Ecore_Timer *timer;
  Evas_List *border;
  Evas_Object *cover;
  Evas_Object *infos;
  int show;
  int padding;
};

struct notifier_t *
notifier_new (struct omc_t *omc, struct font_t *font,
              char *x, char *y, char *w, char *h);
void notifier_free (struct notifier_t *notifier);
void notifier_update (struct notifier_t *notifier, char *cover, char *infos);
void notifier_show (struct notifier_t *notifier);
void notifier_hide (struct notifier_t *notifier);

#endif /* _WIDGET_H_ */
