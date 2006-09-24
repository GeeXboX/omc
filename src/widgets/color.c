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
#include "color.h"

color_t *
color_new (char *color, int alpha)
{
  color_t *cl = NULL;

  cl = (color_t *) malloc (sizeof (color_t));
  cl->r = cl->g = cl->b = 0;
  cl->a = alpha;

  if (cl->a < 0)
    cl->a = 0;
  if (cl->a > 255)
    cl->a = 255;
  
  if (color && color[0] == '#' && strlen (color) == 7)
    {
      char *end;
      int val = strtol (color + 1, &end, 16);
      if (end > color + 1 && *end == '\0')
      {
        cl->r = (val & 0xFF0000) >> 16;
        cl->g = (val & 0x00FF00) >> 8;
        cl->b = (val & 0x0000FF);
      }
    }

  return cl;
}

void
color_free (color_t *color)
{
  if (color)
    free (color);
}
