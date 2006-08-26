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

#ifndef _SCREEN_AUDIO_PLAYER_H_
#define _SCREEN_AUDIO_PLAYER_H_

#define SCREEN_APLAYER_TITLE "aplayer"

void screen_aplayer_setup (struct screen_t *screen, char *data);
void screen_aplayer_display (struct screen_t *screen);
void screen_aplayer_free (struct screen_t *screen);

void screen_aplayer_update_infos (struct screen_t *screen,
                                  char *infos, int display);
void screen_aplayer_update_cover (struct screen_t *screen,
                                  char *img, int display);
void screen_aplayer_update_browser (struct screen_t *screen, char *mrl);

#endif /* _SCREEN_AUDIO_PLAYER_H_ */
