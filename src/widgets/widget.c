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

widget_t *
widget_new (char *id, widget_type_t type,
            uint32_t x, uint32_t y,
            uint32_t w, uint32_t h,
            uint32_t layer, uint32_t show, uint32_t focusable)
{
  widget_t *widget = NULL;

  if (!id) /* mandatory */
    return NULL;
  
  widget = (widget_t *) malloc (sizeof (widget_t));
  widget->id = strdup (id);
  widget->type = type;
  widget->flags = WIDGET_FLAG_NONE;
  widget->x = x;
  widget->y = y;
  widget->w = w;
  widget->h = h;
  widget->focus = NULL;
  widget->priv = NULL;

  if (show)
    widget->flags |= WIDGET_FLAG_SHOW;
  if (focusable)
    widget->flags |= WIDGET_FLAG_FOCUSABLE;
  
  widget->show = NULL;
  widget->hide = NULL;
  widget->set_focus = NULL;
  widget->action = NULL;
  widget->free = NULL;

  return widget;
}

int
widget_show (widget_t *widget)
{
  if (widget && widget->show)
    if (!(widget->flags & WIDGET_FLAG_SHOW)) /* current state: hidden */
      return widget->show (widget);

  return -1;
}

int
widget_hide (widget_t *widget)
{
  if (widget && widget->hide)
    if (widget->flags & WIDGET_FLAG_SHOW) /* current state: display */
      return widget->hide (widget);

  return -1;
}

int
widget_set_focus (widget_t *widget, uint32_t state)
{
  if (!widget || !(widget->flags & WIDGET_FLAG_FOCUSABLE))
    return -1;

  if (state)
    widget->flags |= WIDGET_FLAG_FOCUSED;
  else
    widget->flags &= WIDGET_FLAG_FOCUSED;
  
  if (widget->set_focus)
    return widget->set_focus (widget);

  return -1;
}

int
widget_move_focus (widget_t *widget, neighbours_type_t where)
{
  widget_focus_t *focus;
  neighbours_t *nb;
  widget_t *next;
  
  if (!widget || !(widget->flags & WIDGET_FLAG_FOCUSED))
    return -1;

  focus = widget->focus;
  if (!focus)
    return -1;

  nb = focus->neighbours;
  if (!nb)
    return -1;

  switch (where)
  {
  case NEIGHBOURS_UP:
    next = nb->up;
    break;
  case NEIGHBOURS_DOWN:
    next = nb->down;
    break;
  case NEIGHBOURS_LEFT:
    next = nb->left;
    break;
  case NEIGHBOURS_RIGHT:
    next = nb->right;
    break;
  }

  if (!next) /* no neighbour in that direction */
    return -1;

  widget_set_focus (widget, 0); /* unfocus current widget */
  widget_set_focus (next, 1); /* focus new one */

  return 0;
}

int
widget_action (widget_t *widget, action_event_type_t ev)
{
  if (widget && widget->action)
    if (widget->flags & WIDGET_FLAG_FOCUSED) /* widget has focus */
      return widget->action (widget, ev);
  
  return -1;
}

void
widget_free (widget_t *widget)
{
  if (!widget)
    return;

  if (widget->id)
    free (widget->id);

  if (widget->free)
    widget->free (widget);

  free (widget);
}

widget_t *
get_widget_by_id (Evas_List *list, char *id)
{
  Evas_List *l;
  
  if (!list || !id)
    return NULL;

  for (l = list; l; l = l->next)
  {
    widget_t *w;
    
    w = (widget_t *) list->data;
    if (!w || !w->id)
      continue;

    if (!strcmp (w->id, id))
      return w;
  }

  return NULL;
}

neighbours_t *
neighbours_new (void)
{
  neighbours_t *nb = NULL;

  nb = (neighbours_t *) malloc (sizeof (neighbours_t));
  nb->up = NULL;
  nb->down = NULL;
  nb->left = NULL;
  nb->right = NULL;

  return nb;
}

void
neighbours_set (neighbours_t *nb, widget_t *widget, neighbours_type_t type)
{
  if (!nb)
    return;

  switch (type)
  {
  case NEIGHBOURS_UP:
    nb->up = widget;
    break;
  case NEIGHBOURS_DOWN:
    nb->down = widget;
    break;
  case NEIGHBOURS_LEFT:
    nb->left = widget;
    break;
  case NEIGHBOURS_RIGHT:
    nb->right = widget;
    break;
  }
}

void
neighbours_free (neighbours_t *nb)
{
  if (!nb)
    return;

  nb->up = NULL;
  nb->down = NULL;
  nb->left = NULL;
  nb->right = NULL;

  free (nb);
}

widget_focus_t *
widget_focus_new (void)
{
  widget_focus_t *focus = NULL;

  focus = (widget_focus_t *) malloc (sizeof (widget_focus_t));
  focus->neighbours = neighbours_new ();

  return focus;
}

void
widget_focus_free (widget_focus_t *focus)
{
  if (!focus)
    return;

  if (focus->neighbours)
    neighbours_free (focus->neighbours);
  
  free (focus);
}
