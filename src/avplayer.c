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
#include <player.h>

#include "screens/screen.h"
#include "omc.h"
#include "widget.h"
#include "avplayer.h"
#include "screens/screen_aplayer.h"

av_mrl_t *
av_mrl_new (char *file, int type, char *infos, char *cover)
{
  av_mrl_t *mrl = NULL;

  if (!file)
    return NULL;
  
  mrl = (av_mrl_t *) malloc (sizeof (av_mrl_t));
  mrl->file = strdup (file);
  mrl->type = type;
  mrl->infos = infos ? strdup (infos) : NULL;
  mrl->cover = cover ? strdup (cover) : NULL;

  return  mrl;
}

void
av_mrl_free (av_mrl_t *mrl)
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

static void
av_player_event_listener_cb (void *user_data, const xine_event_t *event)
{
  av_player_t *av_player = NULL;

  av_player = (av_player_t *) user_data;
  if (!av_player)
    return;

  switch (event->type)
  {
  case XINE_EVENT_UI_PLAYBACK_FINISHED:
  {
    printf ("Playback of stream has ended\n");
    av_player_next_mrl (av_player);
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

av_player_t *
av_player_init (void)
{
  av_player_t *av_player = NULL;

  av_player = (av_player_t *) malloc (sizeof (av_player_t));
  av_player->xine = NULL;
  av_player->stream = NULL;
  av_player->event_queue = NULL;
  av_player->vo_port = NULL;
  av_player->ao_port = NULL;
  av_player->playlist = NULL;
  av_player->current = NULL;
  av_player->state = PLAYER_STATE_IDLE;
  av_player->type = PLAYER_MRL_TYPE_NONE;
  av_player->loop = 0;
  av_player->shuffle = 0;
  av_player->x = 0;
  av_player->y = 0;
  av_player->w = 0;
  av_player->h = 0;

  if (!av_player->xine)
  {
    av_player->xine = xine_new ();
    xine_init (av_player->xine);

    xine_engine_set_param (av_player->xine,
                           XINE_ENGINE_PARAM_VERBOSITY, XINE_VERBOSITY_NONE);
  }

  //av_player->vo_port = xine_open_video_driver (av_player->xine, NULL, NULL);
  av_player->ao_port = xine_open_audio_driver (av_player->xine, "auto", NULL);

  av_player->stream =
    xine_stream_new (av_player->xine, av_player->ao_port, av_player->vo_port);
  
  av_player->event_queue = xine_event_new_queue (av_player->stream);
  xine_event_create_listener_thread (av_player->event_queue,
                                     av_player_event_listener_cb, av_player);
  
  return av_player;
}

void
av_player_uninit (av_player_t *av_player)
{
  Evas_List *list;
  
  if (!av_player)
    return;

  if (av_player->stream)
  {
    xine_close (av_player->stream);
    xine_dispose (av_player->stream);
  }
  
  if (av_player->event_queue)
    xine_event_dispose_queue (av_player->event_queue);

  if (av_player->ao_port)
    xine_close_audio_driver (av_player->xine, av_player->ao_port);
  if (av_player->vo_port)
    xine_close_video_driver (av_player->xine, av_player->vo_port);
  
  if (av_player->xine)
    xine_exit (av_player->xine);

  for (list = av_player->playlist; list; list = list->next)
  {
    av_mrl_t *mrl = NULL;

    mrl = (av_mrl_t *) list->data;
    if (mrl)
      av_mrl_free (mrl);
  }
  
  free (av_player);
}

void
av_player_set_loop (av_player_t *av_player, int value)
{
  if (!av_player)
    return;

  av_player->loop = value;
}

void
av_player_set_shuffle (av_player_t *av_player, int value)
{
  if (!av_player)
    return;

  av_player->shuffle = value;
}

void
av_player_prev_mrl (av_player_t *av_player)
{
  Evas_List *list;

  if (!av_player)
    return;

  for (list = av_player->playlist; list; list = list->next)
  {
    av_mrl_t *mrl;
    
    mrl = (av_mrl_t *) list->data;
    if (mrl == av_player->current)
    {
      if (list->prev)
      {
        av_player->current = (av_mrl_t *) list->prev->data;
        av_player_start (av_player);
        break;
      }

      if (av_player->loop)
      {
        while (list && list->next)
          list = list->next;
        av_player->current = (av_mrl_t *) list->data;
        av_player_start (av_player);
        break;
      }
    }
  }
}

void
av_player_next_mrl (av_player_t *av_player)
{
  Evas_List *list;

  if (!av_player)
    return;

  for (list = av_player->playlist; list; list = list->next)
  {
    av_mrl_t *mrl;
    
    mrl = (av_mrl_t *) list->data;
    if (mrl == av_player->current)
    {
      if (list->next)
      {
        av_player->current = (av_mrl_t *) list->next->data;
        av_player_start (av_player);
        break;
      }

      if (av_player->loop)
      {
        while (list && list->prev)
          list = list->prev;
        av_player->current = (av_mrl_t *) list->data;
        av_player_start (av_player);
        break;
      }
    }
  }
}

void
av_player_pause_playback (av_player_t *av_player)
{
  if (!av_player || !av_player->stream)
    return;
  
  if (xine_get_param (av_player->stream, XINE_PARAM_SPEED) != XINE_SPEED_PAUSE)
  {
    xine_set_param (av_player->stream, XINE_PARAM_SPEED, XINE_SPEED_PAUSE);
    av_player->state = PLAYER_STATE_PAUSE;
  }
  else
  {
    xine_set_param (av_player->stream, XINE_PARAM_SPEED, XINE_SPEED_NORMAL);
    av_player->state = PLAYER_STATE_RUNNING;
  }
}

void
av_player_stop (av_player_t *av_player)
{
  if (av_player->state == PLAYER_STATE_IDLE)
    return; /* not running */

  av_player->state = PLAYER_STATE_IDLE;

  if (av_player->stream)
  {
    xine_stop (av_player->stream);
    xine_close (av_player->stream);
  }

  if (omc->screen->type == SCREEN_TYPE_APLAYER)
  {
    screen_aplayer_update_infos (omc->screen, NULL, 0);
    screen_aplayer_update_cover (omc->screen, NULL, 0);
    screen_aplayer_update_browser (omc->screen, NULL);
  }
}

void
av_player_start (av_player_t *av_player)
{
  if (!av_player)
    return;
  
  if (av_player->state != PLAYER_STATE_IDLE) /* already running : stop it */
    av_player_stop (av_player);

  if (!av_player->current)
    return;

  if (!av_player->stream)
    return;
  
  av_player->type = av_player->current->type;
  av_player->state = PLAYER_STATE_RUNNING;

  xine_open (av_player->stream, av_player->current->file);
  xine_play (av_player->stream, 0, 0);

  printf ("Now playing : %s\n", av_player->current->file);

  if (omc->screen->type == SCREEN_TYPE_APLAYER)
  {
    if (av_player->current->infos)
      screen_aplayer_update_infos (omc->screen, av_player->current->infos, 1);
    if (av_player->current->cover)
      screen_aplayer_update_cover (omc->screen, av_player->current->cover, 1);
    screen_aplayer_update_browser (omc->screen, av_player->current->file);
  }
  else
    screen_update_notifier (omc->screen,
                            av_player->current->cover,
                            basename (av_player->current->file));
}

void
av_player_fast_forward (av_player_t *av_player, int value)
{
  int pos_time = 0, length = 0;
  
  if (!av_player || !av_player->stream)
    return;

  xine_get_pos_length (omc->player->stream, NULL, &pos_time, &length);
  pos_time += value * 1000;
  if (pos_time > length)
    pos_time = length;
  xine_play (av_player->stream, 0, pos_time);
}

void
av_player_fast_rewind (av_player_t *av_player, int value)
{
  int pos_time = 0, length = 0;
  
  if (!av_player || !av_player->stream)
    return;

  xine_get_pos_length (omc->player->stream, NULL, &pos_time, &length);
  pos_time -= value * 1000;
  if (pos_time < 0)
    pos_time = 0;
  xine_play (av_player->stream, 0, pos_time);
}

void
av_player_volume_up (av_player_t *av_player, int value)
{
  if (!av_player || !av_player->stream)
    return;

  xine_set_param (av_player->stream, XINE_PARAM_AUDIO_VOLUME,
                  (xine_get_param (av_player->stream,
                                   XINE_PARAM_AUDIO_VOLUME) + 1));
}

void
av_player_volume_down (av_player_t *av_player, int value)
{
  if (!av_player || !av_player->stream)
    return;

  xine_set_param (av_player->stream, XINE_PARAM_AUDIO_VOLUME,
                  (xine_get_param (av_player->stream,
                                   XINE_PARAM_AUDIO_VOLUME) - 1));
}

void
av_player_add_mrl (av_player_t *av_player, item_t *item, int when)
{
  av_mrl_t *mrl = NULL;

  if (!item)
    return;

  mrl = av_mrl_new (item->mrl->name, item->mrl->type, item->infos, item->cover);
  av_player->playlist = evas_list_append (av_player->playlist, mrl);

  if (when == PLAYER_ADD_MRL_NOW)
  {
    av_player->current = mrl;
    av_player_start (av_player);
  }
}
   
