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

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "screen.h"
#include "omc.h"
#include "cfgparser.h"
#include "widget.h"
#include "screen_main.h"
#include "screen_video.h"
#include "screen_audio.h"
#include "screen_tv.h"
#include "screen_image.h"
#include "screen_settings.h"
#include "lang_i18n.h"

typedef struct smain_s {
  menu_t *menu;
  notifier_t *notifier;
} smain_t;

static smain_t *
smain_new (void)
{
  smain_t *smain = NULL;

  smain = (smain_t *) malloc (sizeof (smain_t));
  smain->menu = NULL;
  smain->notifier = NULL;
  
  return smain;
}

static void
smain_free (smain_t *smain)
{
  if (!smain)
    return;

  if (smain->menu)
    menu_free (smain->menu);
  if (smain->notifier)
    notifier_free (smain->notifier);

  free (smain);
}

#define SCREEN_IMG_TOP_BORDER OMC_DATA_DIR"/top_border.png"
#define SCREEN_IMG_LOGO_GEEXBOX OMC_DATA_DIR"/logo_gx.png"

static void
main_headers_setup (screen_t *screen)
{
  Evas_Object *obj = NULL;
  font_t *font = NULL;

  font = get_font (omc->cfg->fonts, "header");
  if (!font)
    return;

  obj = image_new (omc, 0, SCREEN_IMG_TOP_BORDER,
                   NULL, 0, "0", "0", "100%", "76");
  screen->objects = evas_list_append (screen->objects, obj);
  
  obj = image_new (omc, 0, SCREEN_IMG_LOGO_GEEXBOX,
                   NULL, 0, "75%", "8", "190", "52");
  screen->objects = evas_list_append (screen->objects, obj);

  obj = text_new (omc, 0, font,
                  i18n_get_string (STR_APP_VERSION), 255, 0, "2%", "2%");
  screen->objects = evas_list_append (screen->objects, obj);
  
  obj = text_new (omc, 0, font,
                  i18n_get_string (STR_APP_COPYRIGHT), 255, 0, "2%", "6%");
  screen->objects = evas_list_append (screen->objects, obj);
}

#define IMG_MENU_SELECTOR OMC_DATA_DIR"/selector.png"
#define IMG_MENU_MOVIES OMC_DATA_DIR"/logo_mov.png"
#define IMG_MENU_AUDIO OMC_DATA_DIR"/logo_snd.png"
#define IMG_MENU_TV OMC_DATA_DIR"/logo_tv.png"
#define IMG_MENU_PICTURES OMC_DATA_DIR"/logo_img.png"
#define IMG_MENU_SETTINGS OMC_DATA_DIR"/logo_set.png"

static void
main_menu_setup (screen_t *screen)
{
  Evas_Object *item = NULL;
  smain_t *smain = NULL;
  font_t *font = NULL;

  if (!screen || screen->type != SCREEN_TYPE_MAIN)
    return;
  
  smain = (smain_t *) screen->private;
  if (!smain)
    return;

  font = get_font (omc->cfg->fonts, "menu");
  if (!font)
    return;
  
  smain->menu =
    menu_new (omc, font, MENU_ALIGN_RIGHT, IMG_MENU_SELECTOR, "50%", "50",
              "55%", "20%", "40%", "63%");

  item = menu_add_menuitem (omc, smain->menu,
                            i18n_get_string (STR_MENU_VIDEO_PLAYER),
                            IMG_MENU_MOVIES, "10%", "25%", "35%", "56%");
  evas_object_event_callback_add (item, EVAS_CALLBACK_MOUSE_DOWN,
                                  cb_switch_screen, SCREEN_VIDEO_TITLE);

  item = menu_add_menuitem (omc, smain->menu,
                            i18n_get_string (STR_MENU_AUDIO_PLAYER),
                            IMG_MENU_AUDIO, "11%", "25%", "30%", "56%");
  evas_object_event_callback_add (item, EVAS_CALLBACK_MOUSE_DOWN,
                                  cb_switch_screen, SCREEN_AUDIO_TITLE);

  item = menu_add_menuitem (omc, smain->menu,
                            i18n_get_string (STR_MENU_TV),
                            IMG_MENU_TV, "5%", "30%", "35%", "40%");
  evas_object_event_callback_add (item, EVAS_CALLBACK_MOUSE_DOWN,
                                  cb_switch_screen, SCREEN_TV_TITLE);

  item = menu_add_menuitem (omc, smain->menu,
                            i18n_get_string (STR_MENU_PICTURES),
                            IMG_MENU_PICTURES, "8%", "20%", "35%", "65%");
  evas_object_event_callback_add (item, EVAS_CALLBACK_MOUSE_DOWN,
                                  cb_switch_screen, SCREEN_IMAGE_TITLE);

  item = menu_add_menuitem (omc, smain->menu,
                            i18n_get_string (STR_MENU_SETTINGS),
                            IMG_MENU_SETTINGS, "8%", "35%", "35%", "40%");
  evas_object_event_callback_add (item, EVAS_CALLBACK_MOUSE_DOWN,
                                  cb_switch_screen, SCREEN_SETTINGS_TITLE);

  menu_compute_items (smain->menu);
}

#define IMG_ICON_QUIT OMC_DATA_DIR"/quit.png"
#define IMG_ICON_QUIT_FOCUS OMC_DATA_DIR"/quit_focus.png"
#define IMG_ICON_INFO OMC_DATA_DIR"/info.png"
#define IMG_ICON_INFO_FOCUS OMC_DATA_DIR"/info_focus.png"
#define IMG_ICON_HELP OMC_DATA_DIR"/help.png"
#define IMG_ICON_HELP_FOCUS OMC_DATA_DIR"/help_focus.png"

static void
cb_button_exit_clicked (void *data, Evas *e,
                        Evas_Object *obj, void *event_info)
{
  ecore_main_loop_quit ();
}

static void
main_buttons_setup (screen_t *screen)
{
  Evas_Object *obj = NULL;
  
  obj = image_new (omc, 1, IMG_ICON_HELP, IMG_ICON_HELP_FOCUS,
                   0, "70%", "89%", "48", "49");
  screen->objects = evas_list_append (screen->objects, obj);
  object_add_default_cb (obj);

  obj = image_new (omc, 1, IMG_ICON_INFO, IMG_ICON_INFO_FOCUS,
                   0, "80%", "89%", "48", "49");
  screen->objects = evas_list_append (screen->objects, obj);
  object_add_default_cb (obj);
  
  obj = image_new (omc, 1, IMG_ICON_QUIT, IMG_ICON_QUIT_FOCUS,
                   0, "90%", "89%", "48", "49");
  screen->objects = evas_list_append (screen->objects, obj);
  object_add_default_cb (obj);
  evas_object_event_callback_add (obj, EVAS_CALLBACK_MOUSE_DOWN,
                                  cb_button_exit_clicked, NULL);
}

void
screen_main_update_notifier (screen_t *screen,
                             char *cover, char *infos)
{
  smain_t *smain = NULL;
  notifier_t *notifier = NULL;
  
  if (!screen || screen->type != SCREEN_TYPE_MAIN)
    return;

  smain = (smain_t *) screen->private;
  if (!smain)
    return;

  notifier = smain->notifier;
  if (!notifier)
    return;

  notifier_update (notifier, cover, infos);
}

static void
main_notifier_setup (screen_t *screen)
{
  smain_t *smain = NULL;
  font_t *font = NULL;

  font = get_font (omc->cfg->fonts, "infos");
  if (!font)
    return;
  
  smain = (smain_t *) screen->private;
  if (!smain)
    return;

  smain->notifier = notifier_new (omc, font, "40%", "83%", "100%", "10%");
}

void
screen_main_setup (screen_t *screen, char *data)
{
  if (!screen || screen->type != SCREEN_TYPE_MAIN)
    return;

  screen->private = (smain_t *) smain_new ();
  
  widget_background_setup (screen);
  
  main_headers_setup (screen);
  main_menu_setup (screen);
  main_buttons_setup (screen);
  main_notifier_setup (screen);
}

void
screen_main_display (screen_t *screen)
{
  smain_t *smain = NULL;
  Evas_List *l;
  
  smain = (smain_t *) screen->private;
  if (!smain)
    return;
  
  if (smain->menu)
  {
    for (l = smain->menu->items; l; l = l->next)
    {
      Evas_Object *obj;
      
      obj = (Evas_Object *) l->data;
      evas_object_show (obj);
    }
  }
}

void
screen_main_free (screen_t *screen)
{
  smain_t *smain = NULL;
  
  if (!screen || screen->type != SCREEN_TYPE_MAIN)
    return;

  smain = (smain_t *) screen->private;
  if (smain)
    smain_free (smain);
}
