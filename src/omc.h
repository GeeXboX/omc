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

#ifndef _OMC_H_
#define _OMC_H_

#ifdef HAVE_CONFIG_H
#include "config.h"
#else
#error "Missing config.h file : run configure again"
#endif

#include <Evas.h>
#include <Ecore_Evas.h>

struct omc_t {
  Evas *evas;  
  Ecore_Evas *ee;
  Evas_List *filters;
  struct config_t *cfg;
  char *cwd;
  struct screen_t *screen;
  struct player_t *player;
  int w;
  int h;
};

void omc_update_cwd (struct omc_t *omc, char *dir);

struct omc_t *omc;

#define OMC_DEFAULT_WIDTH  640
#define OMC_DEFAULT_HEIGHT 480
#define OMC_WM_TITLE "GeeXboX Open Media Center"
#define OMC_DATA_DIR DATA_DIR
#define OMC_MEDIA_DIR "/"

#endif /* _OMC_H_ */
