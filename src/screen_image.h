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

#ifndef _SCREEN_IMAGE_H_
#define _SCREEN_IMAGE_H_

#define SCREEN_IMAGE_TITLE "image"
#define SCREEN_IMAGE_OBJ_HEADERS "headers_image"
#include "screen.h"

void screen_image_setup (screen_t *screen, char *data);
void screen_image_display (screen_t *screen);
void screen_image_free (screen_t *screen);
void screen_image_update_cwd (screen_t *screen);

#endif /* _SCREEN_IMAGE_H_ */
