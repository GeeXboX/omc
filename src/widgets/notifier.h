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

#ifndef _WIDGET_NOTIFIER_H_
#define _WIDGET_NOTIFIER_H_

typedef struct notifier_s {
  Ecore_Timer *timer;
  Evas_List *border;
  Evas_Object *cover;
  Evas_Object *infos;
  int show;
  int padding;
} notifier_t;

notifier_t *
notifier_new (omc_t *omc, font_t *font,
              char *x, char *y, char *w, char *h);
void notifier_free (notifier_t *notifier);

void notifier_update (notifier_t *notifier, char *cover, char *infos);
void notifier_show (notifier_t *notifier);
void notifier_hide (notifier_t *notifier);

#endif /* _WIDGET_NOTIFIER_H_ */
