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

#ifndef _WIDGET_BROWSER_H_
#define _WIDGET_BROWSER_H_

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

browser_t *browser_new (screen_t *screen, font_t *font, int filter_type,
                        char *x, char *y, char *w, char *h);

void browser_free (browser_t *browser);

#endif /* _WIDGET_BROWSER_H_ */
