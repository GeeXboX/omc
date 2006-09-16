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
#include <inttypes.h>
#include <player.h>
#include <pthread.h>
#include <Evas.h>
#include <Ecore_Evas.h>

#include "screens/screen.h"
#include "omc.h"
#include "widgets/widget.h"
#include "av_demuxer.h"

static void *
th_demuxer (void *data)
{
  demuxer_t *demuxer = (demuxer_t *) data;

  printf ("th_demuxer has started\n");
  
  while (1)
  {
    pthread_cond_wait (&demuxer->cond, &demuxer->mutex);

    /* do the demuxing stuff */
    player_mrl_get_properties (demuxer->player, demuxer->current->mrl);
    player_mrl_get_metadata (demuxer->player, demuxer->current->mrl);
    demuxer->current->updated = 1;

    demuxer->current = NULL;
    demuxer->busy = 0;
    pthread_testcancel ();  
  }

  printf ("th_demuxer has ended\n");
  
  return NULL;
}

void
av_demux_mrl (item_t *item)
{
  if (omc->demuxer->busy)
    return; /* thread is already demuxing: postponed */

  if (item->updated)
    return; /* already has been demuxed */
  
  omc->demuxer->busy = 1;
  printf ("file to demux: %s\n", item->mrl->name);
  omc->demuxer->current = item;
  pthread_cond_signal (&omc->demuxer->cond);
}

demuxer_t *
av_demuxer_init (player_type_t type)
{
  demuxer_t *demuxer;
  
  /* start tha A/V demuxer */
  demuxer = malloc (sizeof (demuxer_t));
  demuxer->player = player_init (type, NULL, NULL, NULL);
  demuxer->busy = 0;
  demuxer->current = NULL;

  /* create the demuxing thread */
  pthread_cond_init (&demuxer->cond, NULL);
  pthread_mutex_init (&demuxer->mutex, NULL);
  pthread_create (&demuxer->th, NULL, th_demuxer, (void *) demuxer);
  pthread_detach (demuxer->th);
  
  return demuxer;
}

void
av_demuxer_uninit (demuxer_t *demuxer)
{
  if (!demuxer)
    return;

  pthread_cond_destroy (&demuxer->cond);
  pthread_mutex_destroy (&demuxer->mutex);
  pthread_cancel (demuxer->th);
  if (demuxer->player)
    player_uninit (demuxer->player);
  free (demuxer);
}
