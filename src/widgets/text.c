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

/* Callbacks */
static void
cb_text_focus_update (void *data, Evas *e, Evas_Object *obj, void *event_info)
{
  color_t *color = (color_t *) data;
  if (obj)
    evas_object_color_set (obj, color->r, color->g, color->b, color->a);
}

/* Text Object */
static Evas_Object *
evas_text_new (omc_t *omc, int focusable, char *font, char *str,
               int x, int y, int layer, int size, int style,
               color_t *color, color_t *fcolor)
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
text_new (omc_t *omc, int focusable, font_t *font, char *str,
          int alpha, int layer, char *x, char *y)
{
  Evas_Object *txt = NULL;
  color_t *cl = NULL, *fcl = NULL;
  int x2, y2;

  x2 = omc_compute_coord (x, omc->w);
  y2 = omc_compute_coord (y, omc->h);

  if (font->color)
    cl = color_new (font->color, alpha);
  if (focusable && font->fcolor)
    fcl = color_new (font->fcolor, alpha);

  txt = evas_text_new (omc, focusable, font->ft, str, x2, y2,
                       layer, font->size, font->style, cl, fcl);

  if (cl)
    color_free (cl);
  if (fcl)
    color_free (fcl);

  return txt;
}
