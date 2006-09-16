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

#ifndef _AV_DEMUXER_H_
#define _AV_DEMUXER_H_

#include <player.h>
#include <pthread.h>

typedef struct demuxer_s {
  int busy;
  pthread_t th;
  pthread_cond_t cond;
  pthread_mutex_t mutex;
  struct player_s *player;
  item_t *current;
} demuxer_t;

demuxer_t *av_demuxer_init (player_type_t type);
void av_demuxer_uninit (demuxer_t *demuxer);
void av_demux_mrl (item_t *item);

#endif /* _AV_DEMUXER_H_ */
