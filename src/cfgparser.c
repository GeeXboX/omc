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

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

#include "omc.h"
#include "cfgparser.h"

#define CONFIG_FILE_NAME "omc.conf"

static struct font_t *
font_new (char *id, char *ft, int size,
          int style, char *color, char *fcolor)
{
  struct font_t *font = NULL;
  
  if (!id || !ft || !color)
    return NULL;

  font = (struct font_t *) malloc (sizeof (struct font_t));
  font->id = strdup (id);
  font->ft = strdup (ft);
  font->size = size;
  font->style = style;
  font->color = strdup (color);
  font->fcolor = fcolor ? strdup (fcolor) : strdup (color);
  
  return font;
}

static void
font_free (struct font_t *font)
{
  if (!font)
    return;

  if (font->id)
    free (font->id);
  if (font->ft)
    free (font->ft);
  if (font->color)
    free (font->color);
  if (font->fcolor)
    free (font->fcolor);
  free (font);
}

struct font_t *
get_font (Evas_List *list, char *id)
{
  Evas_List *l;

  if (!id)
    return NULL;

  for (l = list; l; l = l->next)
  {
    struct font_t *font = NULL;
    font = (struct font_t *) l->data;

    if (font->id && !strcmp (font->id, id))
      return font;
  }

  return NULL;
}


struct config_t *
config_new (void)
{
  struct config_t *cfg = NULL;

  cfg = (struct config_t *) malloc (sizeof (struct config_t));
  cfg->media_dir = NULL;
  cfg->fonts = NULL;
  
  return cfg;
}

void
config_free (struct config_t *cfg)
{
  Evas_List *l;
  
  if (!cfg)
    return;

  if (cfg->media_dir)
    free (cfg->media_dir);

  for (l = cfg->fonts; l; l = l->next)
  {
    struct font_t *font = NULL;
    font = (struct font_t *) l->data;
    font_free (font);
  }
  free (cfg->fonts);

  free (cfg);
}

static char *
get_config_file (void)
{
  char path[1024];
  char *home = NULL;
  struct stat st;
  
  home = getenv ("HOME");
  if (home)
  {
    memset (path, '\0', 1024);
    sprintf (path, "%s/.%s", home, CONFIG_FILE_NAME);
    if ((stat (path, &st) == 0) && S_ISREG (st.st_mode))
      return strdup (path);
  }

  /* no config file in user directory : try to a more generic path */
  memset (path, '\0', 1024);
  sprintf (path, "%s/%s", CFG_DIR, CONFIG_FILE_NAME);
  if ((stat (path, &st) == 0) && S_ISREG (st.st_mode))
    return strdup (path);

  /* no config file can be found : drop it */
  return NULL;
}

static char *
xstrdup (char *old, char *new)
{
  if (!new)
    return NULL;

  if (old)
    free (old);

  return strdup (new);
}

static void
parse_font (struct config_t *cfg, char *line)
{
  struct font_t *font = NULL;
  char *id = NULL, *ttf = NULL, *size = NULL;
  char *style = NULL, *color = NULL, *fcolor = NULL;
  char *pos;
  int len = 0;
  
  pos = strchr (line, ':');
  if (!pos)
    return;
  id = strndup (line, strlen (line) - strlen (pos));
  len += strlen (id) + 1;

  pos = strchr (pos + 1, ':');
  if (!pos)
    return;
  ttf = strndup (line + len, strlen (line) - len - strlen (pos));
  len += strlen (ttf) + 1;

  pos = strchr (pos + 1, ':');
  if (!pos)
    return;
  size = strndup (line + len, strlen (line) - len - strlen (pos));
  len += strlen (size) + 1;

  pos = strchr (pos + 1, ':');
  if (!pos)
    return;
  style = strndup (line + len, strlen (line) - len - strlen (pos));
  len += strlen (style) + 1;

  pos = strchr (pos + 1, ':');
  if (!pos)
    return;
  color = strndup (line + len, strlen (line) - len - strlen (pos));
  len += strlen (color) + 1;

  fcolor = strndup (line + len, strlen (line) - len);
  len += strlen (fcolor) + 1;

  font = font_new (id, ttf, atoi (size), atoi (style), color, fcolor);
  if (!font)
    return;

  cfg->fonts = evas_list_append (cfg->fonts, font);
}

#define TAG_MEDIA_DIR "media_dir"
#define TAG_SCREEN_WIDTH "screenw"
#define TAG_SCREEN_HEIGHT "screenh"
#define TAG_FONT "font"

static void
parse_line (struct config_t *cfg, char *line)
{
  char *pos, *tag, *val;
  
  if (!cfg || !line)
    return;
  
  /* do not consider commentaries */
  if (line[0] == '#')
    return;

  pos = strchr (line, '=');
  if (!pos)
    return;

  tag = strndup (line, strlen (line) - strlen (pos));
  val = strdup (line + strlen (tag) + 1);

  if (!strcmp (tag, TAG_MEDIA_DIR))
    cfg->media_dir = xstrdup (cfg->media_dir, val);
  else if (!strcmp (tag, TAG_SCREEN_WIDTH))
    cfg->screenw = xstrdup (cfg->screenw, val);
  else if (!strcmp (tag, TAG_SCREEN_HEIGHT))
    cfg->screenh = xstrdup (cfg->screenh, val);
  else if (!strcmp (tag, TAG_FONT))
    parse_font (cfg, val);

  free (tag);
  free (val);
}

static void
parse_buffer (struct config_t *cfg, char *buffer)
{
  char *line;
  char *delimiter;

  /* check for CR/LF to find the right delimiter to use */
  if (strstr (buffer, "\r\n"))
    delimiter = "\r\n";
  else
    delimiter = "\n";
  
  line = strtok (buffer, delimiter);
  while (line)
  {
    parse_line (cfg, line);
    line = strtok (NULL, delimiter);
  }
}

struct config_t *
parse_config (void)
{
  struct config_t *cfg = NULL;
  char *buffer = NULL, *file = NULL;
  struct stat st;
  int fd;
  
  file = get_config_file ();
  printf ("Configuration File : %s\n", file);
  cfg = config_new ();

  stat (file, &st);
  fd = open (file, O_RDONLY);
  if (fd < 0)
    return NULL;

  buffer = (char *) malloc ((st.st_size + 1) * sizeof (char));
  memset (buffer, '\0', (st.st_size + 1));
  read (fd, buffer, st.st_size);
  parse_buffer (cfg, buffer);
  close (fd);
 
  return cfg;
}
