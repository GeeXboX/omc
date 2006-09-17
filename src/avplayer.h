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

#ifndef _AV_PLAYER_H_
#define _AV_PLAYER_H_

#define PLAYER_MRL_TYPE_IMAGE 3

player_t *av_player_init (player_type_t type);
void av_player_uninit (player_t *player);

void av_player_start (player_t *player);
void av_player_prev_mrl (player_t *player);
void av_player_next_mrl (player_t *player);
void av_player_pause_playback (player_t *player);
void av_player_stop (player_t *player);

void av_player_fast_forward (player_t *player, int value);
void av_player_fast_rewind (player_t *player, int value);
void av_player_volume_up (player_t *player, int value);
void av_player_volume_down (player_t *player, int value);

void av_player_add_mrl (player_t *player,
                        item_t *item, player_add_mrl_t when);

#endif /* _AV_PLAYER_H_ */
