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

#ifndef _GFX_H_
#define _GFX_H_

/* Image Positionning */

enum {
  ROTATE_CLOCKWISE,
  ROTATE_COUNTERCLOCKWISE
};

void image_fit_to_screen (Evas_Object *img);
void image_center (Evas_Object *img);
void image_move (Evas_Object *img, int x, int y);
void image_rotate (Evas_Object *img, int direction);
void image_flip_horizontal (Evas_Object *img);
void image_flip_vertical (Evas_Object *img);

/* Zoom Facilities */

#define MAX_ZOOM_LEVEL 5.00 /* 500 % */
#define MIN_ZOOM_LEVEL 0.05 /* 5 % */
#define ZOOM_STEP 1.25 /* 25 % */

void image_zoom_in (Evas_Object *img);
void image_zoom_out (Evas_Object *img);
void image_zoom_1_1 (Evas_Object *img);

/* Image Transformation */

void image_blur (Evas_Object *img);
void image_sharpen (Evas_Object *img);

#endif /* _GFX_H_ */
