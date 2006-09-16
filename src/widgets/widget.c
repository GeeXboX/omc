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
#include <Evas.h>
#include <Ecore.h>

#include <dirent.h>
#include <libgen.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <player.h>

#include "screens/screen.h"
#include "omc.h"
#include "widget.h"
#include "filter.h"
#include "avplayer.h"
#include "av_demuxer.h"
#include "infos.h"
#include "amazon.h"
#include "screens/screen_audio.h"
#include "screens/screen_video.h"
#include "screens/screen_viewer.h"

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

/* Browser */
static item_t *
item_new (browser_t *browser, Evas_Object *icon, Evas_Object *text,
          int type, char *mrl, int mrl_type)
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
  item->artist = NULL;
  item->album = NULL;
  item->cover = NULL;
  item->updated = 0;
  
  return item;
}

static void
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
  if (item->artist)
    free (item->artist);
  if (item->album)
    free (item->album);
  if (item->cover)
    free (item->cover);
  
  free (item);
}

#define BROWSER_THUMBNAIL_MAX_SIZE_W 164
#define BROWSER_THUMBNAIL_MAX_SIZE_H 110
#define BROWSER_THUMBNAIL_PADDING_W 20
#define BROWSER_THUMBNAIL_PADDING_H 25
#define BROWSER_TEXT_PADDING_H 10

static void
browser_compute (browser_t *browser)
{
  Evas_Coord x, y, w, h;
  Evas_Object *obj;
  int txt_size = 0, icon_size = 0;

  if (!browser->entries)
    return;

  if (browser->filter_type == FILTER_TYPE_IMAGE)
  {
    evas_object_geometry_get (browser->clip, &x, &y, &w, &h);

    obj = ((item_t *) browser->entries[0].data)->text;
    if (obj)
      evas_object_geometry_get (obj, NULL, NULL, NULL, &txt_size);

    browser->pos = 0;
    browser->capacity_w =
      (int) (w / (BROWSER_THUMBNAIL_MAX_SIZE_W
                  + BROWSER_THUMBNAIL_PADDING_W));
    browser->capacity_h =
      (int) (h / (BROWSER_THUMBNAIL_MAX_SIZE_H
                  + txt_size + BROWSER_THUMBNAIL_PADDING_H));

    return;
  }
  
  obj = ((item_t *) browser->entries[0].data)->text;
  if (obj)
    evas_object_geometry_get (obj, NULL, NULL, NULL, &txt_size);
  
  obj = ((item_t *) browser->entries[0].data)->icon;
  if (obj)
    evas_object_geometry_get (obj, NULL, NULL, NULL, &icon_size);
  
  browser->pos = 0;
  browser->capacity_w = 1;
  h = (txt_size > icon_size) ? txt_size : icon_size;
  browser->capacity_h = (int) (browser->h / (h + BROWSER_TEXT_PADDING_H)) - 1;
}

static void
browser_hide (browser_t *browser)
{
  Evas_List *l;
  for (l = browser->entries; l; l = l->next)
  {
    Evas_Object *obj = NULL;

    obj = ((item_t *) l->data)->text;
    if (obj)
      evas_object_hide (obj);
    obj = ((item_t *) l->data)->clip;
    if (obj)
      evas_object_hide (obj);
    obj = ((item_t *) l->data)->icon;
    if (obj)
      evas_object_hide (obj);
  }
}

static void
browser_display_update (browser_t *browser)
{
  Evas_List *l;
  Evas_Coord x, y;
  int count, start, end;

  if (!browser->entries)
    return;
  
  x = browser->x + BROWSER_THUMBNAIL_PADDING_W;
  y = browser->y;
  count = 0;
  
  /* hide previous browser item list */
  browser_hide (browser);

  /* determine which items should be displayed */
  if (browser->pos < browser->capacity_h)
  {
    start = 0;
    end = browser->capacity_h + 1;
  }
  else
  {
    start = browser->pos - browser->capacity_h;
    end = browser->pos + 1;
  }

  if (browser->filter_type == FILTER_TYPE_IMAGE)
  {
    int capacity_w = 0;
    for (l = evas_list_nth_list (browser->entries, browser->pos);
         l && count++ <= (int)(browser->capacity_w * browser->capacity_h) - 1;
         l = l->next)
    {
      item_t *item = NULL;
      Evas_Object *icon, *text;
      int txt_size = 0;
      
      item = (item_t *) l->data;
      if (!item)
        continue;

      icon = item->icon;
      if (icon)
      {
        Evas_Coord cx, cy, cw, ch;

        /* the image thumbnail hasn't been calculated yet */
        if (!item->updated && item->type == ITEM_TYPE_FILE)
        {
          evas_object_image_file_set (icon, item->mrl->name, NULL);
          evas_object_image_size_get (icon, &cw, &ch);

          /* do not consider small files (usually amazon retrieved dummy
             covers or failed FreeDesktop thumbnail files */
          if (cw <= 1 || ch <= 1)
            continue;
          
          if (cw == ch &&
              ((cw > BROWSER_THUMBNAIL_MAX_SIZE_W)
               || (ch > BROWSER_THUMBNAIL_MAX_SIZE_H)))
            cw = ch =
              (BROWSER_THUMBNAIL_MAX_SIZE_W > BROWSER_THUMBNAIL_MAX_SIZE_H) ?
              BROWSER_THUMBNAIL_MAX_SIZE_H : BROWSER_THUMBNAIL_MAX_SIZE_W;
          
          if (cw > 0 && cw > ch && cw > BROWSER_THUMBNAIL_MAX_SIZE_W)
          {
            int hmax = (int) (ch * BROWSER_THUMBNAIL_MAX_SIZE_W / cw);

            if (hmax < BROWSER_THUMBNAIL_MAX_SIZE_H)
            {
              ch = hmax;
              cw = BROWSER_THUMBNAIL_MAX_SIZE_W;
            }
            else
            {
              cw = (int) (cw * BROWSER_THUMBNAIL_MAX_SIZE_H / ch);
              ch = BROWSER_THUMBNAIL_MAX_SIZE_H;
            }
          }

          if (ch > 0 && ch > cw && ch > BROWSER_THUMBNAIL_MAX_SIZE_H)
          {
            int wmax = (int) (cw * BROWSER_THUMBNAIL_MAX_SIZE_H / ch);

            if (wmax < BROWSER_THUMBNAIL_MAX_SIZE_W)
            {
              cw = wmax;
              ch = BROWSER_THUMBNAIL_MAX_SIZE_H;
            }
            else
            {
              ch = (int) (ch * BROWSER_THUMBNAIL_MAX_SIZE_W / cw);
              cw = BROWSER_THUMBNAIL_MAX_SIZE_W;
            }
          }
  
          evas_object_resize (icon, cw, ch);
          evas_object_image_fill_set (icon, 0, 0, cw, ch);
          
          item->updated = 1;
        }

        evas_object_geometry_get (icon, &cx, &cy, &cw, &ch);

        cx = x;      
        if (cw > BROWSER_THUMBNAIL_MAX_SIZE_W)
          cw = BROWSER_THUMBNAIL_MAX_SIZE_W;
        else
          cx += (int) ((BROWSER_THUMBNAIL_MAX_SIZE_W - cw) / 2);

        cy = y;
        if (ch > BROWSER_THUMBNAIL_MAX_SIZE_H)
          ch = BROWSER_THUMBNAIL_MAX_SIZE_H;
        else
          cy += (int) ((BROWSER_THUMBNAIL_MAX_SIZE_H - ch) / 2);

        evas_object_move (icon, cx, cy);
        evas_object_show (icon);

        text = item->text;
        if (text)
        {
          evas_object_geometry_get (text, NULL, NULL, &cw, &ch);
          txt_size = ch;
          
          cx = x;      
          if (cw > BROWSER_THUMBNAIL_MAX_SIZE_W)
            cw = BROWSER_THUMBNAIL_MAX_SIZE_W;
          else
            cx += (int) ((BROWSER_THUMBNAIL_MAX_SIZE_W - cw) / 2);

          cy = y + txt_size + BROWSER_THUMBNAIL_MAX_SIZE_H;

          if (cw >= BROWSER_THUMBNAIL_MAX_SIZE_W)
          {
            char x2[8], y2[8], w2[8], h2[8];

            sprintf (x2, "%d", cx);
            sprintf (y2, "%d", cy);
            sprintf (w2, "%d", cw);
            sprintf (h2, "%d", ch);
            item->clip = object_clipper (omc, x2, y2, w2, h2);
          }
                  
          evas_object_move (text, cx, cy);
          evas_object_color_set (text, 255, 255, 255, 255);

          if (item->clip)
          {
            evas_object_clip_set (text, item->clip);
            evas_object_show (item->clip);
          }
          
          evas_object_show (text);
        }
      }
    
      capacity_w++;
      x += (int) (browser->w / browser->capacity_w);
      if (capacity_w == browser->capacity_w)
      {
        x = browser->x + BROWSER_THUMBNAIL_PADDING_W;
        y += BROWSER_THUMBNAIL_MAX_SIZE_H
          + txt_size + BROWSER_THUMBNAIL_PADDING_H;
        capacity_w = 0;
      }
    }

    return;
  }
  
  /* display items */
  x = browser->x;
  for (l = evas_list_nth_list (browser->entries, browser->pos);
       l && count++ <= browser->capacity_h; l = l->next)
  {
    item_t *item = NULL;
    Evas_Object *icon, *text;
    Evas_Coord ih = 0, th = 0, shift = 0, size = 0;

    item = (item_t *) l->data;
    if (!item)
      continue;
    
    icon = item->icon;
    if (icon)
    {
      evas_object_geometry_get (icon, NULL, NULL, &shift, &ih);
      if (shift)
        shift += 3;
      evas_object_move (icon, x, y);
      evas_object_show (icon);
    }

    text = item->text;
    if (text)
    {
      evas_object_geometry_get (text, NULL, NULL, NULL, &th);
      evas_object_move (text, x + shift, y);
      evas_object_color_set (text, 255, 255, 255, 255);
      evas_object_show (text);
    }

    size = (ih >= th) ? ih : th;
    y += size + BROWSER_TEXT_PADDING_H;    
  }
}

#define MAX_PATH_LEN 1024

static void
compute_directory (const char *dir, char *mrl)
{
  char newdir[MAX_PATH_LEN];
    
  if (!dir || !mrl)
    return;

  if (!strcmp (dir, ".."))
    sprintf (newdir, dirname (omc->cwd));
  else
    sprintf (newdir, mrl);

  omc_update_cwd (omc, newdir);
}

static void browser_update (omc_t *omc, browser_t *browser);

enum {
  MOUSE_BUTTON_LEFT = 1,
  MOUSE_BUTTON_WHEEL = 2,
  MOUSE_BUTTON_RIGHT = 3
};

static void
cb_browser_entry_execute (void *data, Evas *e,
                          Evas_Object *obj, void *event_info)
{
  item_t *item = NULL;
  const char *dir = NULL;
  Evas_Event_Mouse_Down *event = (Evas_Event_Mouse_Down *) event_info;

  item = (item_t *) data;
  if (!item)
    return;

  dir = evas_object_text_text_get (item->text);
  
  if (item->type == ITEM_TYPE_DIRECTORY)
  {
    compute_directory (dir, item->mrl->name);
    browser_update (omc, item->browser);
  }
  else if (item->type == ITEM_TYPE_FILE)
  {
    switch (item->mrl->type)
    {
    case PLAYER_MRL_TYPE_AUDIO:
      switch (event->button)
      {
      case MOUSE_BUTTON_LEFT:
        printf ("Play File : %s\n", item->mrl->name);
        av_player_add_mrl (omc->player, item, PLAYER_ADD_MRL_NOW);
        break;
      case MOUSE_BUTTON_RIGHT:
        printf ("Append File to playlist : %s\n", item->mrl->name);
        av_player_add_mrl (omc->player, item, PLAYER_ADD_MRL_QUEUE);
        break;
      default:
        break;
      }
      break;
    case PLAYER_MRL_TYPE_IMAGE:
      printf ("To be viewed : %s\n", item->mrl->name);
      switch_screen (SCREEN_VIEWER_TITLE, strdup (item->mrl->name));
      break;
    default:
      break;
    }
  }
 
}

static void
cb_browser_mrl_execute (void *data, Evas *e,
                        Evas_Object *obj, void *event_info)
{
  item_t *item = NULL;
  Evas_Event_Mouse_Down *event = (Evas_Event_Mouse_Down *) event_info;

  item = (item_t *) data;
  if (!item)
    return;
  
  switch (event->button)
  {
  case MOUSE_BUTTON_LEFT:
  {
    av_mrl_t *mrl = NULL;
    Evas_List *list;
    
    printf ("Need to find MRL for %s\n", item->mrl->name);
    if (!omc->player || !omc->player->playlist)
      break;

    for (list = omc->player->playlist; list; list = list->next)
    {
      av_mrl_t *tmp = NULL;

      tmp = (av_mrl_t *) list->data;
      if (!tmp)
        continue;
      
      if (!strcmp (item->mrl->name, tmp->file))
      {
        mrl = tmp;
        break;
      }
    }

    if (mrl)
    {
      printf ("Found the corresponding MRL in playlist\n");
      omc->player->current = mrl;
      av_player_start (omc->player);
    }
    
    break;
  }
  default:
    break;
  }
}

static void
cb_browser_get_file_info (void *data, Evas *e,
                          Evas_Object *obj, void *event_info)
{
  item_t *item = NULL;

  item = (item_t *) data;
  if (!item)
    return;

  av_demux_mrl (item);
  
  switch (omc->screen->type)
  {
  case SCREEN_TYPE_AUDIO:
    if (item->infos)
      screen_audio_update_infos (omc->screen, item->infos, 1);
    if (item->cover)
      screen_audio_update_cover (omc->screen, item->cover, 1);
    break;
  case SCREEN_TYPE_VIDEO:
    if (item->infos)
      screen_video_update_infos (omc->screen, item->infos, 1);
    if (item->cover)
      screen_video_update_cover (omc->screen, item->cover, 1);
    break;
  default:
    break;
  }
}

static void
cb_browser_hide_file_info (void *data, Evas *e,
                           Evas_Object *obj, void *event_info)
{
  switch (omc->screen->type)
  {
  case SCREEN_TYPE_AUDIO:
    screen_audio_update_infos (omc->screen, NULL, 0);
    screen_audio_update_cover (omc->screen, NULL, 0);
    break;
  case SCREEN_TYPE_VIDEO:
    screen_video_update_infos (omc->screen, NULL, 0);
    screen_video_update_cover (omc->screen, NULL, 0);
    break;
  default:
    break;
  }
}

static void
browser_prev_item (browser_t *browser)
{
  Evas_List *l;

  l = evas_list_nth_list (browser->entries, browser->pos);
  if (l && l->prev)
  {
    browser->pos--;
    browser_display_update (browser);
  }
}

static void
browser_next_item (browser_t *browser)
{
  Evas_List *l;

  l = evas_list_nth_list (browser->entries, browser->pos);
  if (l && l->next
      && (browser->pos + browser->capacity_h
          < evas_list_count (browser->entries) - 1))
  {
    browser->pos++;
    browser_display_update (browser);
  }
}

static void
cb_browser_mouse_wheel (void *data, Evas *e,
                        Evas_Object *obj, void *event_info)
{
  Evas_Event_Mouse_Wheel *event = event_info;
  browser_t *browser = NULL;
  int i;
  
  browser = (browser_t *) data;
  if (!browser)
    return;
  
  if (event->z < 0)
    for (i = 0; i < browser->capacity_w; i++)
      browser_prev_item (browser);
  else if (event->z > 0)
    for (i = 0; i < browser->capacity_w; i++)
      browser_next_item (browser);
}

#define IMG_ICON_FOLDER OMC_DATA_DIR"/folder.png"
#define IMG_ICON_FILE_AUDIO OMC_DATA_DIR"/music.png"
#define IMG_ICON_FILE_MOVIE OMC_DATA_DIR"/film.png"
#define IMG_ICON_FILE_IMAGE OMC_DATA_DIR"/picture.png"

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

static void
browser_update (omc_t *omc, browser_t *browser)
{
  struct dirent **namelist;
  filter_t *filter = NULL;
  char *cwd_cover = NULL;
  int n, i;

  if (!browser)
    return;
  
  /* remove entries */
  if (browser->entries)
  {
    Evas_List *list;
    for (list = browser->entries; list; list = list->next)
    {
      item_t *item = NULL;
    
      item = (item_t *) list->data;
      if (!item)
        continue;
      
      item_free (item);
      browser->entries = evas_list_remove_list (browser->entries,
                                                browser->entries);
    }
    browser->entries = NULL;
  }

  if (browser->filter_type == FILTER_TYPE_MRL)
  {
    if (omc->player)
    {
      Evas_List *list;

      for (list = omc->player->playlist; list; list = list->next)
      {
        av_mrl_t *mrl;
        item_t *item = NULL;
        Evas_Object *txt = NULL;

        mrl = (av_mrl_t *) list->data;
        if (!mrl)
          continue;
        
        printf ("MRL : %s\n", mrl->file);
       
        txt = text_new (omc, 1, browser->font,
                        basename (mrl->file), 255, 5, "0", "0");
        object_add_default_cb (txt);
      
        if (browser->clip)
          evas_object_clip_set (txt, browser->clip);
       
        item = item_new (browser, NULL, txt,
                         ITEM_TYPE_FILE, mrl->file, mrl->type);
        item->infos = mrl->infos ? strdup (mrl->infos) : NULL;
        item->cover = mrl->cover ? strdup (mrl->cover) : NULL;
        
        evas_object_event_callback_add (txt, EVAS_CALLBACK_MOUSE_DOWN,
                                        cb_browser_mrl_execute, item);
        evas_object_event_callback_add (txt, EVAS_CALLBACK_MOUSE_WHEEL,
                                        cb_browser_mouse_wheel, browser);

        browser->entries = evas_list_append (browser->entries, item);
      }

      if (!browser->entries || evas_list_count (browser->entries) == 0)
      {
        /* no playlist right now */
        item_t *item = NULL;
        Evas_Object *txt = NULL;

        txt = text_new (omc, 0, browser->font,
                        "Empty Playlist ...", 255, 5, "0", "0");
       
        if (browser->clip)
          evas_object_clip_set (txt, browser->clip);
       
        item = item_new (browser, NULL, txt, ITEM_TYPE_FILE,
                         "empty", PLAYER_MRL_TYPE_NONE);
        browser->entries = evas_list_append (browser->entries, item);
      }
    }
  }
  else
  {
    filter = filter_get (omc->filters, browser->filter_type);
    if (!filter)
      return;

    n = scandir (omc->cwd, &namelist, 0, alphasort);
    if (n < 0)
    {
      perror ("scandir");
      return;
    }
  
    for (i = 0; i < n; i++)
    {
      item_t *item = NULL;
      Evas_Object *txt = NULL;
      Evas_Object *icon = NULL;
      struct stat st;
      char *path = NULL;
      int type = -1;
      size_t len;
      int mrl_type = PLAYER_MRL_TYPE_NONE;
      
      /* do not consider hidden files */
      if ((namelist[i]->d_name[0] == '.' && namelist[i]->d_name[1] != '.'))
      {
        free (namelist[i]);
        continue;
      }
      
      if (!strcmp (namelist[i]->d_name, "..") && !strcmp (omc->cwd, "/"))
        continue;
      
      len = strlen (omc->cwd) + 1 + strlen (namelist[i]->d_name) + 1;
      path = (char *) malloc (len);
      if (!strcmp (omc->cwd, "/"))
        sprintf (path, "/%s", namelist[i]->d_name);
      else
        sprintf (path, "%s/%s", omc->cwd, namelist[i]->d_name);
      
      stat (path, &st);
      if (S_ISDIR (st.st_mode))
      {
        type = ITEM_TYPE_DIRECTORY;
        if (browser->filter_type == FILTER_TYPE_IMAGE)
          icon = image_new (omc, 0, IMG_ICON_FOLDER,
                            NULL, 0, "0", "0", "80", "55");
        else
          icon = image_new (omc, 0, IMG_ICON_FOLDER,
                            NULL, 0, "0", "0", "36", "25");
      }
      else if (S_ISREG (st.st_mode))
      {
        char *ext = NULL;
        
        ext = getExtension (namelist[i]->d_name);
        if (!ext)
          continue;
        
        if (!filter_supports_extension (filter, ext))
        {
          free (ext);
          continue;
        }
        
        type = ITEM_TYPE_FILE;
        switch (browser->filter_type)
        {
        case FILTER_TYPE_AUDIO:
          mrl_type = PLAYER_MRL_TYPE_AUDIO;
          icon = image_new (omc, 0, IMG_ICON_FILE_AUDIO,
                            NULL, 0, "0", "0", "21", "25");
          break;
        case FILTER_TYPE_VIDEO:
          mrl_type = PLAYER_MRL_TYPE_VIDEO;
          icon = image_new (omc, 0, IMG_ICON_FILE_MOVIE,
                            NULL, 0, "0", "0", "21", "25");
          break;
        case FILTER_TYPE_IMAGE:
          mrl_type = PLAYER_MRL_TYPE_IMAGE;
          icon = image_new (omc, 0, IMG_ICON_FILE_IMAGE,
                            NULL, 0, "0", "0", "51", "55");
          break;
        }
        
        free (ext);
      }

      txt = text_new (omc, 1, browser->font,
                      namelist[i]->d_name, 255, 5, "0", "0");
      object_add_default_cb (txt);
      
      if (browser->clip)
        evas_object_clip_set (txt, browser->clip);
      
      item = item_new (browser, icon, txt, type, path, mrl_type);
      
      if (item->type == ITEM_TYPE_FILE && mrl_type != PLAYER_MRL_TYPE_IMAGE)
      {
        if (omc->cfg->show_infos)
          grab_file_infos (item);
        
        if (omc->cfg->show_cover)
        {
          if (cwd_cover)
            item->cover = strdup (cwd_cover);
          else
            grab_file_covers (item);
        }
        
        if (item->cover && item->type == PLAYER_MRL_TYPE_AUDIO)
          cwd_cover = strdup (item->cover);
      }
      
      evas_object_event_callback_add (icon, EVAS_CALLBACK_MOUSE_DOWN,
                                      cb_browser_entry_execute, item);
      evas_object_event_callback_add (icon, EVAS_CALLBACK_MOUSE_WHEEL,
                                      cb_browser_mouse_wheel, browser);

      evas_object_event_callback_add (txt, EVAS_CALLBACK_MOUSE_DOWN,
                                      cb_browser_entry_execute, item);
      evas_object_event_callback_add (txt, EVAS_CALLBACK_MOUSE_WHEEL,
                                      cb_browser_mouse_wheel, browser);
      
      if (S_ISREG (st.st_mode))
      {
        evas_object_event_callback_add (txt, EVAS_CALLBACK_MOUSE_IN,
                                        cb_browser_get_file_info, item);
        evas_object_event_callback_add (txt, EVAS_CALLBACK_MOUSE_OUT,
                                        cb_browser_hide_file_info, NULL);
      }
      
      browser->entries = evas_list_append (browser->entries, item);
      
      free (path);
      free (namelist[i]);
    }
    free (namelist);
  }
  
  browser_compute (browser);
  browser_display_update (browser);
  screen_update_cwd (omc->screen);
}

browser_t *
browser_new (screen_t *screen, font_t *font, int filter_type,
             char *x, char *y, char *w, char *h)
{
  browser_t *browser = NULL;
  Evas_Object *b = NULL;
  
  browser = (browser_t *) malloc (sizeof (browser_t));

  browser->x = omc_compute_coord (x, omc->w);
  browser->y = omc_compute_coord (y, omc->h);
  browser->w = omc_compute_coord (w, omc->w);
  browser->h = omc_compute_coord (h, omc->h);

  browser->pos = 0;
  browser->capacity_w = 0;
  browser->capacity_h = 0;
  browser->filter_type = filter_type;
  browser->entries = NULL;
  browser->clip = NULL;
  browser->font = font;

  browser->clip = object_clipper (omc, x, y, w, h);
  b = border_new (omc, screen->objects, BORDER_TYPE_MENU, x, y, w, h);
  
  browser_update (omc, browser);

  evas_object_event_callback_add (b, EVAS_CALLBACK_MOUSE_WHEEL,
                                  cb_browser_mouse_wheel, browser);
  
  return browser;
}

void
browser_free (browser_t *browser)
{
  Evas_List *list;
  
  if (!browser)
    return;

  if (browser->entries)
  {
    for (list = browser->entries; list; list = list->next)
    {
      item_t *item = NULL;
      
      item = (item_t *) list->data;
      if (!item)
        continue;
      
      item_free (item);
      browser->entries = evas_list_remove_list (browser->entries,
                                                browser->entries);
    }
    free (browser->entries);
  }
  free (browser);
}

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
