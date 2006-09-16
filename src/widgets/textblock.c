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
#include <Evas.h>
#include <Ecore.h>
#include <player.h>

#include "screens/screen.h"
#include "omc.h"
#include "widget.h"

/* Textboxes */
void
text_block_format (Evas_Object *blk, int format)
{
  Evas_Textblock_Cursor *cursor = NULL;

  cursor = (Evas_Textblock_Cursor *) evas_object_textblock_cursor_get (blk);
  if (!cursor)
    return;
  
  switch (format)
    {
    case BLK_NEW_LINE:
      evas_textblock_cursor_format_append (cursor, "\n");
      break;
    case BLK_NEW_TAB:
      evas_textblock_cursor_format_append (cursor, "\t");
      break;
    default:
      return;
    }
}

#define TXT_BLOCK_SIZE 32768

void
text_block_add_text (Evas_Object *blk, char *text)
{
  Evas_Textblock_Cursor *cursor = NULL;
  char str[TXT_BLOCK_SIZE];
  unsigned int i, x = 0;

  if (!text)
    return;

  cursor = (Evas_Textblock_Cursor *) evas_object_textblock_cursor_get (blk);
  if (!cursor)
    return;
  
  bzero (str, TXT_BLOCK_SIZE);
  for (i = 0; i < strlen (text); i++)
  {
    if (text[i] == '\n')
    {
      evas_textblock_cursor_text_append (cursor, str);
      text_block_format (blk, BLK_NEW_LINE);
      bzero (str, TXT_BLOCK_SIZE);
      x = 0;
    }
    else if (text[i] == '\t')
    {
      evas_textblock_cursor_text_append (cursor, str);
      text_block_format (blk, BLK_NEW_TAB);
      bzero (str, TXT_BLOCK_SIZE);
      x = 0;
    }
    else
    {
      str[x] = text[i];
      x++;
    }
  }
  evas_textblock_cursor_text_append (cursor, str);
  evas_textblock_cursor_node_last (cursor);
}

static void
cb_textblock_edit (void *data, Evas *e, Evas_Object *obj, void *event_info)
{
  Evas_Event_Key_Down *event = (Evas_Event_Key_Down *) event_info;
  Evas_Textblock_Cursor *cursor = NULL;
  Evas_Object *cur = (Evas_Object *) data;
  Evas_Coord x = 0, y = 0, cx = 0, cy = 0, cw = 0, ch = 0;
  
  cursor = (Evas_Textblock_Cursor *) evas_object_textblock_cursor_get (obj);
  
  if (!event->keyname)
    return;

  if (!strcmp (event->keyname, "Left"))
    evas_textblock_cursor_char_prev (cursor);
  else if (!strcmp (event->keyname, "Right"))
    evas_textblock_cursor_char_next (cursor);
  else if (!strcmp (event->keyname, "Return"))
    text_block_format (obj, BLK_NEW_LINE);
  else if (!strcmp (event->keyname, "Tab"))
    text_block_format (obj, BLK_NEW_TAB);
  else if (!strcmp (event->keyname, "Delete"))
  {
    if (evas_textblock_cursor_char_next (cursor))
      evas_textblock_cursor_char_delete (cursor);
  }
  else if (!strcmp (event->keyname, "BackSpace"))
    evas_textblock_cursor_char_delete (cursor);
  else
  {
    if (event->string)
    {
      text_block_add_text (obj, (char *) event->string);
      evas_textblock_cursor_char_prev (cursor);
    }
  }

  evas_object_geometry_get (obj, &x, &y, NULL, NULL);
  evas_textblock_cursor_char_geometry_get (cursor, &cx, &cy, &cw, &ch);
  evas_object_move (cur, cx + x + cw, cy + y);
}

static void
cb_textblock_mouse_wheel (void *data, Evas *e,
                          Evas_Object *obj, void *event_info)
{

}

Evas_Object *
text_block_new (omc_t *omc, int editable, char *x, char *y,
                char *w, char *h, int layer, font_t *font,
                char *align_h, char *align_v)
{
  char style[1024];
  Evas_Object *blk = NULL;
  Evas_Textblock_Style *st;
  color_t *cl = NULL;
  Evas_Coord x2, y2, w2, h2;

  x2 = omc_compute_coord (x, omc->w);
  y2 = omc_compute_coord (y, omc->h);
  w2 = omc_compute_coord (w, omc->w);
  h2 = omc_compute_coord (h, omc->h);
  cl = color_new (font->color, 255);
  
  blk = evas_object_textblock_add (omc->evas);
  evas_object_move (blk, x2, y2);
  evas_object_resize (blk, w2, h2);
  evas_object_layer_set (blk, layer);
  evas_object_focus_set (blk, 1);

  evas_object_textblock_cursor_new (blk);

  sprintf (style, "DEFAULT='font=%s font_size=%d color=%sFF wrap=word align=%s valign=%s'", font->ft, font->size, font->color, align_h, align_v);
  
  st = evas_textblock_style_new ();
  evas_textblock_style_set (st, style);
  evas_object_textblock_style_set (blk, st);
  evas_textblock_style_free (st);
  evas_object_show (blk);

  if (editable)
  {
    Evas_Textblock_Cursor *cursor = NULL;
    Evas_Object *cur = NULL;
    Evas_Coord cx = 0, cy = 0, cw = 0, ch = 0;

    cursor = (Evas_Textblock_Cursor *) evas_object_textblock_cursor_get (blk);
    evas_textblock_cursor_node_last (cursor);
    evas_textblock_cursor_char_geometry_get (cursor, &cx, &cy, &cw, &ch);
  
    cur = evas_object_rectangle_add (omc->evas);
    evas_object_move (cur, cx + x2 + cw, cy + y2);
    evas_object_resize (cur, 2, ch);
    evas_object_color_set (cur, cl->r, cl->g, cl->b, cl->a);
    evas_object_layer_set (cur, 0);
    evas_object_show (cur);

    evas_object_event_callback_add (blk, EVAS_CALLBACK_KEY_DOWN,
                                    cb_textblock_edit, cur);
  }

  evas_object_event_callback_add (blk, EVAS_CALLBACK_MOUSE_WHEEL,
                                  cb_textblock_mouse_wheel, NULL);

  return blk;
}
