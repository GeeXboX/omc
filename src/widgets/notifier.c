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

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "omc.h"

notifier_t *
notifier_new (omc_t *omc, font_t *font,
              char *x, char *y, char *w, char *h)
{
  notifier_t *notifier = NULL;
  Evas_Object *dummy = NULL;
  int cx, cw, ch;
  char x2[8], w2[8];
  
  notifier = (notifier_t *) malloc (sizeof (notifier_t));
  notifier->timer = NULL;
  notifier->border = NULL;
  notifier->cover = NULL;
  notifier->infos = NULL;
  notifier->show = 1;
  notifier->padding = 0;

  dummy = evas_object_image_add (omc->evas);
  notifier->border = evas_list_append (notifier->border, dummy);
  border_new (omc, notifier->border, BORDER_TYPE_PLAIN, x, y, w, h);

  notifier->cover =
    image_new (omc, 0, NULL, NULL, 10, x, y, h, h);

  cx = omc_compute_coord (x, omc->w);
  cw = omc_compute_coord (w, omc->w);
  ch = omc_compute_coord (h, omc->w);
  notifier->padding = omc_compute_coord ("3%", omc->w);
  sprintf (x2, "%d", cx + ch + notifier->padding);
  sprintf (w2, "%d", cw - ch - notifier->padding);
  
  notifier->infos = text_block_new (omc, 0, x2, y, w2, h, 5, font,
                                    BLK_ALIGN_LEFT, BLK_ALIGN_TOP);
  
  return notifier;
}

void
notifier_free (notifier_t *notifier)
{
  Evas_List *list;
  
  if (!notifier)
    return;

  if (notifier->timer)
  {
    ecore_timer_del (notifier->timer);
    notifier->timer = NULL;
  }
  
  if (notifier->border)
  {
    for (list = notifier->border; list; list = list->next)
    {
      Evas_Object *obj = NULL;
    
      obj = (Evas_Object *) list->data;
      if (!obj)
        continue;
      
      evas_object_del (obj);
      notifier->border =
        evas_list_remove_list (notifier->border, notifier->border);
    }
    free (notifier->border);
  }
  
  if (notifier->cover)
    evas_object_del (notifier->cover);
  if (notifier->infos)
    evas_object_del (notifier->infos);
  free (notifier);
}

#define NOW_PLAYING_TEXT "Now Playing ...\n\n"

void
notifier_update (notifier_t *notifier, char *cover, char *infos)
{
  Evas_Coord x, y, w, cw, ch;

  if (!notifier)
    return;
  
  if (cover && notifier->cover)
    evas_object_image_file_set (notifier->cover, cover, NULL);

  evas_object_geometry_get (notifier->cover, &x, &y, &w, NULL);
  evas_object_image_size_get (notifier->cover, &cw, &ch);

  if (cw == 0 && ch == 0) /* no cover */
    evas_object_move (notifier->infos, x, y);
  else
    evas_object_move (notifier->infos, x + w + notifier->padding, y);

  if (infos && notifier->infos)
  {
    char *txt = NULL;
    txt = (char *) malloc (strlen (infos) + strlen (NOW_PLAYING_TEXT) + 1);
    sprintf (txt, "%s%s", NOW_PLAYING_TEXT, infos);
    evas_object_textblock_clear (notifier->infos);
    text_block_add_text (notifier->infos, txt);
    free (txt);
  }
  
  if (notifier->show)
    notifier_show (notifier);
}

static int notifier_update_alpha (void *data);

static int
notifier_timer_expired (void *data)
{
  notifier_t *notifier = NULL;

  notifier = (notifier_t *) data;
  if (!notifier)
    return 0;

  if (notifier->timer)
  {
    ecore_timer_del (notifier->timer);
    notifier->timer = NULL;
  }
  
  notifier->timer =
    ecore_timer_add (0.1, notifier_update_alpha, (void *) notifier);

  return 0;
}

static int
notifier_update_alpha (void *data)
{
  notifier_t *notifier = NULL;
  Evas_Coord alpha = 0;
  Evas_List *list;

  notifier = (notifier_t *) data;
  if (!notifier)
    return 0;

  if (notifier->border)
  {
    for (list = notifier->border; list; list = list->next)
    {
      Evas_Object *obj = NULL;
      
      obj = (Evas_Object *) list->data;
      if (!obj)
        continue;
      
      evas_object_color_get (obj, NULL, NULL, NULL, &alpha);
      if (notifier->show && alpha < 255)
        evas_object_color_set (obj, 255, 255, 255, alpha + 32);
      else if (!(notifier->show) && alpha > 0)
        evas_object_color_set (obj, 255, 255, 255, alpha - 32);
    }
  }

  if (notifier->cover)
  {
    evas_object_color_get (notifier->cover, NULL, NULL, NULL, &alpha);
    if (notifier->show && alpha < 255)
      evas_object_color_set (notifier->cover, 255, 255, 255, alpha + 32);
    else if (!(notifier->show) && alpha > 0)
      evas_object_color_set (notifier->cover, 255, 255, 255, alpha - 32);
  }

  if (notifier->infos)
  {
    evas_object_color_get (notifier->infos, NULL, NULL, NULL, &alpha);
    if (notifier->show && alpha < 255)
      evas_object_color_set (notifier->infos, 255, 255, 255, alpha + 32);
    else if (!(notifier->show) && alpha > 0)
      evas_object_color_set (notifier->infos, 255, 255, 255, alpha - 32);
  }
  
  if (notifier->show && alpha >= 255)
  {
    notifier->show = 0;
    ecore_timer_del (notifier->timer);
    notifier->timer = NULL;
    notifier->timer =
      ecore_timer_add (5.0, notifier_timer_expired, (void *) notifier);
    return 0;
  }
  else if (!(notifier->show) && alpha <= 0)
  {
    notifier->show = 1;
    notifier_hide (notifier);
    return 0;
  }
  
  return 1; /* to keep the timer going */
}

void
notifier_show (notifier_t *notifier)
{
  Evas_List *list;

  if (!notifier)
    return;
  
  if (notifier->border)
  {
    for (list = notifier->border; list; list = list->next)
    {
      Evas_Object *obj = NULL;
      
      obj = (Evas_Object *) list->data;
      if (!obj)
        continue;
      
      evas_object_color_set (obj, 255, 255, 255, 0);
      evas_object_show (obj);
    }
  }

  if (notifier->cover)
  {
    evas_object_color_set (notifier->cover, 255, 255, 255, 0);
    evas_object_show (notifier->cover);
  }

  if (notifier->infos)
  {
    evas_object_color_set (notifier->infos, 255, 255, 255, 0);
    evas_object_show (notifier->infos);
  }

  notifier->timer =
    ecore_timer_add (0.1, notifier_update_alpha, (void *) notifier);
}

void
notifier_hide (notifier_t *notifier)
{
  Evas_List *list;

  if (!notifier)
    return;
  
  if (notifier->border)
  {
    for (list = notifier->border; list; list = list->next)
    {
      Evas_Object *obj = NULL;
      
      obj = (Evas_Object *) list->data;
      if (!obj)
        continue;
      
      evas_object_hide (obj);
    }
  }

  if (notifier->cover)
   evas_object_hide (notifier->cover);

  if (notifier->infos)
   evas_object_hide (notifier->infos);
}
