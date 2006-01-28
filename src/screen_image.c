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
#include <stdlib.h>

#include "omc.h"
#include "widget.h"
#include "filter.h"
#include "screen.h"
#include "screen_image.h"

struct simage_t {
  struct browser_t *browser;
};

static struct simage_t *
simage_new (void)
{
  struct simage_t *simage = NULL;

  simage = (struct simage_t *) malloc (sizeof (struct simage_t));
  simage->browser = NULL;
  
  return simage;
}

static void
simage_free (struct simage_t *simage)
{
  if (!simage)
    return;

  if (simage->browser)
    browser_free (simage->browser);
  free (simage);
}

static void
simage_browser_setup (struct screen_t *screen)
{
  struct simage_t *simage = NULL;
  struct font_t *font = NULL;

  font = get_font (omc->cfg->fonts, "browser_small");
  if (!font)
    return;
  
  simage = (struct simage_t *) screen->private;
  if (!simage)
    return;

  simage->browser =
    browser_new (omc->screen, font,
                 FILTER_TYPE_IMAGE, "14%", "15%", "80%", "80%");
}

void
screen_image_setup (struct screen_t *screen)
{
  if (!screen || screen->type != SCREEN_TYPE_IMAGE)
    return;

  screen->private = (struct audio_t *) simage_new ();
  
  widget_background_setup (screen);
  widget_common_toolbar_setup (screen);
  browser_filter_toolbar_setup (screen);

  simage_browser_setup (screen);
}

void
screen_image_display (struct screen_t *screen)
{
  struct simage_t *simage = (struct simage_t *) screen->private;

  if (!simage)
    return;
}

void
screen_image_free (struct screen_t *screen)
{
  struct simage_t *simage = NULL;
  
  if (!screen || screen->type != SCREEN_TYPE_IMAGE)
    return;

  simage = (struct simage_t *) screen->private;
  if (simage)
    simage_free (simage);
}

