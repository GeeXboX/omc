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

#ifndef _WIDGET_H_
#define _WIDGET_H_

#include "image.h"
#include "color.h"
#include "text.h"
#include "textblock.h"
#include "notifier.h"
#include "border.h"
#include "menu.h"
#include "cover.h"
#include "clipper.h"
#include "browser.h"
#include "item.h"
#include "cwd.h"

int omc_compute_coord (char *coord, int max);

void object_add_default_cb (Evas_Object *obj);

char *getExtension (char *filename);

#endif /* _WIDGET_H_ */
