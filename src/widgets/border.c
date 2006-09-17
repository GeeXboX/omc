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

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "omc.h"

#define IMG_PLAIN_CENTER OMC_DATA_DIR"/border-plain-c.png"
#define IMG_PLAIN_TOP OMC_DATA_DIR"/border-plain-t.png"
#define IMG_PLAIN_BOTTOM OMC_DATA_DIR"/border-plain-b.png"
#define IMG_PLAIN_LEFT OMC_DATA_DIR"/border-plain-l.png"
#define IMG_PLAIN_RIGHT OMC_DATA_DIR"/border-plain-r.png"
#define IMG_PLAIN_TOP_LEFT OMC_DATA_DIR"/border-plain-tl.png"
#define IMG_PLAIN_TOP_RIGHT OMC_DATA_DIR"/border-plain-tr.png"
#define IMG_PLAIN_BOTTOM_LEFT OMC_DATA_DIR"/border-plain-bl.png"
#define IMG_PLAIN_BOTTOM_RIGHT OMC_DATA_DIR"/border-plain-br.png"

#define IMG_MENU_CENTER OMC_DATA_DIR"/menu-c.png"
#define IMG_MENU_TOP OMC_DATA_DIR"/menu-t.png"
#define IMG_MENU_BOTTOM OMC_DATA_DIR"/menu-b.png"
#define IMG_MENU_LEFT OMC_DATA_DIR"/menu-l.png"
#define IMG_MENU_RIGHT OMC_DATA_DIR"/menu-r.png"
#define IMG_MENU_TOP_LEFT OMC_DATA_DIR"/menu-tl.png"
#define IMG_MENU_TOP_RIGHT OMC_DATA_DIR"/menu-tr.png"
#define IMG_MENU_BOTTOM_LEFT OMC_DATA_DIR"/menu-bl.png"
#define IMG_MENU_BOTTOM_RIGHT OMC_DATA_DIR"/menu-br.png"

#define IMG_COVER_CENTER OMC_DATA_DIR"/menu-c.png"
#define IMG_COVER_TOP OMC_DATA_DIR"/cover-t.png"
#define IMG_COVER_BOTTOM OMC_DATA_DIR"/cover-b.png"
#define IMG_COVER_LEFT OMC_DATA_DIR"/cover-l.png"
#define IMG_COVER_RIGHT OMC_DATA_DIR"/cover-r.png"
#define IMG_COVER_TOP_LEFT OMC_DATA_DIR"/cover-tl.png"
#define IMG_COVER_TOP_RIGHT OMC_DATA_DIR"/cover-tr.png"
#define IMG_COVER_BOTTOM_LEFT OMC_DATA_DIR"/cover-bl.png"
#define IMG_COVER_BOTTOM_RIGHT OMC_DATA_DIR"/cover-br.png"

Evas_Object *
border_new (omc_t *omc, Evas_List *list,
            int type, char *cx, char *cy, char *cw, char *ch)
{
  Evas_Object *obj = NULL, *obj2 = NULL;
  char x2[8], y2[8], w2[8], h2[8];
  int x, y, w, h;
  int border = 0;

  if (!cx || !cy || !cw || !ch)
    return NULL;
  
  x = omc_compute_coord (cx, omc->w);
  y = omc_compute_coord (cy, omc->h);
  w = omc_compute_coord (cw, omc->w);
  h = omc_compute_coord (ch, omc->h);

  if (type == BORDER_TYPE_PLAIN || type == BORDER_TYPE_MENU)
    border = 20;
  else if (type == BORDER_TYPE_COVER)
    border = 12;
      
  sprintf (x2, "%d", x);
  sprintf (y2, "%d", y);
  sprintf (w2, "%d", w);
  sprintf (h2, "%d", h);
  if (type == BORDER_TYPE_PLAIN)
    obj = image_new (omc, 0, IMG_PLAIN_CENTER, NULL, 0, x2, y2, w2, h2);
  else if (type == BORDER_TYPE_MENU)
    obj = image_new (omc, 0, IMG_MENU_CENTER, NULL, 0, x2, y2, w2, h2);
  else if (type == BORDER_TYPE_COVER)
    obj = image_new (omc, 0, IMG_COVER_CENTER, NULL, 0, x2, y2, w2, h2);
  obj2 = obj;
  list = evas_list_append (list, obj);

  sprintf (x2, "%d", x);
  sprintf (y2, "%d", y - border);
  sprintf (w2, "%d", w);
  sprintf (h2, "%d", border);
  if (type == BORDER_TYPE_PLAIN)
    obj = image_new (omc, 0, IMG_PLAIN_TOP, NULL, 0, x2, y2, w2, h2);
  else if (type == BORDER_TYPE_MENU)
    obj = image_new (omc, 0, IMG_MENU_TOP, NULL, 0, x2, y2, w2, h2);
  else if (type == BORDER_TYPE_COVER)
    obj = image_new (omc, 0, IMG_COVER_TOP, NULL, 0, x2, y2, w2, h2);
  list = evas_list_append (list, obj);

  sprintf (x2, "%d", x);
  sprintf (y2, "%d", y + h);
  sprintf (w2, "%d", w);
  sprintf (h2, "%d", border);
  if (type == BORDER_TYPE_PLAIN)
    obj = image_new (omc, 0, IMG_PLAIN_BOTTOM, NULL, 0, x2, y2, w2, h2);
  else if (type == BORDER_TYPE_MENU)
    obj = image_new (omc, 0, IMG_MENU_BOTTOM, NULL, 0, x2, y2, w2, h2);
  else
    obj = image_new (omc, 0, IMG_COVER_BOTTOM, NULL, 0, x2, y2, w2, h2);
  list = evas_list_append (list, obj);

  sprintf (x2, "%d", x - border);
  sprintf (y2, "%d", y);
  sprintf (w2, "%d", border);
  sprintf (h2, "%d", h);
  if (type == BORDER_TYPE_PLAIN)
    obj = image_new (omc, 0, IMG_PLAIN_LEFT, NULL, 0, x2, y2, w2, h2);
  else if (type == BORDER_TYPE_MENU)
    obj = image_new (omc, 0, IMG_MENU_LEFT, NULL, 0, x2, y2, w2, h2);
  else
    obj = image_new (omc, 0, IMG_COVER_LEFT, NULL, 0, x2, y2, w2, h2);
  list = evas_list_append (list, obj);

  sprintf (x2, "%d", x + w);
  sprintf (y2, "%d", y);
  sprintf (w2, "%d", border);
  sprintf (h2, "%d", h);
  if (type == BORDER_TYPE_PLAIN)
    obj = image_new (omc, 0, IMG_PLAIN_RIGHT, NULL, 0, x2, y2, w2, h2);
  else if (type == BORDER_TYPE_MENU)
    obj = image_new (omc, 0, IMG_MENU_RIGHT, NULL, 0, x2, y2, w2, h2);
  else
    obj = image_new (omc, 0, IMG_COVER_RIGHT, NULL, 0, x2, y2, w2, h2);
  list = evas_list_append (list, obj);

  sprintf (x2, "%d", x - border);
  sprintf (y2, "%d", y - border);
  sprintf (w2, "%d", border);
  sprintf (h2, "%d", border);
  if (type == BORDER_TYPE_PLAIN)
    obj = image_new (omc, 0, IMG_PLAIN_TOP_LEFT, NULL, 0, x2, y2, w2, h2);
  else if (type == BORDER_TYPE_MENU)
    obj = image_new (omc, 0, IMG_MENU_TOP_LEFT, NULL, 0, x2, y2, w2, h2);
  else
    obj = image_new (omc, 0, IMG_COVER_TOP_LEFT, NULL, 0, x2, y2, w2, h2);
  list = evas_list_append (list, obj);

  sprintf (x2, "%d", x + w);
  sprintf (y2, "%d", y - border);
  sprintf (w2, "%d", border);
  sprintf (h2, "%d", border);
  if (type == BORDER_TYPE_PLAIN)
    obj = image_new (omc, 0, IMG_PLAIN_TOP_RIGHT, NULL, 0, x2, y2, w2, h2);
  else if (type == BORDER_TYPE_MENU)
    obj = image_new (omc, 0, IMG_MENU_TOP_RIGHT, NULL, 0, x2, y2, w2, h2);
  else
    obj = image_new (omc, 0, IMG_COVER_TOP_RIGHT, NULL, 0, x2, y2, w2, h2);
  list = evas_list_append (list, obj);

   sprintf (x2, "%d", x - border);
   sprintf (y2, "%d", y + h);
   sprintf (w2, "%d", border);
   sprintf (h2, "%d", border);
  if (type == BORDER_TYPE_PLAIN)
    obj = image_new (omc, 0, IMG_PLAIN_BOTTOM_LEFT, NULL, 0, x2, y2, w2, h2);
  else if (type == BORDER_TYPE_MENU)
    obj = image_new (omc, 0, IMG_MENU_BOTTOM_LEFT, NULL, 0, x2, y2, w2, h2);
  else
    obj = image_new (omc, 0, IMG_COVER_BOTTOM_LEFT, NULL, 0, x2, y2, w2, h2);
  list = evas_list_append (list, obj);

  sprintf (x2, "%d", x + w);
  sprintf (y2, "%d", y + h);
  sprintf (w2, "%d", border);
  sprintf (h2, "%d", border);
  if (type == BORDER_TYPE_PLAIN)
    obj = image_new (omc, 0, IMG_PLAIN_BOTTOM_RIGHT, NULL, 0, x2, y2, w2, h2);
  else if (type == BORDER_TYPE_MENU)
    obj = image_new (omc, 0, IMG_MENU_BOTTOM_RIGHT, NULL, 0, x2, y2, w2, h2);
  else
    obj = image_new (omc, 0, IMG_COVER_BOTTOM_RIGHT, NULL, 0, x2, y2, w2, h2);
  list = evas_list_append (list, obj);

  return obj2;
}
