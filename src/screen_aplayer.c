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
#include <Ecore_Evas.h>
#include <xine.h>

#include "screen.h"
#include "omc.h"
#include "filter.h"
#include "widget.h"
#include "avplayer.h"
#include "screen_audio.h"
#include "screen_aplayer.h"

typedef struct info_s {
  Evas_Object *box;
  Evas_List *border;
} info_t;

static info_t *
info_new (void)
{
  info_t *info = NULL;

  info = (info_t *) malloc (sizeof (info_t));
  info->box = NULL;
  info->border = NULL;
  
  return info;
}

static void
info_free (info_t *info)
{
  Evas_List *list;
  
  if (!info)
    return;

  if (info->box)
    evas_object_del (info->box);

  if (info->border)
  {
    for (list = info->border; list; list = list->next)
    {
      Evas_Object *obj;
    
      obj = (Evas_Object *) list->data;
      evas_object_del (obj);
      info->border = evas_list_remove_list (info->border, info->border);
    }
    free (info->border);
  }
  free (info);
}

typedef struct aplayer_s {
  Ecore_Timer *timer;
  Evas_Object *time_pos;
  Evas_Object *time_len;
  Evas_Object *time_border;
  Evas_Object *time_bar;
  Evas_Object *volume_border;
  Evas_Object *volume_bar;
  info_t *info;
  cover_t *cover;
  browser_t *browser;
} aplayer_t;

static aplayer_t *
aplayer_new (void)
{
  aplayer_t *aplayer = NULL;

  aplayer = (aplayer_t *) malloc (sizeof (aplayer_t));
  aplayer->timer = NULL;
  aplayer->time_pos = NULL;
  aplayer->time_len = NULL;
  aplayer->time_border = NULL;
  aplayer->time_bar = NULL;
  aplayer->volume_border = NULL;
  aplayer->volume_bar = NULL;
  aplayer->info = NULL;
  aplayer->cover = NULL;
  aplayer->browser = NULL;
  
  return aplayer;
}

static void
aplayer_free (aplayer_t *aplayer)
{
  if (!aplayer)
    return;

  if (aplayer->timer)
  {
    ecore_timer_del (aplayer->timer);
    aplayer->timer = NULL;
  }
  if (aplayer->time_pos)
    evas_object_del (aplayer->time_pos);
  if (aplayer->time_len)
    evas_object_del (aplayer->time_len);
  if (aplayer->time_border)
    evas_object_del (aplayer->time_border);
  if (aplayer->time_bar)
    evas_object_del (aplayer->time_bar);
  if (aplayer->volume_border)
    evas_object_del (aplayer->volume_border);
  if (aplayer->volume_bar)
    evas_object_del (aplayer->volume_bar); 
  if (aplayer->info)
    info_free (aplayer->info);
  if (aplayer->cover)
    cover_free (aplayer->cover);
  if (aplayer->browser)
    browser_free (aplayer->browser);
  free (aplayer);
}

#define SCREEN_IMG_BACKGROUND OMC_DATA_DIR"/back_brushed2.png"

static void
aplayer_background_setup (screen_t *screen)
{
  Evas_Object *obj = NULL;
  
  obj = image_new (omc, 0, SCREEN_IMG_BACKGROUND,
                   NULL, 0, "0", "0", "100%", "100%");
  screen->objects = evas_list_append (screen->objects, obj);
}

#define IMG_ICON_BACK_MENU OMC_DATA_DIR"/back_menu.png"
#define IMG_ICON_BACK_MENU_FOCUS OMC_DATA_DIR"/back_menu_focus.png"

#define IMG_ICON_PREV OMC_DATA_DIR"/ctrl_prev.png"
#define IMG_ICON_PREV_FOCUS OMC_DATA_DIR"/ctrl_prev_focus.png"
#define IMG_ICON_REWIND OMC_DATA_DIR"/ctrl_rewind.png"
#define IMG_ICON_REWIND_FOCUS OMC_DATA_DIR"/ctrl_rewind_focus.png"
#define IMG_ICON_PAUSE OMC_DATA_DIR"/ctrl_pause.png"
#define IMG_ICON_PAUSE_FOCUS OMC_DATA_DIR"/ctrl_pause_focus.png"
#define IMG_ICON_STOP OMC_DATA_DIR"/ctrl_stop.png"
#define IMG_ICON_STOP_FOCUS OMC_DATA_DIR"/ctrl_stop_focus.png"
#define IMG_ICON_PLAY OMC_DATA_DIR"/ctrl_play.png"
#define IMG_ICON_PLAY_FOCUS OMC_DATA_DIR"/ctrl_play_focus.png"
#define IMG_ICON_FORWARD OMC_DATA_DIR"/ctrl_forward.png"
#define IMG_ICON_FORWARD_FOCUS OMC_DATA_DIR"/ctrl_forward_focus.png"
#define IMG_ICON_NEXT OMC_DATA_DIR"/ctrl_next.png"
#define IMG_ICON_NEXT_FOCUS OMC_DATA_DIR"/ctrl_next_focus.png"

static void
cb_player_prev_mrl (void *data, Evas *e, Evas_Object *obj, void *event_info)
{
  printf ("Previous MRL\n");
  av_player_prev_mrl (omc->player);
}

static void
cb_player_rewind (void *data, Evas *e, Evas_Object *obj, void *event_info)
{
  printf ("Rewind\n");
  av_player_fast_rewind (omc->player, 10);
}

static void
cb_player_stop (void *data, Evas *e, Evas_Object *obj, void *event_info)
{
  printf ("Stop\n");
  av_player_stop (omc->player);
}

static void
cb_player_toggle_pause (void *data, Evas *e,
                        Evas_Object *obj, void *event_info)
{
  printf ("Toggle Pause\n");
  if (omc->player->state != PLAYER_STATE_IDLE)
    av_player_pause_playback (omc->player);
  else
    av_player_start (omc->player);
}

static void
cb_player_forward (void *data, Evas *e, Evas_Object *obj, void *event_info)
{
  printf ("Forward\n");
  av_player_fast_forward (omc->player, 10);
}

static void
cb_player_next_mrl (void *data, Evas *e, Evas_Object *obj, void *event_info)
{
  printf ("Next MRL\n");
  av_player_next_mrl (omc->player);
}

static void
cb_play_button_focus_update (void *data, Evas *e,
                             Evas_Object *obj, void *event_info)
{
  if (evas_object_focus_get (obj))
  {
    if (omc->player->state != PLAYER_STATE_RUNNING)
      evas_object_image_file_set (obj, IMG_ICON_PLAY_FOCUS, NULL);
    else
      evas_object_image_file_set (obj, IMG_ICON_PAUSE_FOCUS, NULL);
  }
  else
  {
    if (omc->player->state != PLAYER_STATE_RUNNING)
      evas_object_image_file_set (obj, IMG_ICON_PLAY, NULL);
    else
      evas_object_image_file_set (obj, IMG_ICON_PAUSE, NULL);
  }
} 

static void
aplayer_toolbar_setup (screen_t *screen)
{
  Evas_Object *obj = NULL;
  
  obj = image_new (omc, 1, IMG_ICON_BACK_MENU, IMG_ICON_BACK_MENU_FOCUS,
                   0, "91%", "92%", "8%", "8%");
  omc->screen->objects = evas_list_append (omc->screen->objects, obj);
  object_add_default_cb (obj);
  evas_object_event_callback_add (obj, EVAS_CALLBACK_MOUSE_DOWN,
                                  cb_switch_screen, SCREEN_AUDIO_TITLE);

  obj = image_new (omc, 1, IMG_ICON_PREV, IMG_ICON_PREV_FOCUS,
                   0, "34%", "95%", "20", "20");
  omc->screen->objects = evas_list_append (omc->screen->objects, obj);
  object_add_default_cb (obj);
  evas_object_event_callback_add (obj, EVAS_CALLBACK_MOUSE_DOWN,
                                  cb_player_prev_mrl, NULL);

  obj = image_new (omc, 1, IMG_ICON_REWIND, IMG_ICON_REWIND_FOCUS,
                   0, "38%", "93%", "28", "28");
  omc->screen->objects = evas_list_append (omc->screen->objects, obj);
  object_add_default_cb (obj);
  evas_object_event_callback_add (obj, EVAS_CALLBACK_MOUSE_DOWN,
                                  cb_player_rewind, NULL);

  obj = image_new (omc, 1, IMG_ICON_STOP, IMG_ICON_STOP_FOCUS,
                   0, "43%", "92%", "36", "36");
  omc->screen->objects = evas_list_append (omc->screen->objects, obj);
  object_add_default_cb (obj);
  evas_object_event_callback_add (obj, EVAS_CALLBACK_MOUSE_DOWN,
                                  cb_player_stop, NULL);

  if (omc->player->state == PLAYER_STATE_IDLE)
    obj = image_new (omc, 0, IMG_ICON_PLAY, NULL,
                     0, "50%", "92%", "36", "36");
  else
    obj = image_new (omc, 0, IMG_ICON_PAUSE, NULL,
                     0, "50%", "92%", "36", "36");
  omc->screen->objects = evas_list_append (omc->screen->objects, obj);
  object_add_default_cb (obj);
  evas_object_event_callback_add (obj, EVAS_CALLBACK_FOCUS_IN,
                                  cb_play_button_focus_update, NULL);
  evas_object_event_callback_add (obj, EVAS_CALLBACK_FOCUS_OUT,
                                  cb_play_button_focus_update, NULL);
  evas_object_event_callback_add (obj, EVAS_CALLBACK_MOUSE_DOWN,
                                  cb_player_toggle_pause, NULL);

  obj = image_new (omc, 1, IMG_ICON_FORWARD, IMG_ICON_FORWARD_FOCUS,
                   0, "57%", "93%", "28", "28");
  omc->screen->objects = evas_list_append (omc->screen->objects, obj);
  object_add_default_cb (obj);
  evas_object_event_callback_add (obj, EVAS_CALLBACK_MOUSE_DOWN,
                                  cb_player_forward, NULL);

  obj = image_new (omc, 1, IMG_ICON_NEXT, IMG_ICON_NEXT_FOCUS,
                   0, "62%", "95%", "20", "20");
  omc->screen->objects = evas_list_append (omc->screen->objects, obj);
  object_add_default_cb (obj);
  evas_object_event_callback_add (obj, EVAS_CALLBACK_MOUSE_DOWN,
                                  cb_player_next_mrl, NULL);
}

#define NOW_PLAYING_TEXT "Now Playing ...\n\n"

void
screen_aplayer_update_browser (screen_t *screen, char *mrl)
{
  aplayer_t *aplayer = NULL;
  browser_t *browser = NULL;
  Evas_List *list;
  
  if (!screen || screen->type != SCREEN_TYPE_APLAYER)
    return;

  aplayer = (aplayer_t *) screen->private;
  if (!aplayer)
    return;

  if (!mrl)
  {
    /* */
    return;
  }

  browser = aplayer->browser;
  if (!browser)
    return;

  for (list = browser->entries; list; list = list->next)
  {
    item_t *item = NULL;
        
    item = (item_t *) list->data;
    if (!item)
      continue;

    printf ("Item MRL : %s\n", item->mrl);
    
    if (strcmp (item->mrl, mrl))
      continue;
   
    if (item->text)
    {
      evas_object_color_set (item->text, 255, 12, 255, 255);
      break;
    }
  }
  
  printf ("Update MRL : %s\n", mrl);
}

void
screen_aplayer_update_infos (screen_t *screen, char *infos, int display)
{
  aplayer_t *aplayer = NULL;
  info_t *info = NULL;
  
  if (!screen || screen->type != SCREEN_TYPE_APLAYER)
    return;

  aplayer = (aplayer_t *) screen->private;
  if (!aplayer)
    return;

  info = aplayer->info;
  if (!info)
    return;

  if (display && infos)
  {
    Evas_List *list;

    if (info->border)
    {
      for (list = info->border; list; list = list->next)
      {
        Evas_Object *obj;
        
        obj = (Evas_Object *) list->data;
        evas_object_show (obj);
      }
    }

    if (info->box)
    {
      char *txt = NULL;
      txt = (char *) malloc (strlen (infos) + strlen (NOW_PLAYING_TEXT) + 1);
      sprintf (txt, "%s%s", NOW_PLAYING_TEXT, infos);
      text_block_add_text (info->box, txt);
      free (txt);
    }
  }
  else
  {
    Evas_List *list;

    if (info->border)
    {
      for (list = info->border; list; list = list->next)
      {
        Evas_Object *obj;
        
        obj = (Evas_Object *) list->data;
        evas_object_hide (obj);
      }
    }
    
    if (info->box)
      evas_object_textblock_clear (info->box);
  }
}

static void
aplayer_infos_setup (screen_t *screen)
{
  aplayer_t *aplayer = NULL;
  Evas_Object *dummy = NULL;
  font_t *font = NULL;

  font = get_font (omc->cfg->fonts, "infos");
  if (!font)
    return;
  
  aplayer = (aplayer_t *) screen->private;
  if (!aplayer)
    return;

  aplayer->info = info_new ();
  if (!aplayer->info)
    return;
  
  dummy = evas_object_image_add (omc->evas);
  aplayer->info->border = evas_list_append (aplayer->info->border, dummy);

  border_new (omc, aplayer->info->border,
              BORDER_TYPE_COVER, "3%", "14%", "40%", "30%");
  
  aplayer->info->box =
    text_block_new (omc, 0, "4%", "15%", "38%", "28%", 5, font,
                    BLK_ALIGN_LEFT, BLK_ALIGN_TOP);
}

void
screen_aplayer_update_cover (screen_t *screen, char *img, int display)
{
  aplayer_t *aplayer = NULL;
  cover_t *cover = NULL;
  
  if (!screen || screen->type != SCREEN_TYPE_APLAYER)
    return;

  aplayer = (aplayer_t *) screen->private;
  if (!aplayer)
    return;

  cover = aplayer->cover;
  if (!cover || !cover->cover)
    return;

  if (display && img)
  {
    Evas_List *list;

    if (cover->border)
    {
      for (list = cover->border; list; list = list->next)
      {
        Evas_Object *obj;
        
        obj = (Evas_Object *) list->data;
        evas_object_show (obj);
      }
    }

    if (cover->cover)
    {
      evas_object_image_file_set (cover->cover, img, NULL);
      evas_object_show (cover->cover);
    }
  }
  else
  {
    Evas_List *list;

    if (cover->border)
    {
      for (list = cover->border; list; list = list->next)
      {
        Evas_Object *obj;
        
        obj = (Evas_Object *) list->data;
        evas_object_hide (obj);
      }
    }

    if (cover->cover)
      evas_object_hide (cover->cover);
  }
}

static void
aplayer_cover_setup (screen_t *screen)
{
  aplayer_t *aplayer = NULL;
  Evas_Object *dummy = NULL;
  char s[8];
  int size;
  
  aplayer = (aplayer_t *) screen->private;
  if (!aplayer)
    return;

  aplayer->cover = cover_new ();
  if (!aplayer->cover)
    return;

  dummy = evas_object_image_add (omc->evas);
  aplayer->cover->border = evas_list_append (aplayer->cover->border, dummy);

  size = omc_compute_coord ("35%", omc->h);
  sprintf (s, "%d", size);

  border_new (omc, aplayer->cover->border,
              BORDER_TYPE_COVER, "3%", "50%", s, s);

  aplayer->cover->cover =
    image_new (omc, 0, NULL, NULL, 0, "3%", "50%", s, s);
}

static void
aplayer_browser_setup (screen_t *screen)
{
  aplayer_t *aplayer = NULL;
  font_t *font = NULL;

  font = get_font (omc->cfg->fonts, "playlist");
  if (!font)
    return;

  aplayer = (aplayer_t *) screen->private;
  if (!aplayer)
    return;
  
  aplayer->browser =
    browser_new (omc->screen, font,
                 FILTER_TYPE_MRL, "52%", "15%", "44%", "70%");
}

static int
timer_position (void *data)
{
  Evas_Object *opos = NULL, *olen = NULL;
  Evas_Object *border = NULL, *progress = NULL;
  aplayer_t *aplayer = NULL;

  if (omc->screen->type != SCREEN_TYPE_APLAYER)
    return 0;
  
  aplayer = (aplayer_t *) omc->screen->private;
  if (!aplayer)
    return 0;

  opos = aplayer->time_pos;
  olen = aplayer->time_len;
  border = aplayer->time_border;
  progress = aplayer->time_bar;
  
  if (omc->player->stream && omc->player->state == PLAYER_STATE_RUNNING)
  {
    char time[1024];
    int pos_time, pos, pos_m = 0, pos_s = 0;
    int length_time, len, len_m = 0, len_s = 0;

    xine_get_pos_length (omc->player->stream, NULL, &pos_time, &length_time);
    pos = (int) (pos_time / 1000);
    pos_m = (int) (pos / 60);
    pos_s = pos - (pos_m * 60);
    len = (int) (length_time / 1000);
    len_m = (int) (len / 60);
    len_s = len - (len_m * 60);

    if (opos)
    {
      memset (time, '\0', 1024);
      sprintf (time, "%.2d:%.2d", pos_m, pos_s);
      evas_object_text_text_set (opos, time);
    }
    if (olen)
    {
      memset (time, '\0', 1024);
      sprintf (time, "%.2d:%.2d", len_m, len_s);
      evas_object_text_text_set (olen, time);
    }
    if (border && progress)
    {
      Evas_Coord wb, wp, hp;
      int percent = 0;
      
      evas_object_geometry_get (border, NULL, NULL, &wb, NULL);
      evas_object_geometry_get (progress, NULL, NULL, NULL, &hp);
      percent = (int) (pos * 100 / len);
      wp = (int) (wb * percent / 100);
      if (wp > wb)
        wp = wb;
      evas_object_resize (progress, wp, hp);
    }
  }
  else if (omc->player->state == PLAYER_STATE_IDLE)
  {
    char time[1024];
    memset (time, '\0', 1024);
    sprintf (time, "%.2d:%.2d", 0, 0);

    if (opos)
      evas_object_text_text_set (opos, time);
    if (olen)
      evas_object_text_text_set (olen, time);
    if (progress)
    {
      Evas_Coord h;
      evas_object_geometry_get (progress, NULL, NULL, NULL, &h);
      evas_object_resize (progress, 0, h);
    }
  }

  return 1; /* to keep the timer going */
}

static void
cb_player_seek (void *data, Evas *e, Evas_Object *obj, void *event_info)
{
  Evas_Event_Mouse_Down *event = event_info;
  aplayer_t *aplayer = (aplayer_t *) omc->screen->private;
  Evas_Object *border = aplayer->time_border;
  Evas_Coord x, w;
  int length = 0, seek = 0, percent = 0;
  double mx;

  if (!omc->player->stream || omc->player->state == PLAYER_STATE_IDLE)
    return;

  if (!border)
    return;
  
  mx = event->canvas.x;
  evas_object_geometry_get (border, &x, NULL, &w, NULL);
  percent = 100 - (int) ((x + w - mx) / w * 100);
  if (percent < 0)
    percent = 0;
  if (percent > 100)
    percent = 100;
  
  xine_get_pos_length (omc->player->stream, NULL, NULL, &length);
  seek = (int) (percent * length / 100);
  xine_play (omc->player->stream, 0, seek);
}

#define IMG_ICON_PROGRESS_BORDER OMC_DATA_DIR"/progress_border.png"

static void
aplayer_timer_setup (screen_t *screen)
{
  aplayer_t *aplayer = (aplayer_t *) screen->private;
  font_t *font = NULL;

  font = get_font (omc->cfg->fonts, "infos");
  if (!font)
    return;
  
  aplayer->time_pos = text_new (omc, 0, font, "", 255, 0, "59%", "2%");
  screen->objects = evas_list_append (screen->objects, aplayer->time_pos);

  aplayer->time_border = image_new (omc, 0, IMG_ICON_PROGRESS_BORDER, NULL,
                                    0, "67%", "1%", "25%", "5%");
  screen->objects = evas_list_append (screen->objects, aplayer->time_border);
  evas_object_event_callback_add (aplayer->time_border,
                                  EVAS_CALLBACK_MOUSE_DOWN,
                                  cb_player_seek, NULL);

  aplayer->time_bar = object_clipper (omc, "67%", "1%", "0%", "5%");
  evas_object_color_set (aplayer->time_bar, 0, 0, 0, 128);
  evas_object_event_callback_add (aplayer->time_bar, EVAS_CALLBACK_MOUSE_DOWN,
                                  cb_player_seek, NULL);
  
  aplayer->time_len = text_new (omc, 0, font, "", 255, 0, "93%", "2%");
  screen->objects = evas_list_append (screen->objects, aplayer->time_len);

  aplayer->timer = ecore_timer_add (1.0, timer_position, NULL);
}

#define IMG_ICON_VOLUME OMC_DATA_DIR"/volume.png"

static void
cb_player_volume_update (void *data, Evas *e,
                         Evas_Object *obj, void *event_info)
{
  Evas_Event_Mouse_Down *event = event_info;
  Evas_Object *border = NULL, *progress = NULL;
  aplayer_t *aplayer = (aplayer_t *) omc->screen->private;
  Evas_Coord x, w, h;
  int len = 0, percent = 0;
  double mx;

  if (!omc->player->stream || omc->player->state == PLAYER_STATE_IDLE)
    return;

  border = aplayer->volume_border;
  progress = aplayer->volume_bar;
  
  if (!border || !progress)
    return;
  
  mx = event->canvas.x;
  evas_object_geometry_get (border, &x, NULL, &w, NULL);
  percent = 100 - (int) ((x + w - mx) / w * 100);
  if (percent < 0)
    percent = 0;
  if (percent > 100)
    percent = 100;

  xine_set_param (omc->player->stream, XINE_PARAM_AUDIO_VOLUME, percent);
  len = (int) (percent * w / 100);
  evas_object_geometry_get (border, NULL, NULL, NULL, &h);
  evas_object_resize (progress, len, h);
}

static void
aplayer_volume_setup (screen_t *screen)
{
  Evas_Object *volume = NULL;
  aplayer_t *aplayer = (aplayer_t *) screen->private;
  
  volume = image_new (omc, 0, IMG_ICON_VOLUME, NULL,
                      0, "1%", "1%", "5%", "5%");
  screen->objects = evas_list_append (screen->objects, volume);

  aplayer->volume_border = image_new (omc, 0, IMG_ICON_PROGRESS_BORDER, NULL,
                                      0, "7%", "1%", "12%", "5%");
  screen->objects = evas_list_append (screen->objects, aplayer->volume_border);

  aplayer->volume_bar = object_clipper (omc, "7%", "1%", "0%", "5%");
  evas_object_color_set (aplayer->volume_bar, 0, 0, 0, 128);

  if (omc->player->stream && omc->player->state != PLAYER_STATE_IDLE)
  {
    int vol = 0, len = 0;
    Evas_Coord w, h;
    
    vol = xine_get_param (omc->player->stream, XINE_PARAM_AUDIO_VOLUME);
    evas_object_geometry_get (aplayer->volume_border, NULL, NULL, &w, NULL);
    evas_object_geometry_get (aplayer->volume_bar, NULL, NULL, NULL, &h);
    len = (int) (vol * w / 100);
    evas_object_resize (aplayer->volume_bar, len, h);
  }

  evas_object_event_callback_add (aplayer->volume_border,
                                  EVAS_CALLBACK_MOUSE_DOWN,
                                  cb_player_volume_update, NULL);
  evas_object_event_callback_add (aplayer->volume_bar,
                                  EVAS_CALLBACK_MOUSE_DOWN,
                                  cb_player_volume_update, NULL);
}

void
screen_aplayer_setup (screen_t *screen, char *data)
{
  if (!screen || screen->type != SCREEN_TYPE_APLAYER)
    return;

  screen->private = (aplayer_t *) aplayer_new ();
  
  aplayer_background_setup (screen);
  aplayer_toolbar_setup (screen);
  
  aplayer_infos_setup (screen);
  aplayer_cover_setup (screen);
  aplayer_browser_setup (screen);
  aplayer_volume_setup (screen);
  aplayer_timer_setup (screen);
}

void
screen_aplayer_display (screen_t *screen)
{
  aplayer_t *aplayer = NULL;
  av_player_t *player = NULL;

  aplayer = (aplayer_t *) screen->private;
  if (!aplayer)
    return;
  
  if (aplayer->info && aplayer->info->box)
    evas_object_show (aplayer->info->box);

  player = omc->player;
  if (!player)
    return;
  
  if (player && player->state == PLAYER_STATE_RUNNING)
  {
    if (player->current->infos)
      screen_aplayer_update_infos (screen, player->current->infos, 1);
    if (player->current->cover)
      screen_aplayer_update_cover (screen, player->current->cover, 1);
    screen_aplayer_update_browser (screen, player->current->file);
  }
}

void
screen_aplayer_free (screen_t *screen)
{
  aplayer_t *aplayer = NULL;
  
  if (!screen || screen->type != SCREEN_TYPE_APLAYER)
    return;

  aplayer = (aplayer_t *) screen->private;
  if (aplayer)
    aplayer_free (aplayer);
}
