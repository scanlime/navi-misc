/*
 * box.h - Definition for the box object
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

#ifndef __BOX_H__
#define __BOX_H__

#include "sceneobject.h"
#include "displaylist.h"

G_BEGIN_DECLS

#define BOX_TYPE		(box_get_type ())
#define BOX(obj)		(G_TYPE_CHECK_INSTANCE_CAST ((obj), BOX_TYPE, Box))
#define BOX_CLASS(klass)	(G_TYPE_CHECK_CLASS_CAST ((klass), BOX_TYPE, BoxClass))
#define IS_BOX(obj)		(G_TYPE_CHECK_INSTANCE_TYPE ((obj), BOX_TYPE))
#define IS_BOX_CLASS(klass)	(G_TYPE_CHECK_CLASS_TYPE ((klass), BOX_TYPE))

typedef struct _Box      Box;
typedef struct _BoxClass BoxClass;

typedef struct
{
  gdouble position[3];
  gdouble size[3];
  gdouble rotation;
  gboolean drive_through;
  gboolean shoot_through;
} BoxParams;

struct _Box
{
  SceneObject parent;

  BoxParams param;
  gboolean state_dirty;
  GList *drawables;
  Drawable *sides, *top;
};

struct _BoxClass
{
  SceneObjectClass parent_class;
};

GType box_get_type (void) G_GNUC_CONST;
Box*  box_new      (void);

#define BOX_SIDES_DRAWABLE_TYPE            (box_sides_drawable_get_type ())
#define BOX_SIDES_DRAWABLE(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), BOX_SIDES_DRAWABLE_TYPE, BoxSidesDrawable))
#define BOX_SIDES_DRAWABLE_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), BOX_SIDES_DRAWABLE_TYPE, BoxSidesDrawableClass))
#define IS_BOX_SIDES_DRAWABLE(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), BOX_SIDES_DRAWABLE_TYPE))
#define IS_BOX_SIDES_DRAWABLE_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), BOX_SIDES_DRAWABLE_TYPE))

typedef struct _BoxSidesDrawable      BoxSidesDrawable;
typedef struct _BoxSidesDrawableClass BoxSidesDrawableClass;

struct _BoxSidesDrawable
{
  DisplayList parent;
};

struct _BoxSidesDrawableClass
{
  DisplayListClass parent_class;
};

GType     box_sides_drawable_get_type (void) G_GNUC_CONST;
Drawable* box_sides_drawable_new      (SceneObject *parent);

#define BOX_TOP_DRAWABLE_TYPE            (box_top_drawable_get_type ())
#define BOX_TOP_DRAWABLE(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), BOX_TOP_DRAWABLE_TYPE, BoxTopDrawable))
#define BOX_TOP_DRAWABLE_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), BOX_TOP_DRAWABLE_TYPE, BoxTopDrawableClass))
#define IS_BOX_TOP_DRAWBLE(obj)          (G_TYPE_CHECK_INSTANCE_TYPE ((obj), BOX_TOP_DRAWABLE_TYPE))
#define IS_BOX_TOP_DRAWABLE_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), BOX_TOP_DRAWABLE_TYPE))

typedef struct _BoxTopDrawable      BoxTopDrawable;
typedef struct _BoxTopDrawableClass BoxTopDrawableClass;

struct _BoxTopDrawable
{
  DisplayList parent;
};

struct _BoxTopDrawableClass
{
  DisplayListClass parent_class;
};

GType     box_top_drawable_get_type (void) G_GNUC_CONST;
Drawable* box_top_drawable_new      (SceneObject *parent);

G_END_DECLS

#endif
