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

/* Current Working Directory (CWD) items */
cwd_t *
cwd_new (void)
{
  cwd_t *cwd = NULL;

  cwd = (cwd_t *) malloc (sizeof (cwd_t));
  cwd->border = NULL;
  cwd->path = NULL;

  return cwd;
}

void
cwd_free (cwd_t *cwd)
{
  Evas_List *list;
  
  if (!cwd)
    return;

  if (cwd->border)
  {
    for (list = cwd->border; list; list = list->next)
    {
      Evas_Object *obj = NULL;
    
      obj = (Evas_Object *) list->data;
      if (!obj)
        continue;
      
      evas_object_del (obj);
      cwd->border = evas_list_remove_list (cwd->border, cwd->border);
    }
    free (cwd->border);
  }
  if (cwd->path)
    evas_object_del (cwd->path);
  free (cwd);
}
