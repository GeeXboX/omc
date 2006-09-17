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
#include "omc.h"

item_t *
item_new (browser_t *browser, Evas_Object *icon, Evas_Object *text,
          item_type_t type, char *mrl, int mrl_type)
{
  item_t *item = NULL;

  item = (item_t *) malloc (sizeof (item_t));
  item->browser = browser; /* browser the item belongs to */
  item->icon = icon;
  item->text = text;
  item->clip = NULL;
  item->type = type;
  item->mrl = mrl ? mrl_new (mrl, mrl_type) : NULL;
  item->infos = NULL;
  item->cover = NULL;
  item->updated = 0;
  
  return item;
}

void
item_free (item_t *item)
{
  if (!item)
    return;

  if (item->icon)
    evas_object_del (item->icon);
  if (item->text)
    evas_object_del (item->text);
  if (item->clip)
    evas_object_del (item->clip);
  if (item->mrl)
    mrl_free (item->mrl, 0);
  if (item->infos)
    free (item->infos);
  if (item->cover)
    free (item->cover);
  
  free (item);
}
