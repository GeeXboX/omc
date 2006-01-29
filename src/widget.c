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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <Evas.h>
#include <Ecore.h>

#include <dirent.h>
#include <libgen.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "omc.h"
#include "widget.h"
#include "filter.h"
#include "screen.h"
#include "player.h"
#include "infos.h"
#include "screen_audio.h"
#include "screen_video.h"

/* Colors */
struct color_t {
  int r;
  int g;
  int b;
  int a;
};

static struct color_t *
color_new (char *color, int alpha)
{
  struct color_t *cl = NULL;

  cl = (struct color_t *) malloc (sizeof (struct color_t));
  cl->r = cl->g = cl->b = 0;
  cl->a = alpha;

  if (cl->a < 0)
    cl->a = 0;
  if (cl->a > 255)
    cl->a = 255;
  
  if (color && color[0] == '#' && strlen (color) == 7)
    {
      char *end;
      int val = strtol (color + 1, &end, 16);
      if (end > color + 1 && *end == '\0')
      {
        cl->r = (val & 0xFF0000) >> 1;
        cl->g = (val & 0x00FF00) >> 8;
        cl->b = (val & 0x0000FF);
      }
    }

  return cl;
}

static void
color_free (struct color_t *color)
{
  if (!color)
    return;
  free (color);
}

/* Coordinates */
int
omc_compute_coord (char *coord, int max)
{
  int val = 0;
  char *end;
  
  if (!coord)
    return 0;

  val = strtol (coord, &end, 10);
  if (*end == '%')
    val = val * max / 100;

  return val;
}

/* Callbacks */
static void
cb_img_focus_update (void *data, Evas *e, Evas_Object *obj, void *event_info)
{
  char *file = (char *) data;
  if (obj)
    evas_object_image_file_set (obj, file, NULL);
}

static void
cb_text_focus_update (void *data, Evas *e, Evas_Object *obj, void *event_info)
{
  struct color_t *color = (struct color_t *) data;
  if (obj)
    evas_object_color_set (obj, color->r, color->g, color->b, color->a);
}

static void
cb_mouse_in (void *data, Evas *e, Evas_Object *obj, void *event_info)
{
  if (obj)
    evas_object_focus_set (obj, 1);
}

static void
cb_mouse_out (void *data, Evas *e, Evas_Object *obj, void *event_info)
{
  if (obj)
    evas_object_focus_set (obj, 0);
}

void
object_add_default_cb (Evas_Object *obj)
{
  if (!obj)
    return;
  
  evas_object_event_callback_add (obj, EVAS_CALLBACK_MOUSE_IN,
                                  cb_mouse_in, NULL);
  evas_object_event_callback_add (obj, EVAS_CALLBACK_MOUSE_OUT,
                                  cb_mouse_out, NULL);
}

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

/* Image Object */
static Evas_Object *
evas_image_new (struct omc_t *omc, int focusable, char *name, char *fname,
                int layer, int x, int y, int w, int h)
{
  Evas_Object *img = NULL;
  
  img = evas_object_image_add (omc->evas);
  evas_object_image_file_set (img, name, NULL);
  evas_object_move (img, x, y); 
  evas_object_resize (img, w, h);
  evas_object_image_fill_set (img, 0, 0, w, h);
  evas_object_layer_set (img, layer);
  evas_object_image_smooth_scale_set (img, 1);

  if (focusable && fname)
  {
    evas_object_event_callback_add (img, EVAS_CALLBACK_FOCUS_IN,
                                    cb_img_focus_update, fname);
    evas_object_event_callback_add (img, EVAS_CALLBACK_FOCUS_OUT,
                                    cb_img_focus_update, name);
  }

  return img;
}

Evas_Object *
image_new (struct omc_t *omc, int focusable, char *name, char *fname,
           int layer, char *x, char *y, char *w, char *h)
{
  int x2, y2, w2, h2;

  x2 = omc_compute_coord (x, omc->w);
  y2 = omc_compute_coord (y, omc->h);
  w2 = omc_compute_coord (w, omc->w);
  h2 = omc_compute_coord (h, omc->h);
    
  return evas_image_new (omc, focusable, name, fname, layer, x2, y2, w2, h2);
}

/* Text Object */
static Evas_Object *
evas_text_new (struct omc_t *omc, int focusable, char *font, char *str,
               int x, int y, int layer, int size, int style,
               struct color_t *color, struct color_t *fcolor)
{
  Evas_Object *txt = NULL;
  
  txt = evas_object_text_add (omc->evas);
  evas_object_text_font_set (txt, font, size);
  evas_object_text_text_set (txt, str);
  evas_object_move (txt, x, y);
  evas_object_color_set (txt, color->r, color->g, color->b, color->a);
  evas_object_layer_set (txt, layer);
  //evas_object_text_style_set (txt, style);

  if (focusable && fcolor)
  {
    evas_object_event_callback_add (txt, EVAS_CALLBACK_FOCUS_IN,
                                    cb_text_focus_update, fcolor);
    evas_object_event_callback_add (txt, EVAS_CALLBACK_FOCUS_OUT,
                                    cb_text_focus_update, color);
  }

  return txt;
}

Evas_Object *
text_new (struct omc_t *omc, int focusable, struct font_t *font, char *str,
          int alpha, int layer, char *x, char *y)
{
  struct color_t *cl = NULL, *fcl = NULL;
  int x2, y2;

  x2 = omc_compute_coord (x, omc->w);
  y2 = omc_compute_coord (y, omc->h);

  if (font->color)
    cl = color_new (font->color, alpha);
  if (focusable && font->fcolor)
    fcl = color_new (font->fcolor, alpha);

  return evas_text_new (omc, focusable, font->ft, str, x2, y2,
                        layer, font->size, font->style, cl, fcl);

  if (cl)
    color_free (cl);
  if (fcl)
    color_free (fcl);
}

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
border_new (struct omc_t *omc, Evas_List *list,
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

/* Menu Object */
struct menu_t *
menu_new (struct omc_t *omc, struct font_t *font, int align,
          char *select, char *select_w, char *select_h,
          char *x, char *y, char *w, char *h)
{
  struct menu_t *menu = NULL;
  
  menu = (struct menu_t *) malloc (sizeof (struct menu_t));
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
menu_add_menuitem (struct omc_t *omc, struct menu_t *menu,
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
                                    cb_mouse_menu_show_selector, menu->select);
    evas_object_event_callback_add (item, EVAS_CALLBACK_MOUSE_OUT,
                                    cb_mouse_menu_hide_selector, menu->select);
  }

  menu->items = evas_list_append (menu->items, item);

  return item;
}

void
menu_compute_items (struct menu_t *menu)
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
menu_free (struct menu_t *menu)
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

/* Browser */
static struct item_t *
item_new (struct browser_t *browser, Evas_Object *icon, Evas_Object *text,
          int type, char *mrl, int mrl_type)
{
  struct item_t *item = NULL;

  item = (struct item_t *) malloc (sizeof (struct item_t));
  item->browser = browser; /* browser the item belongs to */
  item->icon = icon;
  item->text = text;
  item->clip = NULL;
  item->type = type;
  item->mrl = mrl ? strdup (mrl) : NULL;
  item->mrl_type = mrl_type;
  item->infos = NULL;
  item->artist = NULL;
  item->album = NULL;
  item->cover = NULL;
  item->updated = 0;
  
  return item;
}

static void
item_free (struct item_t *item)
{
  if (!item)
    return;

  if (item->icon)
    evas_object_del (item->icon);
  if (item->text)
    evas_object_del (item->text);
  if (item->clip)
    evas_object_del (item->clip);
  if (item->mrl)
    free (item->mrl);
  if (item->infos)
    free (item->infos);
  if (item->artist)
    free (item->artist);
  if (item->album)
    free (item->album);
  if (item->cover)
    free (item->cover);
  
  free (item);
}

#define BROWSER_THUMBNAIL_MAX_SIZE_W 164
#define BROWSER_THUMBNAIL_MAX_SIZE_H 110
#define BROWSER_THUMBNAIL_PADDING_W 20
#define BROWSER_THUMBNAIL_PADDING_H 25
#define BROWSER_TEXT_PADDING_H 10

static void
browser_compute (struct browser_t *browser)
{
  Evas_Coord x, y, w, h;
  Evas_Object *obj;
  int txt_size = 0, icon_size = 0;

  if (!browser->entries)
    return;

  if (browser->filter_type == FILTER_TYPE_IMAGE)
  {
    evas_object_geometry_get (browser->clip, &x, &y, &w, &h);

    obj = ((struct item_t *) browser->entries[0].data)->text;
    if (obj)
      evas_object_geometry_get (obj, NULL, NULL, NULL, &txt_size);

    browser->pos = 0;
    browser->capacity_w =
      (int) (w / (BROWSER_THUMBNAIL_MAX_SIZE_W
                  + BROWSER_THUMBNAIL_PADDING_W));
    browser->capacity_h =
      (int) (h / (BROWSER_THUMBNAIL_MAX_SIZE_H
                  + txt_size + BROWSER_THUMBNAIL_PADDING_H));

    return;
  }
  
  obj = ((struct item_t *) browser->entries[0].data)->text;
  if (obj)
    evas_object_geometry_get (obj, NULL, NULL, NULL, &txt_size);
  
  obj = ((struct item_t *) browser->entries[0].data)->icon;
  if (obj)
    evas_object_geometry_get (obj, NULL, NULL, NULL, &icon_size);
  
  browser->pos = 0;
  browser->capacity_w = 1;
  h = (txt_size > icon_size) ? txt_size : icon_size;
  browser->capacity_h = (int) (browser->h / (h + BROWSER_TEXT_PADDING_H)) - 1;
}

static void
browser_hide (struct browser_t *browser)
{
  Evas_List *l;
  for (l = browser->entries; l; l = l->next)
  {
    Evas_Object *obj = NULL;

    obj = ((struct item_t *) l->data)->text;
    if (obj)
      evas_object_hide (obj);
    obj = ((struct item_t *) l->data)->clip;
    if (obj)
      evas_object_hide (obj);
    obj = ((struct item_t *) l->data)->icon;
    if (obj)
      evas_object_hide (obj);
  }
}

static void
browser_display_update (struct browser_t *browser)
{
  Evas_List *l;
  Evas_Coord x, y;
  int count, start, end;

  if (!browser->entries)
    return;
  
  x = browser->x + BROWSER_THUMBNAIL_PADDING_W;
  y = browser->y;
  count = 0;
  
  /* hide previous browser item list */
  browser_hide (browser);

  /* determine which items should be displayed */
  if (browser->pos < browser->capacity_h)
  {
    start = 0;
    end = browser->capacity_h + 1;
  }
  else
  {
    start = browser->pos - browser->capacity_h;
    end = browser->pos + 1;
  }

  if (browser->filter_type == FILTER_TYPE_IMAGE)
  {
    int capacity_w = 0;
    for (l = evas_list_nth_list (browser->entries, browser->pos);
         l && count++ <= (int)(browser->capacity_w * browser->capacity_h) - 1;
         l = l->next)
    {
      struct item_t *item = NULL;
      Evas_Object *icon, *text;
      int txt_size = 0;
      
      item = (struct item_t *) l->data;
      if (!item)
        continue;

      icon = item->icon;
      if (icon)
      {
        Evas_Coord cx, cy, cw, ch;

        /* the image thumbnail hasn't been calculated yet */
        if (!item->updated && item->type == ITEM_TYPE_FILE)
        {
          evas_object_image_file_set (icon, item->mrl, NULL);
          evas_object_image_size_get (icon, &cw, &ch);

          /* do not consider small files (usually amazon retrieved dummy
             covers or failed FreeDesktop thumbnail files */
          if (cw <= 1 || ch <= 1)
            continue;
          
          if (cw == ch &&
              ((cw > BROWSER_THUMBNAIL_MAX_SIZE_W)
               || (ch > BROWSER_THUMBNAIL_MAX_SIZE_H)))
            cw = ch =
              (BROWSER_THUMBNAIL_MAX_SIZE_W > BROWSER_THUMBNAIL_MAX_SIZE_H) ?
              BROWSER_THUMBNAIL_MAX_SIZE_H : BROWSER_THUMBNAIL_MAX_SIZE_W;
          
          if (cw > 0 && cw > ch && cw > BROWSER_THUMBNAIL_MAX_SIZE_W)
          {
            int hmax = (int) (ch * BROWSER_THUMBNAIL_MAX_SIZE_W / cw);

            if (hmax < BROWSER_THUMBNAIL_MAX_SIZE_H)
            {
              ch = hmax;
              cw = BROWSER_THUMBNAIL_MAX_SIZE_W;
            }
            else
            {
              cw = (int) (cw * BROWSER_THUMBNAIL_MAX_SIZE_H / ch);
              ch = BROWSER_THUMBNAIL_MAX_SIZE_H;
            }
          }

          if (ch > 0 && ch > cw && ch > BROWSER_THUMBNAIL_MAX_SIZE_H)
          {
            int wmax = (int) (cw * BROWSER_THUMBNAIL_MAX_SIZE_H / ch);

            if (wmax < BROWSER_THUMBNAIL_MAX_SIZE_W)
            {
              cw = wmax;
              ch = BROWSER_THUMBNAIL_MAX_SIZE_H;
            }
            else
            {
              ch = (int) (ch * BROWSER_THUMBNAIL_MAX_SIZE_W / cw);
              cw = BROWSER_THUMBNAIL_MAX_SIZE_W;
            }
          }
  
          evas_object_resize (icon, cw, ch);
          evas_object_image_fill_set (icon, 0, 0, cw, ch);
          
          item->updated = 1;
        }

        evas_object_geometry_get (icon, &cx, &cy, &cw, &ch);

        cx = x;      
        if (cw > BROWSER_THUMBNAIL_MAX_SIZE_W)
          cw = BROWSER_THUMBNAIL_MAX_SIZE_W;
        else
          cx += (int) ((BROWSER_THUMBNAIL_MAX_SIZE_W - cw) / 2);

        cy = y;
        if (ch > BROWSER_THUMBNAIL_MAX_SIZE_H)
          ch = BROWSER_THUMBNAIL_MAX_SIZE_H;
        else
          cy += (int) ((BROWSER_THUMBNAIL_MAX_SIZE_H - ch) / 2);

        evas_object_move (icon, cx, cy);
        evas_object_show (icon);

        text = item->text;
        if (text)
        {
          evas_object_geometry_get (text, NULL, NULL, &cw, &ch);
          txt_size = ch;
          
          cx = x;      
          if (cw > BROWSER_THUMBNAIL_MAX_SIZE_W)
            cw = BROWSER_THUMBNAIL_MAX_SIZE_W;
          else
            cx += (int) ((BROWSER_THUMBNAIL_MAX_SIZE_W - cw) / 2);

          cy = y + txt_size + BROWSER_THUMBNAIL_MAX_SIZE_H;

          if (cw >= BROWSER_THUMBNAIL_MAX_SIZE_W)
          {
            char x2[8], y2[8], w2[8], h2[8];

            sprintf (x2, "%d", cx);
            sprintf (y2, "%d", cy);
            sprintf (w2, "%d", cw);
            sprintf (h2, "%d", ch);
            item->clip = object_clipper (omc, x2, y2, w2, h2);
          }
                  
          evas_object_move (text, cx, cy);
          evas_object_color_set (text, 255, 255, 255, 255);

          if (item->clip)
          {
            evas_object_clip_set (text, item->clip);
            evas_object_show (item->clip);
          }
          
          evas_object_show (text);
        }
      }
    
      capacity_w++;
      x += (int) (browser->w / browser->capacity_w);
      if (capacity_w == browser->capacity_w)
      {
        x = browser->x + BROWSER_THUMBNAIL_PADDING_W;
        y += BROWSER_THUMBNAIL_MAX_SIZE_H
          + txt_size + BROWSER_THUMBNAIL_PADDING_H;
        capacity_w = 0;
      }
    }

    return;
  }
  
  /* display items */
  x = browser->x;
  for (l = evas_list_nth_list (browser->entries, browser->pos);
       l && count++ <= browser->capacity_h; l = l->next)
  {
    struct item_t *item = NULL;
    Evas_Object *icon, *text;
    Evas_Coord ih = 0, th = 0, shift = 0, size = 0;

    item = (struct item_t *) l->data;
    if (!item)
      continue;
    
    icon = item->icon;
    if (icon)
    {
      evas_object_geometry_get (icon, NULL, NULL, &shift, &ih);
      if (shift)
        shift += 3;
      evas_object_move (icon, x, y);
      evas_object_show (icon);
    }

    text = item->text;
    if (text)
    {
      evas_object_geometry_get (text, NULL, NULL, NULL, &th);
      evas_object_move (text, x + shift, y);
      evas_object_color_set (text, 255, 255, 255, 255);
      evas_object_show (text);
    }

    size = (ih >= th) ? ih : th;
    y += size + BROWSER_TEXT_PADDING_H;    
  }
}

#define MAX_PATH_LEN 1024

static void
compute_directory (const char *dir, char *mrl)
{
  char newdir[MAX_PATH_LEN];
    
  if (!dir || !mrl)
    return;

  if (!strcmp (dir, ".."))
    sprintf (newdir, dirname (omc->cwd));
  else
    sprintf (newdir, mrl);

  omc_update_cwd (omc, newdir);
}

static void browser_update (struct omc_t *omc, struct browser_t *browser);

enum {
  MOUSE_BUTTON_LEFT = 1,
  MOUSE_BUTTON_WHEEL = 2,
  MOUSE_BUTTON_RIGHT = 3
};

static void
cb_browser_entry_execute (void *data, Evas *e,
                          Evas_Object *obj, void *event_info)
{
  struct item_t *item = NULL;
  const char *dir = NULL;
  Evas_Event_Mouse_Down *event = (Evas_Event_Mouse_Down *) event_info;

  item = (struct item_t *) data;
  if (!item)
    return;

  dir = evas_object_text_text_get (item->text);
  
  if (item->type == ITEM_TYPE_DIRECTORY)
  {
    compute_directory (dir, item->mrl);
    browser_update (omc, item->browser);
  }
  else if (item->type == ITEM_TYPE_FILE)
  {
    switch (item->mrl_type)
    {
    case PLAYER_MRL_TYPE_AUDIO:
      switch (event->button)
      {
      case MOUSE_BUTTON_LEFT:
        printf ("Play File : %s\n", item->mrl);
        player_add_mrl (omc->player, item, PLAYER_ADD_MRL_NOW);
        break;
      case MOUSE_BUTTON_RIGHT:
        printf ("Append File to playlist : %s\n", item->mrl);
        player_add_mrl (omc->player, item, PLAYER_ADD_MRL_QUEUE);
        break;
      default:
        break;
      }
      break;
    default:
      break;
    }
  } 
}

static void
cb_browser_mrl_execute (void *data, Evas *e,
                        Evas_Object *obj, void *event_info)
{
  struct item_t *item = NULL;
  Evas_Event_Mouse_Down *event = (Evas_Event_Mouse_Down *) event_info;

  item = (struct item_t *) data;
  if (!item)
    return;
  
  switch (event->button)
  {
  case MOUSE_BUTTON_LEFT:
  {
    struct mrl_t *mrl = NULL;
    Evas_List *list;
    
    printf ("Need to find MRL for %s\n", item->mrl);
    if (!omc->player || !omc->player->playlist)
      break;

    for (list = omc->player->playlist; list; list = list->next)
    {
      struct mrl_t *tmp = NULL;

      tmp = (struct mrl_t *) list->data;
      if (!tmp)
        continue;
      
      if (!strcmp (item->mrl, tmp->file))
      {
        mrl = tmp;
        break;
      }
    }

    if (mrl)
    {
      printf ("Found the corresponding MRL in playlist\n");
      omc->player->current = mrl;
      player_start (omc->player);
    }
    
    break;
  }
  default:
    break;
  }
}

static void
cb_browser_get_file_info (void *data, Evas *e,
                          Evas_Object *obj, void *event_info)
{
  struct item_t *item = NULL;

  item = (struct item_t *) data;
  if (!item)
    return;

  switch (omc->screen->type)
  {
  case SCREEN_TYPE_AUDIO:
    if (item->infos)
      screen_audio_update_infos (omc->screen, item->infos, 1);
    if (item->cover)
      screen_audio_update_cover (omc->screen, item->cover, 1);
    break;
  case SCREEN_TYPE_VIDEO:
    if (item->infos)
      screen_video_update_infos (omc->screen, item->infos, 1);
    if (item->cover)
      screen_video_update_cover (omc->screen, item->cover, 1);
    break;
  default:
    break;
  }
}

static void
cb_browser_hide_file_info (void *data, Evas *e,
                           Evas_Object *obj, void *event_info)
{
  switch (omc->screen->type)
  {
  case SCREEN_TYPE_AUDIO:
    screen_audio_update_infos (omc->screen, NULL, 0);
    screen_audio_update_cover (omc->screen, NULL, 0);
    break;
  case SCREEN_TYPE_VIDEO:
    screen_video_update_infos (omc->screen, NULL, 0);
    screen_video_update_cover (omc->screen, NULL, 0);
    break;
  default:
    break;
  }
}

static void
browser_prev_item (struct browser_t *browser)
{
  Evas_List *l;

  l = evas_list_nth_list (browser->entries, browser->pos);
  if (l && l->prev)
  {
    browser->pos--;
    browser_display_update (browser);
  }
}

static void
browser_next_item (struct browser_t *browser)
{
  Evas_List *l;

  l = evas_list_nth_list (browser->entries, browser->pos);
  if (l && l->next
      && (browser->pos + browser->capacity_h
          < evas_list_count (browser->entries) - 1))
  {
    browser->pos++;
    browser_display_update (browser);
  }
}

static void
cb_browser_mouse_wheel (void *data, Evas *e,
                        Evas_Object *obj, void *event_info)
{
  Evas_Event_Mouse_Wheel *event = event_info;
  struct browser_t *browser = NULL;
  int i;
  
  browser = (struct browser_t *) data;
  if (!browser)
    return;
  
  if (event->z < 0)
    for (i = 0; i < browser->capacity_w; i++)
      browser_prev_item (browser);
  else if (event->z > 0)
    for (i = 0; i < browser->capacity_w; i++)
      browser_next_item (browser);
}

#define IMG_ICON_FOLDER OMC_DATA_DIR"/folder.png"
#define IMG_ICON_FILE_AUDIO OMC_DATA_DIR"/music.png"
#define IMG_ICON_FILE_MOVIE OMC_DATA_DIR"/film.png"
#define IMG_ICON_FILE_IMAGE OMC_DATA_DIR"/picture.png"

static char *
getExtension (char *filename)
{
  char *delimiter =".";
  char *str, *token, *extension;

  if (!filename)
    return NULL;

  str = strdup (filename);
  token = strtok (str, delimiter);
  extension = strdup (token);

  while (token)
  {
    token = strtok (NULL, delimiter);
    if (token)
    {
      if (extension)
        free (extension);
      extension = strdup (token);
    }
  }

  free (str);

  return extension;
}

static void
browser_update (struct omc_t *omc, struct browser_t *browser)
{
  struct dirent **namelist;
  struct filter_t *filter = NULL;
  char *cwd_cover = NULL;
  int n, i;

  if (!browser)
    return;
  
  /* remove entries */
  if (browser->entries)
  {
    Evas_List *list;
    for (list = browser->entries; list; list = list->next)
    {
      struct item_t *item = NULL;
    
      item = (struct item_t *) list->data;
      if (!item)
        continue;
      
      item_free (item);
      browser->entries = evas_list_remove_list (browser->entries,
                                                browser->entries);
    }
    browser->entries = NULL;
  }

  if (browser->filter_type == FILTER_TYPE_MRL)
  {
    if (omc->player)
    {
      Evas_List *list;

      for (list = omc->player->playlist; list; list = list->next)
      {
        struct mrl_t *mrl;
        struct item_t *item = NULL;
        Evas_Object *txt = NULL;

        mrl = (struct mrl_t *) list->data;
        if (!mrl)
          continue;
        
        printf ("MRL : %s\n", mrl->file);
       
        txt = text_new (omc, 1, browser->font,
                        basename (mrl->file), 255, 5, "0", "0");
        object_add_default_cb (txt);
      
        if (browser->clip)
          evas_object_clip_set (txt, browser->clip);
       
        item = item_new (browser, NULL, txt,
                         ITEM_TYPE_FILE, mrl->file, mrl->type);
        item->infos = mrl->infos ? strdup (mrl->infos) : NULL;
        item->cover = mrl->cover ? strdup (mrl->cover) : NULL;
        
        evas_object_event_callback_add (txt, EVAS_CALLBACK_MOUSE_DOWN,
                                        cb_browser_mrl_execute, item);
        evas_object_event_callback_add (txt, EVAS_CALLBACK_MOUSE_WHEEL,
                                        cb_browser_mouse_wheel, browser);

        browser->entries = evas_list_append (browser->entries, item);
      }

      if (!browser->entries || evas_list_count (browser->entries) == 0)
      {
        /* no playlist right now */
        struct item_t *item = NULL;
        Evas_Object *txt = NULL;

        txt = text_new (omc, 0, browser->font,
                        "Empty Playlist ...", 255, 5, "0", "0");
       
        if (browser->clip)
          evas_object_clip_set (txt, browser->clip);
       
        item = item_new (browser, NULL, txt, ITEM_TYPE_FILE,
                         "empty", PLAYER_MRL_TYPE_NONE);
        browser->entries = evas_list_append (browser->entries, item);
      }
    }
  }
  else
  {
    filter = filter_get (omc->filters, browser->filter_type);
    if (!filter)
      return;

    n = scandir (omc->cwd, &namelist, 0, alphasort);
    if (n < 0)
    {
      perror ("scandir");
      return;
    }
  
    for (i = 0; i < n; i++)
    {
      struct item_t *item = NULL;
      Evas_Object *txt = NULL;
      Evas_Object *icon = NULL;
      struct stat st;
      char *path = NULL;
      int len, type = -1;
      int mrl_type = PLAYER_MRL_TYPE_NONE;
      
      /* do not consider hidden files */
      if ((namelist[i]->d_name[0] == '.' && namelist[i]->d_name[1] != '.'))
      {
        free (namelist[i]);
        continue;
      }
      
      if (!strcmp (namelist[i]->d_name, "..") && !strcmp (omc->cwd, "/"))
        continue;
      
      len = strlen (omc->cwd) + 1 + strlen (namelist[i]->d_name) + 1;
      path = (char *) malloc (len);
      if (!strcmp (omc->cwd, "/"))
        sprintf (path, "/%s", namelist[i]->d_name);
      else
        sprintf (path, "%s/%s", omc->cwd, namelist[i]->d_name);
      
      stat (path, &st);
      if (S_ISDIR (st.st_mode))
      {
        type = ITEM_TYPE_DIRECTORY;
        if (browser->filter_type == FILTER_TYPE_IMAGE)
          icon = image_new (omc, 0, IMG_ICON_FOLDER,
                            NULL, 0, "0", "0", "80", "55");
        else
          icon = image_new (omc, 0, IMG_ICON_FOLDER,
                            NULL, 0, "0", "0", "36", "25");
      }
      else if (S_ISREG (st.st_mode))
      {
        char *ext = NULL;
        
        ext = getExtension (namelist[i]->d_name);
        if (!ext)
          continue;
        
        if (!filter_supports_extension (filter, ext))
        {
          free (ext);
          continue;
        }
        
        type = ITEM_TYPE_FILE;
        switch (browser->filter_type)
        {
        case FILTER_TYPE_AUDIO:
          mrl_type = PLAYER_MRL_TYPE_AUDIO;
          icon = image_new (omc, 0, IMG_ICON_FILE_AUDIO,
                            NULL, 0, "0", "0", "21", "25");
          break;
        case FILTER_TYPE_VIDEO:
          mrl_type = PLAYER_MRL_TYPE_VIDEO;
          icon = image_new (omc, 0, IMG_ICON_FILE_MOVIE,
                            NULL, 0, "0", "0", "21", "25");
          break;
        case FILTER_TYPE_IMAGE:
          mrl_type = PLAYER_MRL_TYPE_IMAGE;
          icon = image_new (omc, 0, IMG_ICON_FILE_IMAGE,
                            NULL, 0, "0", "0", "51", "55");
          break;
        }
        
        free (ext);
      }

      txt = text_new (omc, 1, browser->font,
                      namelist[i]->d_name, 255, 5, "0", "0");
      object_add_default_cb (txt);
      
      if (browser->clip)
        evas_object_clip_set (txt, browser->clip);
      
      item = item_new (browser, icon, txt, type, path, mrl_type);
      
      if (item->type == ITEM_TYPE_FILE && mrl_type != PLAYER_MRL_TYPE_IMAGE)
      {
        if (omc->cfg->show_infos)
          grab_file_infos (item);
        
        if (omc->cfg->show_cover)
        {
          if (cwd_cover)
            item->cover = strdup (cwd_cover);
          else
            grab_file_covers (item);
        }
        
        if (item->cover && item->type == PLAYER_MRL_TYPE_AUDIO)
          cwd_cover = strdup (item->cover);
      }
      
      evas_object_event_callback_add (icon, EVAS_CALLBACK_MOUSE_DOWN,
                                      cb_browser_entry_execute, item);
      evas_object_event_callback_add (icon, EVAS_CALLBACK_MOUSE_WHEEL,
                                      cb_browser_mouse_wheel, browser);

      evas_object_event_callback_add (txt, EVAS_CALLBACK_MOUSE_DOWN,
                                      cb_browser_entry_execute, item);
      evas_object_event_callback_add (txt, EVAS_CALLBACK_MOUSE_WHEEL,
                                      cb_browser_mouse_wheel, browser);
      
      if (S_ISREG (st.st_mode))
      {
        evas_object_event_callback_add (txt, EVAS_CALLBACK_MOUSE_IN,
                                        cb_browser_get_file_info, item);
        evas_object_event_callback_add (txt, EVAS_CALLBACK_MOUSE_OUT,
                                        cb_browser_hide_file_info, NULL);
      }
      
      browser->entries = evas_list_append (browser->entries, item);
      
      free (path);
      free (namelist[i]);
    }
    free (namelist);
  }
  
  browser_compute (browser);
  browser_display_update (browser);
  screen_update_cwd (omc->screen);
}

Evas_Object *
object_clipper (struct omc_t *omc, char *x, char *y, char *w, char *h)
{
  Evas_Object *clip = NULL;
  Evas_Coord x2, y2, w2, h2;

  x2 = omc_compute_coord (x, omc->w);
  y2 = omc_compute_coord (y, omc->h);
  w2 = omc_compute_coord (w, omc->w);
  h2 = omc_compute_coord (h, omc->h);

  clip = evas_object_rectangle_add (omc->evas);

  evas_object_move (clip, x2, y2);
  evas_object_resize (clip, w2, h2);
  omc->screen->objects = evas_list_append (omc->screen->objects, clip);

  return clip;
}

struct browser_t *
browser_new (struct screen_t *screen, struct font_t *font, int filter_type,
             char *x, char *y, char *w, char *h)
{
  struct browser_t *browser = NULL;
  Evas_Object *b = NULL;
  
  browser = (struct browser_t *) malloc (sizeof (struct browser_t));

  browser->x = omc_compute_coord (x, omc->w);
  browser->y = omc_compute_coord (y, omc->h);
  browser->w = omc_compute_coord (w, omc->w);
  browser->h = omc_compute_coord (h, omc->h);

  browser->pos = 0;
  browser->capacity_w = 0;
  browser->capacity_h = 0;
  browser->filter_type = filter_type;
  browser->entries = NULL;
  browser->clip = NULL;
  browser->font = font;

  browser->clip = object_clipper (omc, x, y, w, h);
  b = border_new (omc, screen->objects, BORDER_TYPE_MENU, x, y, w, h);
  
  browser_update (omc, browser);

  evas_object_event_callback_add (b, EVAS_CALLBACK_MOUSE_WHEEL,
                                  cb_browser_mouse_wheel, browser);
  
  return browser;
}

void
browser_free (struct browser_t *browser)
{
  Evas_List *list;
  
  if (!browser)
    return;

  if (browser->entries)
  {
    for (list = browser->entries; list; list = list->next)
    {
      struct item_t *item = NULL;
      
      item = (struct item_t *) list->data;
      if (!item)
        continue;
      
      item_free (item);
      browser->entries = evas_list_remove_list (browser->entries,
                                                browser->entries);
    }
    free (browser->entries);
  }
  free (browser);
}

/* Textboxes */
void
text_block_format (Evas_Object *blk, int format)
{
  Evas_Textblock_Cursor *cursor = NULL;

  cursor = (Evas_Textblock_Cursor *) evas_object_textblock_cursor_get (blk);
  if (!cursor)
    return;
  
  switch (format)
    {
    case BLK_NEW_LINE:
      evas_textblock_cursor_format_append (cursor, "\n");
      break;
    case BLK_NEW_TAB:
      evas_textblock_cursor_format_append (cursor, "\t");
      break;
    default:
      return;
    }

}

#define TXT_BLOCK_SIZE 32768

void
text_block_add_text (Evas_Object *blk, char *text)
{
  Evas_Textblock_Cursor *cursor = NULL;
  char str[TXT_BLOCK_SIZE];
  int i, x = 0;

  if (!text)
    return;

  cursor = (Evas_Textblock_Cursor *) evas_object_textblock_cursor_get (blk);
  if (!cursor)
    return;
  
  bzero (str, TXT_BLOCK_SIZE);
  for (i = 0; i < strlen (text); i++)
  {
    if (text[i] == '\n')
    {
      evas_textblock_cursor_text_append (cursor, str);
      text_block_format (blk, BLK_NEW_LINE);
      bzero (str, TXT_BLOCK_SIZE);
      x = 0;
    }
    else if (text[i] == '\t')
    {
      evas_textblock_cursor_text_append (cursor, str);
      text_block_format (blk, BLK_NEW_TAB);
      bzero (str, TXT_BLOCK_SIZE);
      x = 0;
    }
    else
    {
      str[x] = text[i];
      x++;
    }
  }
  evas_textblock_cursor_text_append (cursor, str);
  evas_textblock_cursor_node_last (cursor);
}

static void
cb_textblock_edit (void *data, Evas *e, Evas_Object *obj, void *event_info)
{
  Evas_Event_Key_Down *event = (Evas_Event_Key_Down *) event_info;
  Evas_Textblock_Cursor *cursor = NULL;
  Evas_Object *cur = (Evas_Object *) data;
  Evas_Coord x = 0, y = 0, cx = 0, cy = 0, cw = 0, ch = 0;
  
  cursor = (Evas_Textblock_Cursor *) evas_object_textblock_cursor_get (obj);
  
  if (!event->keyname)
    return;

  if (!strcmp (event->keyname, "Left"))
    evas_textblock_cursor_char_prev (cursor);
  else if (!strcmp (event->keyname, "Right"))
    evas_textblock_cursor_char_next (cursor);
  else if (!strcmp (event->keyname, "Return"))
    text_block_format (obj, BLK_NEW_LINE);
  else if (!strcmp (event->keyname, "Tab"))
    text_block_format (obj, BLK_NEW_TAB);
  else if (!strcmp (event->keyname, "Delete"))
  {
    if (evas_textblock_cursor_char_next (cursor))
      evas_textblock_cursor_char_delete (cursor);
  }
  else if (!strcmp (event->keyname, "BackSpace"))
    evas_textblock_cursor_char_delete (cursor);
  else
  {
    if (event->string)
    {
      text_block_add_text (obj, (char *) event->string);
      evas_textblock_cursor_char_prev (cursor);
    }
  }

  evas_object_geometry_get (obj, &x, &y, NULL, NULL);
  evas_textblock_cursor_char_geometry_get (cursor, &cx, &cy, &cw, &ch);
  evas_object_move (cur, cx + x + cw, cy + y);
}

static void
cb_textblock_mouse_wheel (void *data, Evas *e,
                          Evas_Object *obj, void *event_info)
{

}

Evas_Object *
text_block_new (struct omc_t *omc, int editable, char *x, char *y,
                char *w, char *h, int layer, struct font_t *font,
                char *align_h, char *align_v)
{
  char style[1024];
  Evas_Object *blk = NULL;
  Evas_Textblock_Style *st;
  struct color_t *cl = NULL;
  Evas_Coord x2, y2, w2, h2;

  x2 = omc_compute_coord (x, omc->w);
  y2 = omc_compute_coord (y, omc->h);
  w2 = omc_compute_coord (w, omc->w);
  h2 = omc_compute_coord (h, omc->h);
  cl = color_new (font->color, 255);
  
  blk = evas_object_textblock_add (omc->evas);
  evas_object_move (blk, x2, y2);
  evas_object_resize (blk, w2, h2);
  evas_object_layer_set (blk, layer);
  evas_object_focus_set (blk, 1);

  evas_object_textblock_cursor_new (blk);

  sprintf (style, "DEFAULT='font=%s font_size=%d color=%sFF wrap=word align=%s valign=%s'", font->ft, font->size, font->color, align_h, align_v);
  
  st = evas_textblock_style_new ();
  evas_textblock_style_set (st, style);
  evas_object_textblock_style_set (blk, st);
  evas_textblock_style_free (st);
  evas_object_show (blk);

  if (editable)
  {
    Evas_Textblock_Cursor *cursor = NULL;
    Evas_Object *cur = NULL;
    Evas_Coord cx = 0, cy = 0, cw = 0, ch = 0;

    cursor = (Evas_Textblock_Cursor *) evas_object_textblock_cursor_get (blk);
    evas_textblock_cursor_node_last (cursor);
    evas_textblock_cursor_char_geometry_get (cursor, &cx, &cy, &cw, &ch);
  
    cur = evas_object_rectangle_add (omc->evas);
    evas_object_move (cur, cx + x2 + cw, cy + y2);
    evas_object_resize (cur, 2, ch);
    evas_object_color_set (cur, cl->r, cl->g, cl->b, cl->a);
    evas_object_layer_set (cur, 0);
    evas_object_show (cur);

    evas_object_event_callback_add (blk, EVAS_CALLBACK_KEY_DOWN,
                                    cb_textblock_edit, cur);
  }

  evas_object_event_callback_add (blk, EVAS_CALLBACK_MOUSE_WHEEL,
                                  cb_textblock_mouse_wheel, NULL);

  return blk;
}

/* Cover items */
struct cover_t *
cover_new (void)
{
  struct cover_t *cover = NULL;

  cover = (struct cover_t *) malloc (sizeof (struct cover_t));
  cover->border = NULL;
  cover->cover = NULL;

  return cover;
}

void
cover_free (struct cover_t *cover)
{
  Evas_List *list;
  
  if (!cover)
    return;

  if (cover->border)
  {
    for (list = cover->border; list; list = list->next)
    {
      Evas_Object *obj = NULL;
    
      obj = (Evas_Object *) list->data;
      if (!obj)
        continue;
      
      evas_object_del (obj);
      cover->border = evas_list_remove_list (cover->border, cover->border);
    }
    free (cover->border);
  }
  if (cover->cover)
    evas_object_del (cover->cover);
  free (cover);
}

/* Current Working Directory (CWD) items */
struct cwd_t *
cwd_new (void)
{
  struct cwd_t *cwd = NULL;

  cwd = (struct cwd_t *) malloc (sizeof (struct cwd_t));
  cwd->border = NULL;
  cwd->path = NULL;

  return cwd;
}

void
cwd_free (struct cwd_t *cwd)
{
  Evas_List *list;
  
  if (!cwd)
    return;

  if (cwd->border)
  {
    for (list = cwd->border; list; list = list->next)
    {
      Evas_Object *obj = NULL;
    
      obj = (Evas_Object *) list->data;
      if (!obj)
        continue;
      
      evas_object_del (obj);
      cwd->border = evas_list_remove_list (cwd->border, cwd->border);
    }
    free (cwd->border);
  }
  if (cwd->path)
    evas_object_del (cwd->path);
  free (cwd);
}

/* Notifier Widget */
struct notifier_t *
notifier_new (struct omc_t *omc, struct font_t *font,
              char *x, char *y, char *w, char *h)
{
  struct notifier_t *notifier = NULL;
  Evas_Object *dummy = NULL;
  int cx, cw, ch, padding;
  char x2[8], w2[8];
  
  notifier = (struct notifier_t *) malloc (sizeof (struct notifier_t));
  notifier->timer = NULL;
  notifier->border = NULL;
  notifier->cover = NULL;
  notifier->infos = NULL;
  notifier->show = 1;

  dummy = evas_object_image_add (omc->evas);
  notifier->border = evas_list_append (notifier->border, dummy);
  border_new (omc, notifier->border, BORDER_TYPE_PLAIN, x, y, w, h);

  notifier->cover =
    image_new (omc, 0, NULL, NULL, 10, x, y, h, h);

  cx = omc_compute_coord (x, omc->w);
  cw = omc_compute_coord (w, omc->w);
  ch = omc_compute_coord (h, omc->w);
  padding = omc_compute_coord ("3%", omc->w);
  sprintf (x2, "%d", cx + ch + padding);
  sprintf (w2, "%d", cw - ch - padding);
  
  notifier->infos = text_block_new (omc, 0, x2, y, w2, h, 5, font,
                                    BLK_ALIGN_LEFT, BLK_ALIGN_TOP);
  
  return notifier;
}

void
notifier_free (struct notifier_t *notifier)
{
  Evas_List *list;
  
  if (!notifier)
    return;

  if (notifier->timer)
    ecore_timer_del (notifier->timer);
  
  if (notifier->border)
  {
    for (list = notifier->border; list; list = list->next)
    {
      Evas_Object *obj = NULL;
    
      obj = (Evas_Object *) list->data;
      if (!obj)
        continue;
      
      evas_object_del (obj);
      notifier->border =
        evas_list_remove_list (notifier->border, notifier->border);
    }
    free (notifier->border);
  }
  
  if (notifier->cover)
    evas_object_del (notifier->cover);
  if (notifier->infos)
    evas_object_del (notifier->infos);
  free (notifier);
}

#define NOW_PLAYING_TEXT "Now Playing ...\n\n"

void
notifier_update (struct notifier_t *notifier, char *cover, char *infos)
{
  if (!notifier)
    return;
  
  if (cover && notifier->cover)
    evas_object_image_file_set (notifier->cover, cover, NULL);

  if (infos && notifier->infos)
  {
    char *txt = NULL;
    txt = (char *) malloc (strlen (infos) + strlen (NOW_PLAYING_TEXT) + 1);
    sprintf (txt, "%s%s", NOW_PLAYING_TEXT, infos);
    evas_object_textblock_clear (notifier->infos);
    text_block_add_text (notifier->infos, txt);
    free (txt);
  }
  
  if (notifier->show)
    notifier_show (notifier);
}

static int notifier_update_alpha (void *data);

static int
notifier_timer_expired (void *data)
{
  struct notifier_t *notifier = NULL;

  notifier = (struct notifier_t *) data;
  if (!notifier)
    return 0;

  notifier->timer =
    ecore_timer_add (0.1, notifier_update_alpha, (void *) notifier);

  return 0;
}

static int
notifier_update_alpha (void *data)
{
  struct notifier_t *notifier = NULL;
  Evas_Coord alpha = 0;
  Evas_List *list;

  notifier = (struct notifier_t *) data;
  if (!notifier)
    return 0;

  if (notifier->border)
  {
    for (list = notifier->border; list; list = list->next)
    {
      Evas_Object *obj = NULL;
      
      obj = (Evas_Object *) list->data;
      if (!obj)
        continue;
      
      evas_object_color_get (obj, NULL, NULL, NULL, &alpha);
      if (notifier->show && alpha < 255)
        evas_object_color_set (obj, 255, 255, 255, alpha + 32);
      else if (!(notifier->show) && alpha > 0)
        evas_object_color_set (obj, 255, 255, 255, alpha - 32);
    }
  }

  if (notifier->cover)
  {
    evas_object_color_get (notifier->cover, NULL, NULL, NULL, &alpha);
    if (notifier->show && alpha < 255)
      evas_object_color_set (notifier->cover, 255, 255, 255, alpha + 32);
    else if (!(notifier->show) && alpha > 0)
      evas_object_color_set (notifier->cover, 255, 255, 255, alpha - 32);
  }

  if (notifier->infos)
  {
    evas_object_color_get (notifier->infos, NULL, NULL, NULL, &alpha);
    if (notifier->show && alpha < 255)
      evas_object_color_set (notifier->infos, 255, 255, 255, alpha + 32);
    else if (!(notifier->show) && alpha > 0)
      evas_object_color_set (notifier->infos, 255, 255, 255, alpha - 32);
  }
  
  if (notifier->show && alpha >= 255)
  {
    notifier->show = 0;
    notifier->timer =
      ecore_timer_add (5.0, notifier_timer_expired, (void *) notifier);
    return 0;
  }
  else if (!(notifier->show) && alpha <= 0)
  {
    notifier->show = 1;
    notifier_hide (notifier);
    return 0;
  }
  
  return 1; /* to keep the timer going */
}

void
notifier_show (struct notifier_t *notifier)
{
  Evas_List *list;

  if (!notifier)
    return;
  
  if (notifier->border)
  {
    for (list = notifier->border; list; list = list->next)
    {
      Evas_Object *obj = NULL;
      
      obj = (Evas_Object *) list->data;
      if (!obj)
        continue;
      
      evas_object_color_set (obj, 255, 255, 255, 0);
      evas_object_show (obj);
    }
  }

  if (notifier->cover)
  {
    evas_object_color_set (notifier->cover, 255, 255, 255, 0);
    evas_object_show (notifier->cover);
  }

  if (notifier->infos)
  {
    evas_object_color_set (notifier->infos, 255, 255, 255, 0);
    evas_object_show (notifier->infos);
  }

  notifier->timer =
    ecore_timer_add (0.1, notifier_update_alpha, (void *) notifier);
}

void
notifier_hide (struct notifier_t *notifier)
{
  Evas_List *list;

  if (!notifier)
    return;
  
  if (notifier->border)
  {
    for (list = notifier->border; list; list = list->next)
    {
      Evas_Object *obj = NULL;
      
      obj = (Evas_Object *) list->data;
      if (!obj)
        continue;
      
      evas_object_hide (obj);
    }
  }

  if (notifier->cover)
   evas_object_hide (notifier->cover);

  if (notifier->infos)
   evas_object_hide (notifier->infos);
}
