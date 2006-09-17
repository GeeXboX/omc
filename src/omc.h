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

#include "config.h"

#include <Evas.h>
#include <Ecore.h>
#include <Ecore_Evas.h>
#include <player.h>

#include "cfgparser.h"
#include "screens/screen.h"

typedef struct omc_s {
  Evas *evas;  
  Ecore_Evas *ee;
  Evas_List *filters;
  config_t *cfg;
  char *cwd;
  screen_t *screen;
  player_t *player;
  struct demuxer_s *demuxer;
  int w;
  int h;
} omc_t;

#include "widgets/widget.h"
#include "av_demuxer.h"
#include "avplayer.h"

void omc_update_cwd (omc_t *omc, char *dir);

omc_t *omc;

#define OMC_DEFAULT_WIDTH  800
#define OMC_DEFAULT_HEIGHT 600
#define OMC_WM_TITLE "GeeXboX Open Media Center"
#define OMC_DATA_DIR DATA_DIR
#define OMC_MEDIA_DIR "/"

#endif /* _OMC_H_ */
