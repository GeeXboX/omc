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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "omc.h"

/* Coordinates */
int
omc_compute_coord (char *coord, int max)
{
  int val = 0;
  char *end;
  
  if (!coord)
    return 0;

  val = strtol (coord, &end, 10);
  if (*end == '%')
    val = val * max / 100;

  return val;
}

/* Callbacks */
static void
cb_mouse_in (void *data, Evas *e, Evas_Object *obj, void *event_info)
{
  if (obj)
    evas_object_focus_set (obj, 1);
}

static void
cb_mouse_out (void *data, Evas *e, Evas_Object *obj, void *event_info)
{
  if (obj)
    evas_object_focus_set (obj, 0);
}

void
object_add_default_cb (Evas_Object *obj)
{
  if (!obj)
    return;
  
  evas_object_event_callback_add (obj, EVAS_CALLBACK_MOUSE_IN,
                                  cb_mouse_in, NULL);
  evas_object_event_callback_add (obj, EVAS_CALLBACK_MOUSE_OUT,
                                  cb_mouse_out, NULL);
}

char *
getExtension (char *filename)
{
  char *delimiter =".";
  char *str, *token, *extension;

  if (!filename)
    return NULL;

  str = strdup (filename);
  token = strtok (str, delimiter);
  extension = strdup (token);

  while (token)
  {
    token = strtok (NULL, delimiter);
    if (token)
    {
      if (extension)
        free (extension);
      extension = strdup (token);
    }
  }

  free (str);

  return extension;
}
