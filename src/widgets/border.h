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

#ifndef _WIDGET_BORDER_H_
#define _WIDGET_BORDER_H_

enum {
  BORDER_TYPE_PLAIN,
  BORDER_TYPE_MENU,
  BORDER_TYPE_COVER
};

Evas_Object *border_new (omc_t *omc, Evas_List *list,
                         int type, char *cx, char *cy, char *cw, char *ch);

#endif /* _WIDGET_BORDER_H_ */
