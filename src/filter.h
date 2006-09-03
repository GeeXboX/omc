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

#ifndef _FILE_FILTERS_H_
#define _FILE_FILTERS_H_

enum {
  FILTER_TYPE_NONE,
  FILTER_TYPE_AUDIO,
  FILTER_TYPE_VIDEO,
  FILTER_TYPE_IMAGE,
  FILTER_TYPE_MRL
};

typedef struct filter_s {
  int type;
  Evas_List *exts;
} filter_t;

filter_t *filter_new (int type);
void filter_free (filter_t *filter);

int filter_supports_extension (filter_t *filter, char *extension);
filter_t *filter_get (Evas_List *filter_list, int type);

#endif /* _FILE_FILTERS_H_ */
