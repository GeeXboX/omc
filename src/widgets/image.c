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
#include <Evas.h>
#include <Ecore.h>
#include <player.h>

#include "screens/screen.h"
#include "omc.h"
#include "widget.h"

/* Callbacks */
static void
cb_img_focus_update (void *data, Evas *e, Evas_Object *obj, void *event_info)
{
  char *file = (char *) data;
  if (obj)
    evas_object_image_file_set (obj, file, NULL);
}

/* Image Object */
static Evas_Object *
evas_image_new (omc_t *omc, int focusable, char *name, char *fname,
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
image_new (omc_t *omc, int focusable, char *name, char *fname,
           int layer, char *x, char *y, char *w, char *h)
{
  int x2, y2, w2, h2;

  x2 = omc_compute_coord (x, omc->w);
  y2 = omc_compute_coord (y, omc->h);
  w2 = omc_compute_coord (w, omc->w);
  h2 = omc_compute_coord (h, omc->h);
    
  return evas_image_new (omc, focusable, name, fname, layer, x2, y2, w2, h2);
}

