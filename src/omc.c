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

#include <Evas.h>
#include <Ecore.h>
#include <Ecore_Evas.h>

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "omc.h"
#include "screen.h"
#include "screen_main.h"
#include "player.h"
#include "filter.h"
#include "cfgparser.h"

static int
main_signal_exit (void *data, int ev_type, void *ev)
{
  ecore_main_loop_quit ();

  return 1;
}

static void
main_delete_request (Ecore_Evas *ee)
{
  ecore_main_loop_quit ();
}

static int
main_start(int argc, char **argv)
{
   if (!ecore_init())
     return -1;

   ecore_app_args_set (argc, (const char **) argv);
   ecore_event_handler_add (ECORE_EVENT_SIGNAL_EXIT, main_signal_exit, NULL);
   if (!ecore_evas_init ())
     return -1;

   omc->ee = ecore_evas_software_x11_new (NULL, 0,  0, 0, omc->w, omc->h);
   if (!omc->ee)
     return -1;

   ecore_evas_callback_delete_request_set (omc->ee, main_delete_request);
   ecore_evas_title_set (omc->ee, OMC_WM_TITLE);
   ecore_evas_name_class_set (omc->ee, "geexbox_omc", "main");
   ecore_evas_size_min_set (omc->ee, omc->w, omc->h);
   ecore_evas_size_max_set (omc->ee, omc->w, omc->h);
   ecore_evas_fullscreen_set (omc->ee, 0);
   ecore_evas_show (omc->ee);

   omc->evas = ecore_evas_get (omc->ee);
   evas_image_cache_set (omc->evas, 8 * 1024 * 1024);
   evas_font_cache_set (omc->evas, 1 * 1024 * 1024);
   evas_font_path_append (omc->evas, OMC_DATA_DIR);
   
   omc->player = player_init ();
   
   return 1;
}

static void
main_stop (void)
{
  ecore_evas_shutdown ();
  ecore_shutdown ();
}

void
omc_update_cwd (struct omc_t *omc, char *dir)
{
  if (!omc || !dir)
    return;

  if (omc->cwd)
    free (omc->cwd);
  omc->cwd = strdup (dir);
}

static struct omc_t *
omc_init (void)
{
  struct omc_t *o = NULL;
  struct filter_t *filter = NULL;
  
  o = (struct omc_t *) malloc (sizeof (struct omc_t));
  o->evas = NULL;
  o->ee = NULL;
  o->filters = NULL;
  o->cfg = NULL;
  o->cwd = NULL;
  o->screen = NULL;
  o->player = NULL;

  filter = filter_new (FILTER_TYPE_AUDIO);
  o->filters = evas_list_append (o->filters, filter);

  filter = filter_new (FILTER_TYPE_VIDEO);
  o->filters = evas_list_append (o->filters, filter);

  filter = filter_new (FILTER_TYPE_IMAGE);
  o->filters = evas_list_append (o->filters, filter);

  o->cfg = parse_config ();

  o->w = o->cfg->screenw ? atoi (o->cfg->screenw) : OMC_DEFAULT_WIDTH;
  o->h = o->cfg->screenh ? atoi (o->cfg->screenh) : OMC_DEFAULT_HEIGHT;
  
  omc_update_cwd (o, o->cfg->media_dir);
  
  return o;
}

void
omc_uninit (struct omc_t *o)
{
  if (!o)
    return;

  if (o->evas)
    evas_free (o->evas);

  if (o->filters)
  {
    Evas_List *l;
    for (l = o->filters; l; l = l->next)
    {
      struct filter_t *filter = NULL;
      filter = (struct filter_t *) l->data;
      if (filter)
        filter_free (filter);
    }
    free (o->filters);
  }
  
  if (o->cfg)
    config_free (o->cfg);
  if (o->cwd)
    free (o->cwd);

  if (o->screen)
    screen_uninit (o->screen);
  if (o->player)
    player_uninit (o->player);
  free (o);
}

int
main (int argc, char **argv)
{
  omc = omc_init ();
  if (main_start (argc, argv) < 1)
    return -1;

  screen_init (SCREEN_MAIN_TITLE);
  ecore_main_loop_begin ();
  main_stop ();
  omc_uninit (omc);
    
  return 0;
}
