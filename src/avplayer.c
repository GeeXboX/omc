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

static int
avplayer_event_cb (player_event_t e, void *data)
{
  printf ("Received event type %d from player\n", e);

  switch (e)
  {
  case PLAYER_EVENT_PLAYBACK_FINISHED:
  {
    printf ("Playback of stream has ended\n");
    av_player_next_mrl (omc->player);
    break;
  }
  default:
    break;
  }
  
  return 0;
}

player_t *
av_player_init (player_type_t type)
{
  player_t *av_player = NULL;

  av_player = player_init (type, "auto", NULL, avplayer_event_cb);
  
  return av_player;
}

void
av_player_uninit (player_t *av_player)
{
  if (av_player)
    player_uninit (av_player);
}

void
av_player_prev_mrl (player_t *av_player)
{
  player_mrl_previous (av_player);
}

void
av_player_next_mrl (player_t *av_player)
{
  player_mrl_next (av_player);
}

void
av_player_pause_playback (player_t *av_player)
{
  player_playback_pause (av_player);
}

void
av_player_stop (player_t *av_player)
{
  player_playback_stop (av_player);
  
  if (omc->screen->type == SCREEN_TYPE_APLAYER)
  {
    screen_aplayer_update_infos (omc->screen, NULL, 0);
    screen_aplayer_update_cover (omc->screen, NULL, 0);
    screen_aplayer_update_browser (omc->screen, NULL);
  }
}

void
av_player_start (player_t *av_player)
{
  player_playback_start (av_player);
  
  printf ("Now playing : %s\n", av_player->mrl->name);

  /*
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
  */
}

void
av_player_fast_forward (player_t *av_player, int value)
{
  player_playback_seek (av_player, value);
}

void
av_player_fast_rewind (player_t *av_player, int value)
{
  player_playback_seek (av_player, -value);
}

void
av_player_volume_up (player_t *av_player, int value)
{
  player_set_volume (av_player, value);
}

void
av_player_volume_down (player_t *av_player, int value)
{
  player_set_volume (av_player, -value);
}

void
av_player_add_mrl (player_t *av_player, item_t *item, player_add_mrl_t when)
{
  player_mrl_append (av_player, item->mrl->name, item->mrl->type, when);
}
