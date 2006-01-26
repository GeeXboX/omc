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

#ifndef _CONFIG_PARSER_H_
#define _CONFIG_PARSER_H_

struct font_t {
  char *id;
  char *ft;
  int size;
  int style;
  char *color;
  char *fcolor;
};

struct config_t {
  char *media_dir;
  char *screenw;
  char *screenh;
  Evas_List *fonts;
  int show_infos;
  int show_cover;
  int save_cover;
};

struct config_t *parse_config (void);
void config_free (struct config_t *cfg);

struct font_t *get_font (Evas_List *list, char *id);

#endif /* _CONFIG_PARSER_H_ */
