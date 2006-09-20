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
#include "omc.h"

static void
cb_mouse_menu_item_logo_hide (void *data, Evas *e,
                              Evas_Object *obj, void *event_info)
{
  Evas_Object *logo = (Evas_Object *) data;
  if (logo)
    evas_object_hide (logo);
}

static void
cb_mouse_menu_item_logo_show (void *data, Evas *e,
                              Evas_Object *obj, void *event_info)
{
  Evas_Object *logo = (Evas_Object *) data;
  if (logo)
    evas_object_show (logo);
}

static void
cb_mouse_menu_show_selector (void *data, Evas *e,
                             Evas_Object *obj, void *event_info)
{
  Evas_Object *select = (Evas_Object *) data;
  Evas_Coord x = 0, y = 0, h = 0, oy = 0, oh = 0;

  if (!select || !obj)
    return;

  evas_object_geometry_get (select, &x, NULL, NULL, &h);
  evas_object_geometry_get (obj, NULL, &oy, NULL, &oh);  
  y = oy - ((h - oh) / 2);

  evas_object_move (select, x, y);
  evas_object_show (select);
}

static void
cb_mouse_menu_hide_selector (void *data, Evas *e,
                             Evas_Object *obj, void *event_info)
{
  Evas_Object *select = (Evas_Object *) data;
  if (select)
    evas_object_hide (select);
}

/* Menu Object */
menu_t *
menu_new (omc_t *omc, font_t *font, int align,
          char *select, char *select_w, char *select_h,
          char *x, char *y, char *w, char *h)
{
  menu_t *menu = NULL;
  
  menu = (menu_t *) malloc (sizeof (menu_t));
  menu->align = align;
  menu->x = omc_compute_coord (x, omc->w);
  menu->y = omc_compute_coord (y, omc->h);
  menu->w = omc_compute_coord (w, omc->w);
  menu->h = omc_compute_coord (h, omc->h);
  menu->font = font;
  menu->items = NULL;
  menu->hidden = NULL;
  menu->select = NULL;
  
  if (select)
    menu->select = image_new (omc, 0, select, NULL, 5,
                              x, "0", select_w, select_h);
  
  border_new (omc, omc->screen->objects, BORDER_TYPE_MENU, x, y, w, h);

  return menu;
}

Evas_Object *
menu_add_menu_item (omc_t *omc, menu_t *menu,
                   char *str, char *logo,
                   char *logo_x, char *logo_y,
                   char *logo_w, char *logo_h)
{
  Evas_Object *item = NULL;

  item = text_new (omc, 1, menu->font, str, 255, 5, "0", "0");
  object_add_default_cb (item);

  if (logo)
  {
    Evas_Object *item_logo = NULL;
    item_logo = image_new (omc, 0, logo, NULL, 0,
                           logo_x, logo_y, logo_w, logo_h);
    menu->hidden = evas_list_append (menu->hidden, item_logo);
    evas_object_event_callback_add (item, EVAS_CALLBACK_MOUSE_IN,
                                    cb_mouse_menu_item_logo_show, item_logo);
    evas_object_event_callback_add (item, EVAS_CALLBACK_MOUSE_OUT,
                                    cb_mouse_menu_item_logo_hide, item_logo);
  }

  if (menu->select)
  {
    evas_object_event_callback_add (item, EVAS_CALLBACK_MOUSE_IN,
                                    cb_mouse_menu_show_selector,
                                    menu->select);
    evas_object_event_callback_add (item, EVAS_CALLBACK_MOUSE_OUT,
                                    cb_mouse_menu_hide_selector,
                                    menu->select);
  }

  menu->items = evas_list_append (menu->items, item);

  return item;
}

void
menu_compute_items (menu_t *menu)
{
  Evas_List *list;
  int items, i, x, y, h;
  items = evas_list_count (menu->items);
  i = 0;

  x = 0;
  y = menu->y;
  h = (int) menu->h / items;

  for (list = menu->items; list; list = list->next)
  {
    Evas_Object *obj = NULL;

    obj = (Evas_Object *) list->data;
    if (!obj)
      continue;

    switch (menu->align) {
    case MENU_ALIGN_LEFT:
      x = menu->x;
      break;
    case MENU_ALIGN_CENTER:
      x = (int) (menu->x +
                 (menu->w - evas_object_text_horiz_advance_get (obj)) / 2);
      break;
    case MENU_ALIGN_RIGHT:
      x = (int) (menu->x + menu->w - evas_object_text_horiz_advance_get (obj));
      break;
    }

    y = (int) (menu->y + i++ * h +
               (h - evas_object_text_max_ascent_get (obj)) / 2);
    evas_object_move (obj, x, y);
  }
}

void
menu_free (menu_t *menu)
{
  if (!menu)
    return;

  if (menu->select)
    evas_object_del (menu->select);
  if (menu->items)
  {
    Evas_List *list;
    for (list = menu->items; list; list = list->next)
    {
      Evas_Object *obj = NULL;
    
      obj = (Evas_Object *) list->data;
      if (!obj)
        continue;
      
      evas_object_del (obj);
      menu->items = evas_list_remove_list (menu->items, menu->items);
    }
    free (menu->items);

    for (list = menu->hidden; list; list = list->next)
    {
      Evas_Object *obj = NULL;
    
      obj = (Evas_Object *) list->data;
      if (!obj)
        continue;
      
      evas_object_del (obj);
      menu->hidden = evas_list_remove_list (menu->hidden, menu->hidden);
    }
    free (menu->hidden);
  }
  free (menu);
}
