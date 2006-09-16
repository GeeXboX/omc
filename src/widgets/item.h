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

#ifndef _WIDGET_ITEM_H_
#define _WIDGET_ITEM_H_

#define ITEM_TYPE_DIRECTORY 0
#define ITEM_TYPE_FILE 1

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

item_t *item_new (browser_t *browser, Evas_Object *icon, Evas_Object *text,
                  int type, char *mrl, int mrl_type);

void item_free (item_t *item);

#endif /* _WIDGET_ITEM_H_ */
