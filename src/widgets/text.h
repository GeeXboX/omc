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

#ifndef _WIDGET_TEXT_H_
#define _WIDGET_TEXT_H_

Evas_Object * text_new (omc_t *omc, int focusable, font_t *font, char *str,
                        int alpha, int layer, char *x, char *y);

widget_t *widget_text_new (char *id, uint32_t x, uint32_t y, uint32_t layer,
                           char *str, font_t *font, uint32_t alpha);

#endif /* _WIDGET_TEXT_H_ */
