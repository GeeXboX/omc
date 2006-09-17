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

cover_t *
cover_new (void)
{
  cover_t *cover = NULL;

  cover = (cover_t *) malloc (sizeof (cover_t));
  cover->border = NULL;
  cover->cover = NULL;

  return cover;
}

void
cover_free (cover_t *cover)
{
  Evas_List *list;
  
  if (!cover)
    return;

  if (cover->border)
  {
    for (list = cover->border; list; list = list->next)
    {
      Evas_Object *obj = NULL;
    
      obj = (Evas_Object *) list->data;
      if (!obj)
        continue;
      
      evas_object_del (obj);
      cover->border = evas_list_remove_list (cover->border, cover->border);
    }
    free (cover->border);
  }
  if (cover->cover)
    evas_object_del (cover->cover);
  free (cover);
}
