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
  struct cwd_t *cwd;
};

static struct simage_t *
simage_new (void)
{
  struct simage_t *simage = NULL;

  simage = (struct simage_t *) malloc (sizeof (struct simage_t));
  simage->browser = NULL;
  simage->cwd = NULL;
  
  return simage;
}

static void
simage_free (struct simage_t *simage)
{
  if (!simage)
    return;

  if (simage->browser)
    browser_free (simage->browser);
  if (simage->cwd)
    cwd_free (simage->cwd);
  free (simage);
}

#define CWD_TEXT "Current Path : "

void
screen_image_update_cwd (struct screen_t *screen)
{
  struct simage_t *simage = NULL;
  struct cwd_t *cwd = NULL;
  
  if (!screen || screen->type != SCREEN_TYPE_IMAGE)
    return;

  simage = (struct simage_t *) screen->private;
  if (!simage)
    return;

  cwd = simage->cwd;
  if (!cwd)
    return;

  if (cwd->path)
  {
    char path[1024];

    sprintf (path, "%s%s", CWD_TEXT, omc->cwd);
    evas_object_textblock_clear (cwd->path);
    text_block_add_text (cwd->path, path);
  }
}

static void
simage_cwd_setup (struct screen_t *screen)
{
  struct simage_t *simage = NULL;
  struct font_t *font = NULL;
  Evas_Object *dummy = NULL;
  
  font = get_font (omc->cfg->fonts, "browser_small");
  if (!font)
    return;
  
  simage = (struct simage_t *) screen->private;
  if (!simage)
    return;

  simage->cwd = cwd_new ();
  if (!simage->cwd)
    return;

  
  dummy = evas_object_image_add (omc->evas);
  simage->cwd->border = evas_list_append (simage->cwd->border, dummy);

  border_new (omc, simage->cwd->border,
              BORDER_TYPE_COVER, "25%", "3%", "100%", "4%");

  simage->cwd->path =
    text_block_new (omc, 0, "25%", "3%", "72%", "4%", 0, font,
                    BLK_ALIGN_LEFT, BLK_ALIGN_CENTER);
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
screen_image_setup (struct screen_t *screen, char *data)
{
  if (!screen || screen->type != SCREEN_TYPE_IMAGE)
    return;

  screen->private = (struct audio_t *) simage_new ();
  
  widget_background_setup (screen);
  widget_common_toolbar_setup (screen);
  browser_filter_toolbar_setup (screen);

  simage_cwd_setup (screen);
  simage_browser_setup (screen);
}

void
screen_image_display (struct screen_t *screen)
{
  struct simage_t *simage = NULL;
  struct cwd_t *cwd = NULL;

  simage = (struct simage_t *) screen->private;
  if (!simage)
    return;

  cwd = simage->cwd;
  if (!cwd)
    return;
  
  if (cwd->border)
  {
    Evas_List *list;
    for (list = cwd->border; list; list = list->next)
    {
      Evas_Object *obj;
      
      obj = (Evas_Object *) list->data;
      evas_object_show (obj);
    }
  }
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

