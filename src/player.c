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
#include <libgen.h>
#include <xine.h>

#include "omc.h"
#include "widget.h"
#include "player.h"
#include "screen.h"
#include "screen_aplayer.h"

struct mrl_t *
mrl_new (char *file, int type, char *infos, char *cover)
{
  struct mrl_t *mrl = NULL;

  if (!file)
    return NULL;
  
  mrl = (struct mrl_t *) malloc (sizeof (struct mrl_t));
  mrl->file = strdup (file);
  mrl->type = type;
  mrl->infos = infos ? strdup (infos) : NULL;
  mrl->cover = cover ? strdup (cover) : NULL;

  return  mrl;
}

void
mrl_free (struct mrl_t *mrl)
{
  if (!mrl)
    return;

  if (mrl->file)
    free (mrl->file);
  if (mrl->infos)
    free (mrl->infos);
  if (mrl->cover)
    free (mrl->cover);
  free (mrl);
}

struct player_t *
player_init (void)
{
  struct player_t *player = NULL;

  player = (struct player_t *) malloc (sizeof (struct player_t));
  player->xine = NULL;
  player->stream = NULL;
  player->event_queue = NULL;
  player->vo_port = NULL;
  player->ao_port = NULL;
  player->playlist = NULL;
  player->current = NULL;
  player->state = PLAYER_STATE_IDLE;
  player->type = PLAYER_MRL_TYPE_NONE;
  player->loop = 0;
  player->shuffle = 0;
  player->x = 0;
  player->y = 0;
  player->w = 0;
  player->h = 0;

  if (!player->xine)
  {
    player->xine = xine_new ();
    xine_init (player->xine);

    xine_engine_set_param (player->xine,
                           XINE_ENGINE_PARAM_VERBOSITY, XINE_VERBOSITY_NONE);
  }

  //player->vo_port = xine_open_video_driver (player->xine, NULL, NULL);
  player->ao_port = xine_open_audio_driver (player->xine, "auto", NULL);
  
  return player;
}

void
player_uninit (struct player_t *player)
{
  Evas_List *list;
  
  if (!player)
    return;

  if (player->event_queue)
    xine_event_dispose_queue (player->event_queue);

  if (player->stream)
  {
    xine_close (player->stream);
    xine_dispose (player->stream);
  }

  if (player->ao_port)
    xine_close_audio_driver (player->xine, player->ao_port);
  if (player->vo_port)
    xine_close_video_driver (player->xine, player->vo_port);
  
  if (player->xine)
    xine_exit (player->xine);

  for (list = player->playlist; list; list = list->next)
  {
    struct mrl_t *mrl = NULL;

    mrl = (struct mrl_t *) list->data;
    if (mrl)
      mrl_free (mrl);
  }
  
  if (player->current)
    free (player->current);
  
  free (player);
}

void
player_set_loop (struct player_t *player, int value)
{
  if (!player)
    return;

  player->loop = value;
}

void
player_set_shuffle (struct player_t *player, int value)
{
  if (!player)
    return;

  player->shuffle = value;
}

static void
player_event_listener_cb (void *user_data, const xine_event_t *event)
{
  struct player_t *player = (struct player_t *) user_data;
  if (!player)
    return;
  
  switch (event->type)
  {
  case XINE_EVENT_UI_PLAYBACK_FINISHED:
  {
    Evas_List *list;
    
    printf ("Playback of stream has ended\n");
    player_stop (player);
     
    for (list = player->playlist; list; list = list->next)
    {
      struct mrl_t *mrl;
    
      mrl = (struct mrl_t *) list->data;
      if (mrl == player->current && list->next)
      {
        player->current = (struct mrl_t *) list->next->data;
        player_start (player);
        break;
      }
    }
    break;
  }
  case XINE_EVENT_PROGRESS:
  {
    xine_progress_data_t *pevent = (xine_progress_data_t *) event->data;
    printf ("%s [%d%%]\n", pevent->description, pevent->percent);
    break;
  }
  }
}

void
player_prev_mrl (struct player_t *player)
{
  Evas_List *list;

  if (!player)
    return;

  for (list = player->playlist; list; list = list->next)
  {
    struct mrl_t *mrl;
    
    mrl = (struct mrl_t *) list->data;
    if (mrl == player->current)
    {
      if (list->prev)
      {
        player->current = (struct mrl_t *) list->prev->data;
        player_start (player);
        break;
      }

      if (player->loop)
      {
        while (list && list->next)
          list = list->next;
        player->current = (struct mrl_t *) list->data;
        player_start (player);
        break;
      }
    }
  }
}

void
player_next_mrl (struct player_t *player)
{
  Evas_List *list;

  if (!player)
    return;

  for (list = player->playlist; list; list = list->next)
  {
    struct mrl_t *mrl;
    
    mrl = (struct mrl_t *) list->data;
    if (mrl == player->current)
    {
      if (list->next)
      {
        player->current = (struct mrl_t *) list->next->data;
        player_start (player);
        break;
      }

      if (player->loop)
      {
        while (list && list->prev)
          list = list->prev;
        player->current = (struct mrl_t *) list->data;
        player_start (player);
        break;
      }
    }
  }
}

void
player_pause_playback (struct player_t *player)
{
  if (!player || !player->stream)
    return;
  
  if (xine_get_param (player->stream, XINE_PARAM_SPEED) != XINE_SPEED_PAUSE)
  {
    xine_set_param (player->stream, XINE_PARAM_SPEED, XINE_SPEED_PAUSE);
    player->state = PLAYER_STATE_PAUSE;
  }
  else
  {
    xine_set_param (player->stream, XINE_PARAM_SPEED, XINE_SPEED_NORMAL);
    player->state = PLAYER_STATE_RUNNING;
  }
}

void
player_stop (struct player_t *player)
{
  if (player->state == PLAYER_STATE_IDLE)
    return; /* not running */

  player->state = PLAYER_STATE_IDLE;

  if (player->event_queue)
  {
    xine_event_dispose_queue (player->event_queue);
    player->event_queue = NULL;
  }

  if (player->stream)
  {
    xine_close (player->stream);
    xine_dispose (player->stream);
    player->stream = NULL;
  }

  if (omc->screen->type == SCREEN_TYPE_APLAYER)
  {
    screen_aplayer_update_infos (omc->screen, NULL, 0);
    screen_aplayer_update_cover (omc->screen, NULL, 0);
    screen_aplayer_update_browser (omc->screen, NULL);
  }
}

void
player_start (struct player_t *player)
{
  if (!player)
    return;
  
  if (player->state != PLAYER_STATE_IDLE) /* already running : stop it */
    player_stop (player);

  if (!player->current)
    return;
  
  player->type = player->current->type;
  player->state = PLAYER_STATE_RUNNING;

  if (!player->stream)
    player->stream =
      xine_stream_new (player->xine, player->ao_port, player->vo_port);
  
  if (!player->stream)
    return;

  if (!player->event_queue)
    player->event_queue = xine_event_new_queue (player->stream);
  xine_event_create_listener_thread (player->event_queue,
                                     player_event_listener_cb,
                                     player);

  xine_open (player->stream, player->current->file);
  xine_play (player->stream, 0, 0);

  printf ("Now playing : %s\n", player->current->file);

  if (omc->screen->type == SCREEN_TYPE_APLAYER)
  {
    if (player->current->infos)
      screen_aplayer_update_infos (omc->screen, player->current->infos, 1);
    if (player->current->cover)
      screen_aplayer_update_cover (omc->screen, player->current->cover, 1);
    screen_aplayer_update_browser (omc->screen, player->current->file);
  }
  else
    screen_update_notifier (omc->screen,
                            player->current->cover,
                            basename (player->current->file));
}

void
player_fast_forward (struct player_t *player, int value)
{
  int pos_time = 0, length = 0;
  
  if (!player || !player->stream)
    return;

  xine_get_pos_length (omc->player->stream, NULL, &pos_time, &length);
  pos_time += value * 1000;
  if (pos_time > length)
    pos_time = length;
  xine_play (player->stream, 0, pos_time);
}

void
player_fast_rewind (struct player_t *player, int value)
{
  int pos_time = 0, length = 0;
  
  if (!player || !player->stream)
    return;

  xine_get_pos_length (omc->player->stream, NULL, &pos_time, &length);
  pos_time -= value * 1000;
  if (pos_time < 0)
    pos_time = 0;
  xine_play (player->stream, 0, pos_time);
}

void
player_volume_up (struct player_t *player, int value)
{
  if (!player || !player->stream)
    return;

  xine_set_param (player->stream, XINE_PARAM_AUDIO_VOLUME,
                  (xine_get_param (player->stream,
                                   XINE_PARAM_AUDIO_VOLUME) + 1));
}

void
player_volume_down (struct player_t *player, int value)
{
  if (!player || !player->stream)
    return;

  xine_set_param (player->stream, XINE_PARAM_AUDIO_VOLUME,
                  (xine_get_param (player->stream,
                                   XINE_PARAM_AUDIO_VOLUME) - 1));
}

void
player_add_mrl (struct player_t *player, struct item_t *item, int when)
{
  struct mrl_t *mrl = NULL;

  if (!item)
    return;

  mrl = mrl_new (item->mrl, item->mrl_type, item->infos, item->cover);
  player->playlist = evas_list_append (player->playlist, mrl);

  if (when == PLAYER_ADD_MRL_NOW)
  {
    player->current = mrl;
    player_start (player);
  }
}
   
