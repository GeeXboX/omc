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
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <Evas.h>
#include <Ecore.h>
#include <Ecore_Evas.h>
#include <player.h>

#include "screen.h"
#include "omc.h"
#include "filter.h"
#include "widgets/widget.h"
#include "gfx.h"
#include "avplayer.h"
#include "screen_image.h"
#include "screen_viewer.h"

#define SCREEN_IMG_BACKGROUND OMC_DATA_DIR"/back_brushed3.png"

#define IMG_ICON_EXIT OMC_DATA_DIR"/viewer_exit.png"
#define IMG_ICON_EXIT_FOCUS OMC_DATA_DIR"/viewer_exit_focus.png"

#define IMG_ICON_ROTATE OMC_DATA_DIR"/viewer_rotate.png"
#define IMG_ICON_ROTATE_FOCUS OMC_DATA_DIR"/viewer_rotate_focus.png"

#define IMG_ICON_PREV OMC_DATA_DIR"/viewer_prev.png"
#define IMG_ICON_PREV_FOCUS OMC_DATA_DIR"/viewer_prev_focus.png"
#define IMG_ICON_NEXT OMC_DATA_DIR"/viewer_next.png"
#define IMG_ICON_NEXT_FOCUS OMC_DATA_DIR"/viewer_next_focus.png"

#define IMG_ICON_SLIDESHOW OMC_DATA_DIR"/viewer_slideshow.png"
#define IMG_ICON_SLIDESHOW_FOCUS OMC_DATA_DIR"/viewer_slideshow_focus.png"

#define IMG_ICON_MOVE_DOWN OMC_DATA_DIR"/viewer_move_down.png"
#define IMG_ICON_MOVE_DOWN_FOCUS OMC_DATA_DIR"/viewer_move_down_focus.png"
#define IMG_ICON_MOVE_LEFT OMC_DATA_DIR"/viewer_move_left.png"
#define IMG_ICON_MOVE_LEFT_FOCUS OMC_DATA_DIR"/viewer_move_left_focus.png"
#define IMG_ICON_MOVE_RIGHT OMC_DATA_DIR"/viewer_move_right.png"
#define IMG_ICON_MOVE_RIGHT_FOCUS OMC_DATA_DIR"/viewer_move_right_focus.png"
#define IMG_ICON_MOVE_UP OMC_DATA_DIR"/viewer_move_up.png"
#define IMG_ICON_MOVE_UP_FOCUS OMC_DATA_DIR"/viewer_move_up_focus.png"

#define IMG_ICON_ZOOM_FS OMC_DATA_DIR"/viewer_zoom_fs.png"
#define IMG_ICON_ZOOM_FS_FOCUS OMC_DATA_DIR"/viewer_zoom_fs_focus.png"
#define IMG_ICON_ZOOM_1_1 OMC_DATA_DIR"/viewer_1_1.png"
#define IMG_ICON_ZOOM_1_1_FOCUS OMC_DATA_DIR"/viewer_1_1_focus.png"
#define IMG_ICON_ZOOM_IN OMC_DATA_DIR"/viewer_zoom_in.png"
#define IMG_ICON_ZOOM_IN_FOCUS OMC_DATA_DIR"/viewer_zoom_in_focus.png"
#define IMG_ICON_ZOOM_OUT OMC_DATA_DIR"/viewer_zoom_out.png"
#define IMG_ICON_ZOOM_OUT_FOCUS OMC_DATA_DIR"/viewer_zoom_out_focus.png"

#define IMG_ICON_BLUR OMC_DATA_DIR"/viewer_blur.png"
#define IMG_ICON_BLUR_FOCUS OMC_DATA_DIR"/viewer_blur_focus.png"
#define IMG_ICON_SHARPEN OMC_DATA_DIR"/viewer_sharpen.png"
#define IMG_ICON_SHARPEN_FOCUS OMC_DATA_DIR"/viewer_sharpen_focus.png"

typedef struct img_toolbar_s {
  Evas_List *border;
  Evas_List *items;
} img_toolbar_t;

static img_toolbar_t *
img_toolbar_new (void)
{
  img_toolbar_t *tb = NULL;

  tb = (img_toolbar_t *) malloc (sizeof (img_toolbar_t));
  tb->border = NULL;
  tb->items = NULL;
  
  return tb;
}

static void
img_toolbar_free (img_toolbar_t *tb)
{
  Evas_List *list;
  
  if (!tb)
    return;

  if (tb->border)
  {
    for (list = tb->border; list; list = list->next)
    {
      Evas_Object *obj;
    
      obj = (Evas_Object *) list->data;
      evas_object_del (obj);
      tb->border = evas_list_remove_list (tb->border, tb->border);
    }
    free (tb->border);
  }

  if (tb->items)
  {
    for (list = tb->items; list; list = list->next)
    {
      Evas_Object *obj;
    
      obj = (Evas_Object *) list->data;
      evas_object_del (obj);
      tb->items = evas_list_remove_list (tb->items, tb->items);
    }
    free (tb->border);
  }

  free (tb);
}

static void
img_toolbar_update (screen_t *screen,
                    img_toolbar_t *tb, int display)
{
  if (!screen || screen->type != SCREEN_TYPE_VIEWER || !tb)
    return;

  if (display)
  {
    Evas_List *list;

    if (tb->border)
    {
      for (list = tb->border; list; list = list->next)
      {
        Evas_Object *obj;
        
        obj = (Evas_Object *) list->data;
        evas_object_show (obj);
      }
    }

    if (tb->items)
    {
      for (list = tb->items; list; list = list->next)
      {
        Evas_Object *obj;
        
        obj = (Evas_Object *) list->data;
        evas_object_show (obj);
      }
    }
  }
  else /* hide toolbar */
  {
    Evas_List *list;

    if (tb->border)
    {
      for (list = tb->border; list; list = list->next)
      {
        Evas_Object *obj;
        
        obj = (Evas_Object *) list->data;
        evas_object_hide (obj);
      }
    }

    if (tb->items)
    {
      for (list = tb->items; list; list = list->next)
      {
        Evas_Object *obj;
        
        obj = (Evas_Object *) list->data;
        evas_object_hide (obj);
      }
    }
  }
}

typedef struct viewer_s {
  Evas_Object *img;
  Evas_List *dirlist;
  Evas_List *pos; /* current position of file in dirlist */
  img_toolbar_t *toolbar;
  int slideshow;
} viewer_t;

static viewer_t *
viewer_new (void)
{
  viewer_t *viewer = NULL;

  viewer = (viewer_t *) malloc (sizeof (viewer_t));
  viewer->img = NULL;
  viewer->dirlist = NULL;
  viewer->pos = NULL;
  viewer->toolbar = NULL;
  viewer->slideshow = 0;
  
  return viewer;
}

static void
viewer_free (viewer_t *viewer)
{ 
  if (!viewer)
    return;

  if (viewer->img)
    evas_object_del (viewer->img);

  if (viewer->dirlist)
  {
    Evas_List *list;
    for (list = viewer->dirlist; list; list = list->next)
    {
      char *item;
    
      item = (char *) list->data;
      free (item);
      viewer->dirlist = evas_list_remove_list (viewer->dirlist,
                                               viewer->dirlist);
    }
    free (viewer->dirlist);
  }

  if (viewer->toolbar)
    img_toolbar_free (viewer->toolbar);
  
  free (viewer);
}

static void
viewer_background_setup (screen_t *screen)
{
  Evas_Object *obj = NULL;
  
  obj = image_new (omc, 0, SCREEN_IMG_BACKGROUND,
                   NULL, 0, "0", "0", "100%", "100%");
  screen->objects = evas_list_append (screen->objects, obj);
}

static Evas_Object *
load_image (Evas_List *pos)
{
  Evas_Object *img = NULL;
  char *mrl = NULL;
  int w, h;
  
  if (!pos)
    return NULL;

  mrl = (char *) pos->data;
  if (!mrl)
    return NULL;

  img = image_new (omc, 0, mrl, NULL, 1, "0", "0", "0", "0");
  evas_object_image_size_get (img, &w, &h);
  evas_object_resize (img, w, h);
  evas_object_image_fill_set (img, 0, 0, w, h);
  image_fit_to_screen (img);
  image_center (img);

  return img;
}

static void
cb_viewer_rotate (void *data, Evas *e, Evas_Object *obj, void *event_info)
{
  Evas_Object *img = NULL;
  viewer_t *viewer = NULL;

  viewer = (viewer_t *) data;
  if (!viewer)
    return;
  
  img = viewer->img;
  if (!img)
    return;
  
  image_rotate (img, ROTATE_CLOCKWISE);
}

static void
cb_viewer_prev (void *data, Evas *e, Evas_Object *obj, void *event_info)
{
  viewer_t *viewer = NULL;
  Evas_List *pos;
  
  viewer = (viewer_t *) data;
  if (!viewer)
    return;

  pos = viewer->pos;
  if (!pos)
    return;

  if (pos->prev)
  {
    if (viewer->img)
      evas_object_del ((Evas_Object *) viewer->img);
    
    viewer->img = load_image (pos->prev);
    viewer->pos = pos->prev;

    if (viewer->img)
      evas_object_show (viewer->img);
  }
}

static void
cb_viewer_next (void *data, Evas *e, Evas_Object *obj, void *event_info)
{
  viewer_t *viewer = NULL;
  Evas_List *pos;
  
  viewer = (viewer_t *) data;
  if (!viewer)
    return;

  pos = viewer->pos;
  if (!pos)
    return;

  if (pos->next)
  {
    if (viewer->img)
      evas_object_del ((Evas_Object *) viewer->img);
    
    viewer->img = load_image (pos->next);
    viewer->pos = pos->next;

    if (viewer->img)
      evas_object_show (viewer->img);
  }
}

static void
cb_viewer_slideshow (void *data, Evas *e, Evas_Object *obj, void *event_info)
{
  viewer_t *viewer = NULL;

  viewer = (viewer_t *) data;
  if (!viewer)
    return;

  printf ("Viewer Slideshow\n");
}

static void
cb_viewer_move_down (void *data, Evas *e, Evas_Object *obj, void *event_info)
{
  Evas_Object *img = NULL;
  viewer_t *viewer = NULL;

  viewer = (viewer_t *) data;
  if (!viewer)
    return;
  
  img = viewer->img;
  if (!img)
    return;

  image_move (img, 0, 50);
}

static void
cb_viewer_move_up (void *data, Evas *e, Evas_Object *obj, void *event_info)
{
  Evas_Object *img = NULL;
  viewer_t *viewer = NULL;

  viewer = (viewer_t *) data;
  if (!viewer)
    return;
  
  img = viewer->img;
  if (!img)
    return;

  image_move (img, 0, -50);
}

static void
cb_viewer_move_left (void *data, Evas *e, Evas_Object *obj, void *event_info)
{
  Evas_Object *img = NULL;
  viewer_t *viewer = NULL;

  viewer = (viewer_t *) data;
  if (!viewer)
    return;
  
  img = viewer->img;
  if (!img)
    return;

  image_move (img, -50, 0);
}

static void
cb_viewer_move_right (void *data, Evas *e, Evas_Object *obj, void *event_info)
{
  Evas_Object *img = NULL;
  viewer_t *viewer = NULL;

  viewer = (viewer_t *) data;
  if (!viewer)
    return;
  
  img = viewer->img;
  if (!img)
    return;

  image_move (img, 50, 0);
}

static void
cb_viewer_zoom_fs (void *data, Evas *e, Evas_Object *obj, void *event_info)
{
  Evas_Object *img = NULL;
  viewer_t *viewer = NULL;

  viewer = (viewer_t *) data;
  if (!viewer)
    return;
  
  img = viewer->img;
  if (!img)
    return;

  image_fit_to_screen (img);
}

static void
cb_viewer_zoom_1_1 (void *data, Evas *e, Evas_Object *obj, void *event_info)
{
  Evas_Object *img = NULL;
  viewer_t *viewer = NULL;

  viewer = (viewer_t *) data;
  if (!viewer)
    return;
  
  img = viewer->img;
  if (!img)
    return;

  image_zoom_1_1 (img);
}

static void
cb_viewer_zoom_in (void *data, Evas *e, Evas_Object *obj, void *event_info)
{
  Evas_Object *img = NULL;
  viewer_t *viewer = NULL;

  viewer = (viewer_t *) data;
  if (!viewer)
    return;
  
  img = viewer->img;
  if (!img)
    return;

  image_zoom_in (img);
}

static void
cb_viewer_zoom_out (void *data, Evas *e, Evas_Object *obj, void *event_info)
{
  Evas_Object *img = NULL;
  viewer_t *viewer = NULL;

  viewer = (viewer_t *) data;
  if (!viewer)
    return;
  
  img = viewer->img;
  if (!img)
    return;

  image_zoom_out (img);
}

static void
cb_viewer_blur (void *data, Evas *e, Evas_Object *obj, void *event_info)
{
  Evas_Object *img = NULL;
  viewer_t *viewer = NULL;

  viewer = (viewer_t *) data;
  if (!viewer)
    return;
  
  img = viewer->img;
  if (!img)
    return;

  image_blur (img);
}

static void
cb_viewer_sharpen (void *data, Evas *e, Evas_Object *obj, void *event_info)
{
  Evas_Object *img = NULL;
  viewer_t *viewer = NULL;

  viewer = (viewer_t *) data;
  if (!viewer)
    return;
  
  img = viewer->img;
  if (!img)
    return;

  image_sharpen (img);
}

static void
viewer_toolbar_setup (screen_t *screen)
{
  viewer_t *viewer = NULL;
  img_toolbar_t *tb = NULL;
  Evas_Object *dummy = NULL;
  Evas_Object *obj = NULL;
  
  if (!screen)
    return;
  
  viewer = (viewer_t *) screen->private;
  if (!viewer)
    return;
  
  viewer->toolbar = img_toolbar_new ();
  tb = viewer->toolbar;
  
  dummy = evas_object_image_add (omc->evas);
  tb->border = evas_list_append (tb->border, dummy);

  border_new (omc, tb->border,
              BORDER_TYPE_COVER, "5%", "88%", "90%", "12%");

  /* back button */
  obj = image_new (omc, 1, IMG_ICON_EXIT, IMG_ICON_EXIT_FOCUS,
                   5, "5%", "90%", "9%", "9%");
  tb->items = evas_list_append (tb->items, obj);
  object_add_default_cb (obj);
  evas_object_event_callback_add (obj, EVAS_CALLBACK_MOUSE_DOWN,
                                  cb_switch_screen, SCREEN_IMAGE_TITLE);

  /* image control buttons */
  obj = image_new (omc, 1, IMG_ICON_ROTATE, IMG_ICON_ROTATE_FOCUS,
                   5, "14%", "90%", "8%", "8%");
  tb->items = evas_list_append (tb->items, obj);
  object_add_default_cb (obj);
  evas_object_event_callback_add (obj, EVAS_CALLBACK_MOUSE_DOWN,
                                  cb_viewer_rotate, viewer);
 
  obj = image_new (omc, 1, IMG_ICON_PREV, IMG_ICON_PREV_FOCUS,
                   5, "22%", "90%", "7%", "7%");
  tb->items = evas_list_append (tb->items, obj);
  object_add_default_cb (obj);
  evas_object_event_callback_add (obj, EVAS_CALLBACK_MOUSE_DOWN,
                                  cb_viewer_prev, viewer);

  obj = image_new (omc, 1, IMG_ICON_NEXT, IMG_ICON_NEXT_FOCUS,
                   5, "27%", "90%", "7%", "7%");
  tb->items = evas_list_append (tb->items, obj);
  object_add_default_cb (obj);
  evas_object_event_callback_add (obj, EVAS_CALLBACK_MOUSE_DOWN,
                                  cb_viewer_next, viewer);

  obj = image_new (omc, 1, IMG_ICON_SLIDESHOW, IMG_ICON_SLIDESHOW_FOCUS,
                   5, "34%", "90%", "7%", "7%");
  tb->items = evas_list_append (tb->items, obj);
  object_add_default_cb (obj);
  evas_object_event_callback_add (obj, EVAS_CALLBACK_MOUSE_DOWN,
                                  cb_viewer_slideshow, viewer);

  /* position arrays */
  obj = image_new (omc, 1, IMG_ICON_MOVE_DOWN, IMG_ICON_MOVE_DOWN_FOCUS,
                   5, "47%", "94%", "6%", "6%");
  tb->items = evas_list_append (tb->items, obj);
  object_add_default_cb (obj);
  evas_object_event_callback_add (obj, EVAS_CALLBACK_MOUSE_DOWN,
                                  cb_viewer_move_down, viewer);
  
  obj = image_new (omc, 1, IMG_ICON_MOVE_LEFT, IMG_ICON_MOVE_LEFT_FOCUS,
                   5, "42%", "90%", "6%", "6%");
  tb->items = evas_list_append (tb->items, obj);
  object_add_default_cb (obj);
  evas_object_event_callback_add (obj, EVAS_CALLBACK_MOUSE_DOWN,
                                  cb_viewer_move_left, viewer);

  obj = image_new (omc, 1, IMG_ICON_MOVE_RIGHT, IMG_ICON_MOVE_RIGHT_FOCUS,
                   5, "52%", "90%", "6%", "6%");
  tb->items = evas_list_append (tb->items, obj);
  object_add_default_cb (obj);
  evas_object_event_callback_add (obj, EVAS_CALLBACK_MOUSE_DOWN,
                                  cb_viewer_move_right, viewer);

  obj = image_new (omc, 1, IMG_ICON_MOVE_UP, IMG_ICON_MOVE_UP_FOCUS,
                   5, "47%", "86%", "6%", "6%");
  tb->items = evas_list_append (tb->items, obj);
  object_add_default_cb (obj);
  evas_object_event_callback_add (obj, EVAS_CALLBACK_MOUSE_DOWN,
                                  cb_viewer_move_up, viewer);

  /* image properties buttons */
  obj = image_new (omc, 1, IMG_ICON_ZOOM_FS, IMG_ICON_ZOOM_FS_FOCUS,
                   5, "59%", "89%", "5%", "5%");
  tb->items = evas_list_append (tb->items, obj);
  object_add_default_cb (obj);
  evas_object_event_callback_add (obj, EVAS_CALLBACK_MOUSE_DOWN,
                                  cb_viewer_zoom_fs, viewer);

  obj = image_new (omc, 1, IMG_ICON_ZOOM_1_1, IMG_ICON_ZOOM_1_1_FOCUS,
                   5, "65%", "94%", "5%", "5%");
  tb->items = evas_list_append (tb->items, obj);
  object_add_default_cb (obj);
  evas_object_event_callback_add (obj, EVAS_CALLBACK_MOUSE_DOWN,
                                  cb_viewer_zoom_1_1, viewer);

  obj = image_new (omc, 1, IMG_ICON_ZOOM_IN, IMG_ICON_ZOOM_IN_FOCUS,
                   5, "69%", "89%", "5%", "5%");
  tb->items = evas_list_append (tb->items, obj);
  object_add_default_cb (obj);
  evas_object_event_callback_add (obj, EVAS_CALLBACK_MOUSE_DOWN,
                                  cb_viewer_zoom_in, viewer);

  obj = image_new (omc, 1, IMG_ICON_ZOOM_OUT, IMG_ICON_ZOOM_OUT_FOCUS,
                   5, "75%", "94%", "5%", "5%");
  tb->items = evas_list_append (tb->items, obj);
  object_add_default_cb (obj);
  evas_object_event_callback_add (obj, EVAS_CALLBACK_MOUSE_DOWN,
                                  cb_viewer_zoom_out, viewer);

  obj = image_new (omc, 1, IMG_ICON_BLUR, IMG_ICON_BLUR_FOCUS,
                   5, "81%", "91%", "6%", "6%");
  tb->items = evas_list_append (tb->items, obj);
  object_add_default_cb (obj);
  evas_object_event_callback_add (obj, EVAS_CALLBACK_MOUSE_DOWN,
                                  cb_viewer_blur, viewer);

  obj = image_new (omc, 1, IMG_ICON_SHARPEN, IMG_ICON_SHARPEN_FOCUS,
                   5, "88%", "91%", "6%", "6%");
  tb->items = evas_list_append (tb->items, obj);
  object_add_default_cb (obj);
  evas_object_event_callback_add (obj, EVAS_CALLBACK_MOUSE_DOWN,
                                  cb_viewer_sharpen, viewer);

}

static Evas_List *
setup_dirlist (void)
{
  Evas_List *dirlist = NULL;
  struct dirent **namelist;
  filter_t *filter = NULL;
  int i, n;

  /* create the directory image list */
  n = scandir (omc->cwd, &namelist, 0, alphasort);
  if (n < 0)
  {
    perror ("scandir");
    return NULL;
  }

  filter = filter_get (omc->filters, FILTER_TYPE_IMAGE);
  if (!filter)
    return NULL;
  
  for (i = 0; i < n; i++)
  {
    struct stat st;
    char *path = NULL;
    size_t len;
 
    /* do not consider hidden files */
    if ((namelist[i]->d_name[0] == '.' && namelist[i]->d_name[1] != '.'))
    {
      free (namelist[i]);
      continue;
    }
      
    if (!strcmp (namelist[i]->d_name, "..") && !strcmp (omc->cwd, "/"))
      continue;
      
    len = strlen (omc->cwd) + 1 + strlen (namelist[i]->d_name) + 1;
    path = (char *) malloc (len);
    if (!strcmp (omc->cwd, "/"))
      sprintf (path, "/%s", namelist[i]->d_name);
    else
      sprintf (path, "%s/%s", omc->cwd, namelist[i]->d_name);
      
    stat (path, &st);

    /* only consider regular files */
    if (S_ISREG (st.st_mode))
    {
      char *ext = NULL;
        
      ext = getExtension (namelist[i]->d_name);
      if (!ext)
      {
        free (path);
        free (namelist[i]);
        continue;
      }
        
      if (!filter_supports_extension (filter, ext))
      {
        free (ext);
        free (path);
        free (namelist[i]);
        continue;
      }

      free (ext);
      dirlist = evas_list_append (dirlist, strdup (path));
    }

    free (path);
    free (namelist[i]);
  }

  return dirlist;
}

static Evas_List *
get_pos_in_list (Evas_List *dirlist, char *mrl)
{
  Evas_List *pos;
  int i = 0;

  if (!dirlist || !mrl)
    return NULL;

  for (pos = dirlist; pos; pos = pos->next)
  {
    char *item;

    i++;
    item = (char *) pos->data;
    if (!strcmp (item, mrl))
      return pos;
  }

  return NULL;
}

void
screen_viewer_setup (screen_t *screen, char *mrl)
{
  viewer_t *viewer = NULL;
  
  if (!screen || screen->type != SCREEN_TYPE_VIEWER || !mrl)
    return;

  screen->private = (viewer_t *) viewer_new ();
  viewer = (viewer_t *) screen->private;
  if (!viewer)
    return;

  viewer_background_setup (screen);
  viewer->dirlist = setup_dirlist ();
  viewer->pos = get_pos_in_list (viewer->dirlist, mrl);
  
  /* create the main image */
  viewer->img = load_image (viewer->pos);
  
  viewer_toolbar_setup (screen);
}

void
screen_viewer_display (screen_t *screen)
{
  viewer_t *viewer = NULL;

  viewer = (viewer_t *) screen->private;
  if (!viewer)
    return;

  /* display image */
  if (viewer->img)
    evas_object_show (viewer->img);

  /* display toolbar */
  if (viewer->toolbar)
    img_toolbar_update (screen, viewer->toolbar, 1);
}

void
screen_viewer_free (screen_t *screen)
{
  viewer_t *viewer = NULL;
  
  if (!screen || screen->type != SCREEN_TYPE_VIEWER)
    return;

  viewer = (viewer_t *) screen->private;
  if (viewer)
    viewer_free (viewer);
}
