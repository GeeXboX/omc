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

#ifndef _WIDGET_MENU_H_
#define _WIDGET_MENU_H_

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

menu_t *menu_new (omc_t *omc, font_t *font, int align,
                  char *select, char *select_w, char *select_h,
                  char *x, char *y, char *w, char *h);
void menu_free (menu_t *menu);

Evas_Object *menu_add_menuitem (omc_t *omc, menu_t *menu,
                                char *str, char *logo,
                                char *logo_x, char *logo_y,
                                char *logo_w, char *logo_h);
void menu_compute_items (menu_t *menu);

#endif /* _WIDGET_MENU_H_ */
