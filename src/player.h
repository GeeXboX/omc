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

#ifndef _PLAYER_H_
#define _PLAYER_H_

#include "widget.h"
#include <xine.h>

enum {
  PLAYER_STATE_IDLE,
  PLAYER_STATE_PAUSE,
  PLAYER_STATE_RUNNING
};

enum {
  PLAYER_MRL_TYPE_NONE,
  PLAYER_MRL_TYPE_AUDIO,
  PLAYER_MRL_TYPE_VIDEO,
  PLAYER_MRL_TYPE_IMAGE
};

struct mrl_t {
  char *file;
  int type;
  char *infos;
  char *cover;
};

struct player_t {
  xine_t *xine;
  xine_stream_t *stream;
  xine_event_queue_t *event_queue;
  xine_video_port_t *vo_port;
  xine_audio_port_t *ao_port;
  Evas_List *playlist;
  struct mrl_t *current;
  int state;
  int type;
  int loop;
  int shuffle;
  int x, y;
  int w, h;
};

struct player_t *player_init (void);
void player_uninit (struct player_t *player);

void player_start (struct player_t *player);
void player_prev_mrl (struct player_t *player);
void player_next_mrl (struct player_t *player);
void player_pause_playback (struct player_t *player);
void player_stop (struct player_t *player);

void player_fast_forward (struct player_t *player, int value);
void player_fast_rewind (struct player_t *player, int value);
void player_volume_up (struct player_t *player, int value);
void player_volume_down (struct player_t *player, int value);

enum {
  PLAYER_ADD_MRL_NOW,
  PLAYER_ADD_MRL_QUEUE
};

void player_add_mrl (struct player_t *player, struct item_t *item, int when);

#endif /* _PLAYER_H_ */
