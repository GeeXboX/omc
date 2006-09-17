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
#include "gfx.h"

float zoom = 1.0;

void
image_flip_horizontal (Evas_Object *img)
{
  unsigned int *p1, *p2, tmp;
  unsigned int *data;
  int x, y, w, h;
   
  if (!img)
    return;
   
  evas_object_image_size_get (img, &w, &h);
  data = evas_object_image_data_get (img, 1);
   
  for (y = 0; y < h; y++)
  {
    p1 = data + (y * w);
    p2 = data + ((y + 1) * w) - 1;
    for (x = 0; x < (w >> 1); x++)
    {
      tmp = *p1;
      *p1 = *p2;
      *p2 = tmp;
      p1++;
      p2--;
    }
  }
   
  evas_object_image_data_set (img, data);
  evas_object_image_data_update_add (img, 0, 0, w, h);
}

void
image_flip_vertical (Evas_Object *img)
{
  unsigned int *p1, *p2, tmp;
  unsigned int *data;
  int x, y, w, h;
   
  if (!img)
    return;
   
  evas_object_image_size_get (img, &w, &h);
  data = evas_object_image_data_get (img, 1);
   
  for (y = 0; y < (h >> 1); y++)
  {
    p1 = data + (y * w);
    p2 = data + ((h - 1 - y) * w);
    for (x = 0; x < w; x++)
    {
      tmp = *p1;
      *p1 = *p2;
      *p2 = tmp;
      p1++;
      p2++;
    }
  }
      
  evas_object_image_data_set (img, data);
  evas_object_image_data_update_add (img, 0, 0, w, h);   
}

void
image_fit_to_screen (Evas_Object *img)
{
  Evas_Coord cw, ch;
  
  if (!img)
    return;
  
  evas_object_geometry_get (img, NULL, NULL, &cw, &ch);

  if (cw > omc->w || ch > omc->h) /* overflow */
  {
    float ratio;

    if (cw >= ch)
    {
      ratio = (float) ((float) cw / (float) ch);
      cw = omc->w;
      ch = (int) (cw / ratio);
    }
    else
    {
      ratio = (float) ((float) ch / (float) cw);
      ch = omc->h;
      cw = (int) (ch / ratio);
    }

    evas_object_resize (img, cw, ch);
    evas_object_image_fill_set (img, 0, 0, cw, ch);
  }
}

void
image_center (Evas_Object *img)
{
  Evas_Coord cw, ch;
  
  if (!img)
    return;
  
  evas_object_geometry_get (img, NULL, NULL, &cw, &ch);
  
  if (cw > omc->w && ch > omc->h)
    evas_object_move (img, 0, 0);
  else
  {
    Evas_Coord dx = 0, dy = 0;
    dx = (int) ((omc->w - cw) / 2);
    dy = (int) ((omc->h - ch) / 2);
    
    evas_object_move (img, dx, dy);
  }
}

void
image_rotate (Evas_Object *img, int direction)
{
  unsigned int *data, *data2, *to, *from;
  int x, y, w, hw, iw, ih, cw, ch;
     
  if (!img)
    return;
   
  evas_object_image_size_get (img, &iw, &ih);
  data2 = evas_object_image_data_get (img, 0);
  evas_object_geometry_get (img, NULL, NULL, &cw, &ch);
      
  data = malloc (iw * ih * sizeof (unsigned int));
  from = data2;
  w = ih;
  ih = iw;
  iw = w;
  hw = w * ih;
  
  switch (direction)
  {
  default:
  case ROTATE_CLOCKWISE:
    to = data + w - 1;
    hw = -hw - 1;
    break;
  case ROTATE_COUNTERCLOCKWISE:
    to = data + hw - w;
    w = -w;
    hw = hw + 1;
    break;
  }
  
  for (x = iw; --x >= 0;)
  {
    for (y = ih; --y >= 0;)
    {
      *to = *from;
      from++;
      to += w;
    }
    to += hw;
  }
   
  evas_object_image_size_set (img, iw, ih);   
  evas_object_image_data_set (img, data);
  evas_object_image_data_update_add (img, 0, 0, iw, ih);
  evas_object_resize (img, ch, cw);
  evas_object_image_fill_set (img, 0, 0, ch, cw);
  image_center (img);
}

void
image_blur (Evas_Object *img)
{
  unsigned int *data, *data2;
  int x, y, w, h, mx, my, mw, mh, mt, xx, yy;
  int a, r, g, b;
  int *as, *rs, *gs, *bs;   
  int rad = 2; /* maybe make this configurable later */
  unsigned int *p1, *p2;
   
  if (!img)
    return;
   
  evas_object_image_size_get (img, &w, &h);
  data2 = evas_object_image_data_get (img, 1);
   
  data = malloc (w * h * sizeof (unsigned int));
  as = malloc (sizeof (int) * w);
  rs = malloc (sizeof (int) * w);
  gs = malloc (sizeof (int) * w);
  bs = malloc (sizeof (int) * w);
   
  for (y = 0; y < h; y++)
  {
    my = y - rad;
    mh = (rad << 1) + 1;
    if (my < 0)
    {
      mh += my;
      my = 0;
    }
    if ((my + mh) > h)
      mh = h - my;
	
    p1 = data + (y * w);
    memset(as, 0, w * sizeof (int));
    memset(rs, 0, w * sizeof (int));
    memset(gs, 0, w * sizeof (int));
    memset(bs, 0, w * sizeof (int));
	
    for (yy = 0; yy < mh; yy++)
    {
      p2 = data2 + ((yy + my) * w);
      for (x = 0; x < w; x++)
      {
        as[x] += (*p2 >> 24) & 0xff;
        rs[x] += (*p2 >> 16) & 0xff;
        gs[x] += (*p2 >> 8) & 0xff;
        bs[x] += *p2 & 0xff;
        p2++;
      }
    }
    
    if (w > ((rad << 1) + 1))
    {
      for (x = 0; x < w; x++)
      {
        a = 0;
        r = 0;
        g = 0;
        b = 0;
        mx = x - rad;
        mw = (rad << 1) + 1;
        
        if (mx < 0)
        {
          mw += mx;
          mx = 0;
        }

        if ((mx + mw) > w)
          mw = w - mx;
        mt = mw * mh;

        for (xx = mx; xx < (mw + mx); xx++)
        {
          a += as[xx];
          r += rs[xx];
          g += gs[xx];
          b += bs[xx];
        }

        a = a / mt;
        r = r / mt;
        g = g / mt;
        b = b / mt;
        *p1 = (a << 24) | (r << 16) | (g << 8) | b;
        p1++;
      }
    }
  }
   
  free (as);
  free (rs);
  free (gs);
  free (bs);
   
  evas_object_image_data_set (img, data);
  evas_object_image_data_update_add (img, 0, 0, w, h);   
}

void
image_sharpen (Evas_Object *img)
{
  unsigned int *data, *data2;
  unsigned int *p1, *p2;
  int mul, mul2, tot;
  int x, y, w, h;
  int a, r, g, b;
  int rad = 2; /* maybe make this configurable later */

  if (!img)
    return;
  
  evas_object_image_size_get (img, &w, &h);
  data2 = evas_object_image_data_get (img, 1);
      
  data = malloc (w * h * sizeof (unsigned int));
 
  mul = (rad * 4) + 1;
  mul2 = rad;
  tot = mul - (mul2 * 4);

  for (y = 1; y < (h - 1); y++)
  {
    p1 = data2 + 1 + (y * w);
    p2 = data + 1 + (y * w);
    for (x = 1; x < (w - 1); x++)
    {
      b = (int) ((p1[0]) & 0xff) * 5;
      g = (int) ((p1[0] >> 8) & 0xff) * 5;
      r = (int) ((p1[0] >> 16) & 0xff) * 5;
      a = (int) ((p1[0] >> 24) & 0xff) * 5;
      
      b -= (int) ((p1[-1]) & 0xff);
      g -= (int) ((p1[-1] >> 8) & 0xff);
      r -= (int) ((p1[-1] >> 16) & 0xff);
      a -= (int) ((p1[-1] >> 24) & 0xff);

      b -= (int) ((p1[1]) & 0xff);
      g -= (int) ((p1[1] >> 8) & 0xff);
      r -= (int) ((p1[1] >> 16) & 0xff);
      a -= (int) ((p1[1] >> 24) & 0xff);

      b -= (int) ((p1[-w]) & 0xff);
      g -= (int) ((p1[-w] >> 8) & 0xff);
      r -= (int) ((p1[-w] >> 16) & 0xff);
      a -= (int) ((p1[-w] >> 24) & 0xff);

      b -= (int) ((p1[w]) & 0xff);
      g -= (int) ((p1[w] >> 8) & 0xff);
      r -= (int) ((p1[w] >> 16) & 0xff);
      a -= (int) ((p1[w] >> 24) & 0xff);
		  
      a = (a & ((~a) >> 16));
      a = ((a | ((a & 256) - ((a & 256) >> 8))));
      
      r = (r & ((~r) >> 16));
      r = ((r | ((r & 256) - ((r & 256) >> 8))));
      
      g = (g & ((~g) >> 16));
      g = ((g | ((g & 256) - ((g & 256) >> 8))));

      b = (b & ((~b) >> 16));
      b = ((b | ((b & 256) - ((b & 256) >> 8))));
		  
      *p2 = (a << 24) | (r << 16) | (g << 8) | b;
      p2++;
      p1++;
    }
  }
   
  evas_object_image_data_set (img, data);
  evas_object_image_data_update_add (img, 0, 0, w, h);      
}

static void
image_zoom (Evas_Object *img, double factor)
{
  int w, h;
   
  if (!img)
    return;

  evas_object_image_size_get (img, &w, &h);
  printf ("Before Zoom %d-%d\n", w, h);

  if (factor > 0.0)
  {
    w *= factor;
    h *= factor;
  }
  else if (factor < 0.0)
  {
    w /= abs ((int) factor);
    h /= abs ((int) factor);
  }

  printf ("After Zoom %d-%d\n", w, h);
  evas_object_resize (img, w, h);
  evas_object_image_fill_set (img, 0, 0, w, h);
  image_center (img);
}

static void
zoom_level (void)
{
  printf ("Current Zoom Level : %d%%\n", (int) (zoom * 100));
}

void
image_zoom_in (Evas_Object *img)
{
  if (zoom >= MAX_ZOOM_LEVEL) /* going upper is probably too much */
    return;

  zoom *= ZOOM_STEP;
  image_zoom (img, zoom);
  zoom_level ();
}

void
image_zoom_out (Evas_Object *img)
{
  if (zoom <= MIN_ZOOM_LEVEL) /* going below is probably non-visible */
    return;
  
  zoom /= ZOOM_STEP;
  image_zoom (img, zoom);
  zoom_level ();
}

void
image_zoom_1_1 (Evas_Object *img)
{
  zoom = 1.0;
  image_zoom (img, zoom);
  zoom_level ();
}

void
image_move (Evas_Object *img, int x, int y)
{
  int cx, cy, cw, ch;
   
  if (!img)
    return;

  evas_object_geometry_get (img, &cx, &cy, &cw, &ch);
  
  if (cw <= omc->w || ch <= omc->h)
    return;
  
  cx -= x;
  if (cx + cw < omc->w)
    cx += omc->w - (cx + cw);

  if (cx > 0)
    return;

  cy -= y;
  if (cy + ch < omc->h)
    cy += omc->h - (cy + ch);

  if (cy > 0)
    return;
  
  evas_object_move (img, cx, cy);
}
