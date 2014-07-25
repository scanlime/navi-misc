/*
 * displaylist.h - A drawable that stores itself to a display list before rendering
 *
 * BZEdit
 * Copyright (C) 2004 David Trowbridge
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 *
 */

#include "drawable.h"

#ifndef __DISPLAY_LIST_H__
#define __DISPLAY_LIST_H__

G_BEGIN_DECLS

#define DISPLAY_LIST_TYPE            (display_list_get_type ())
#define DISPLAY_LIST(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), DISPLAY_LIST_TYPE, DisplayList))
#define DISPLAY_LIST_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), DISPLAY_LIST_TYPE, DisplayListClass))
#define IS_DISPLAY_LIST(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), DISPLAY_LIST_TYPE))
#define IS_DISPLAY_LIST_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), DISPLAY_LIST_TYPE))

typedef struct _DisplayList      DisplayList;
typedef struct _DisplayListClass DisplayListClass;

struct _DisplayList
{
  Drawable drawable;

  GLuint list;
  gboolean dirty;
};

struct _DisplayListClass
{
  DrawableClass parent_class;

  /* should probably pass rendering state to these */
  void (*draw_to_list) (DisplayList *display_list);
};

GType display_list_get_type   (void) G_GNUC_CONST;
void  display_list_build_list (DisplayList *display_list);
void  display_list_draw       (Drawable *drawable, RenderState *rstate);

G_END_DECLS

#endif
