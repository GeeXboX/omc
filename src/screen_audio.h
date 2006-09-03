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

#ifndef _SCREEN_AUDIO_H_
#define _SCREEN_AUDIO_H_

#define SCREEN_AUDIO_TITLE "audio"
#include "screen.h"
#include "widget.h"

typedef struct audio_s {
  Evas_Object *infobox;
  cover_t *cover;
  browser_t *browser;
  notifier_t *notifier;
} audio_t;

void screen_audio_setup (screen_t *screen, char *data);
void screen_audio_display (screen_t *screen);
void screen_audio_free (screen_t *screen);

void screen_audio_update_infos (screen_t *screen,
                                char *infos, int display);
void screen_audio_update_cover (screen_t *screen,
                                char *img, int display);
void screen_audio_update_notifier (screen_t *screen,
                                   char *cover, char *infos);

#endif /* _SCREEN_AUDIO_H_ */
