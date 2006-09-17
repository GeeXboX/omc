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

#include "omc.h"

Evas_Object *
object_clipper (omc_t *omc, char *x, char *y, char *w, char *h)
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
