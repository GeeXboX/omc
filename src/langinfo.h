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

#ifndef _LANG_INFO_H_
#define _LANG_INFO_H_

typedef struct lang_country_s {
  char *code;
  char *description;
} lang_country_t;

typedef struct lang_charset_s {
  char *gui;
  char *subs;
} lang_charset_t;

typedef struct lang_dvd_s {
  char *menu;
  char *audio;
  char *subs;
} lang_dvd_t;

typedef enum {
  DATE_TYPE_DAY_NAME,
  DATE_TYPE_DAY_NUMBER,
  DATE_TYPE_MONTH_NAME,
  DATE_TYPE_MONTH_NUMBER,
  DATE_TYPE_YEAR
} date_type_t;

/* something under the the form "MM/DD/YYYY" or "DAY, MONTH DD, YYYY" */
typedef struct region_date_s {
  date_type_t first;
  date_type_t second;
  date_type_t third;
  date_type_t fourth;
} region_date_t;

typedef enum {
  TIME_12H,
  TIME_24H
} time_unit_t;

typedef enum {
  TEMPERATURE_CELSIUS,
  TEMPERATURE_FARENHEIT
} temp_unit_t;

typedef enum {
  SPEED_KMH,
  SPEED_MPH
} speed_unit_t;

typedef struct lang_region_s {
  char *name;
  region_date_t *date_short;
  region_date_t *date_long;
  time_unit_t time;
  temp_unit_t temperature;
  speed_unit_t speed;
} lang_region_t;

typedef struct lang_regions_s {
  lang_region_t **list;
} lang_regions_t;

typedef struct lang_string_s {
  char *id;
  char *str;
} lang_string_t;

typedef struct lang_strings_s {
  lang_string_t **list;
} lang_strings_t;

typedef struct lang_info_s {
  lang_country_t *country;
  lang_charset_t *charset;
  lang_dvd_t *dvd;
  lang_regions_t *regions;
  lang_strings_t *strings;
} lang_info_t;

lang_info_t *lang_info_parse (char *lang);
void lang_info_free (lang_info_t *info);
void lang_strings_parse (lang_info_t *lang);
const char *lang_get_string (lang_info_t *lang, char *id);

#endif /* _LANG_INFO_H_ */
