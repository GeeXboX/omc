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

#include <Evas.h>
#include <Ecore.h>
#include <Ecore_Evas.h>

#include <string.h>
#include <stdio.h>

#include "screen.h"
#include "omc.h"
#include "widget.h"
#include "screen_settings.h"

#define IMG_ICON_LOGO_SETTINGS OMC_DATA_DIR"/logo_set_small.png"

static void
settings_headers_setup (screen_t *screen)
{
  Evas_Object *obj = NULL;
  font_t *font = NULL;

  font = get_font (omc->cfg->fonts, "header");
  if (!font)
    return;
  
  obj = image_new (omc, 0, IMG_ICON_LOGO_SETTINGS,
                   NULL, 0, "1%", "2%", "80", "60");
  screen->objects = evas_list_append (screen->objects, obj);

  obj = text_new (omc, 0, font, "Settings", 255, 0, "15%", "3%");
  screen->objects = evas_list_append (screen->objects, obj);
}

void
screen_settings_setup (screen_t *screen, char *data)
{
  widget_background_setup (screen);
  widget_common_toolbar_setup (screen);
  
  settings_headers_setup (screen);
}
