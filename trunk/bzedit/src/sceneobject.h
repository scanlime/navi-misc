/*
 * sceneobject.h - Base class for objects which appear in the scene
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

#ifndef __SCENE_OBJECT_H__
#define __SCENE_OBJECT_H__

#include "parameter-holder.h"
#include "drawable.h"

G_BEGIN_DECLS

#define SCENE_OBJECT_TYPE               (scene_object_get_type ())
#define SCENE_OBJECT(obj)               (G_TYPE_CHECK_INSTANCE_CAST ((obj), SCENE_OBJECT_TYPE, SceneObject))
#define SCENE_OBJECT_CLASS(klass)       (G_TYPE_CHECK_CLASS_CAST ((klass), SCENE_OBJECT_TYPE, SceneObjectClass))
#define IS_SCENE_OBJECT(obj)            (G_TYPE_CHECK_INSTANCE_TYPE ((obj), SCENE_OBJECT_TYPE))
#define IS_SCENE_OBJECT_CLASS(klass)    (G_TYPE_CHECK_CLASS_TYPE ((klass), SCENE_OBJECT_TYPE))

typedef struct _SceneObject      SceneObject;
typedef struct _SceneObjectClass SceneObjectClass;

typedef struct
{
  /* symmetric in -all three- directions! */
  gdouble position[3];
  gdouble rotation;
  gdouble size[3];
} BoundingBox;

struct _SceneObject
{
  ParameterHolder holder;

  gboolean selected;
  GtkTreeIter iter;
  SceneObject *parent;
  GtkTreePath *path;
  Drawable *selection;

  BoundingBox bb;

  gchar *name;
};

struct _SceneObjectClass
{
  ParameterHolderClass parent_class;

  gboolean creatable;
  gboolean autocreate;
  gboolean canparent;
  gboolean parentable;

  /* signal handlers */
  void       (*dirty)             (SceneObject *self);
  void       (*selected)          (SceneObject *self);

  /* subclassable functions */
  void       (*render)            (SceneObject *self, guint name);
  void       (*serialize)         (SceneObject *self, GIOChannel *out);
  GdkPixbuf* (*get_icon)          (void);
  GList*     (*get_drawables)     (SceneObject *self);
  GList*     (*get_all_drawables) (SceneObject *self);
  void       (*select)            (SceneObject *self);
  void       (*deselect)          (SceneObject *self);
  gboolean   (*parent)            (SceneObject *self, SceneObject *child);
  gboolean   (*deparent)          (SceneObject *self, SceneObject *child);
};

GType      scene_object_get_type          (void) G_GNUC_CONST;
void       scene_object_render            (SceneObject *self, guint name);
void       scene_object_serialize         (SceneObject *self, GIOChannel *out);
GList*     scene_object_get_drawables     (SceneObject *self);
GList*     scene_object_get_all_drawables (SceneObject *self);
void       scene_object_select            (SceneObject *self);
void       scene_object_deselect          (SceneObject *self);
gboolean   scene_object_parent            (SceneObject *parent, SceneObject *child);
gboolean   scene_object_deparent          (SceneObject *parent, SceneObject *child);

#define SELECTION_DRAWABLE_TYPE            (selection_drawable_get_type ())
#define SELECTION_DRAWABLE(obj)            (G_TYPE_CHECK_INSTANCE_TYPE ((obj), SELECTION_DRAWABLE_TYPE, SelectionDrawable))
#define SELECTION_DRAWABLE_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), SELECTION_DRAWABLE_TYPE, SelectionDrawableClass))
#define IS_SELECTION_DRAWABLE(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), SELECTION_DRAWABLE_TYPE))
#define IS_SELECTION_DRAWABLE_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), SELECTION_DRAWABLE_TYPE))

typedef struct _SelectionDrawable      SelectionDrawable;
typedef struct _SelectionDrawableClass SelectionDrawableClass;

struct _SelectionDrawable
{
  Drawable parent;
};

struct _SelectionDrawableClass
{
  DrawableClass parent_class;
};

GType     selection_drawable_get_type (void) G_GNUC_CONST;
Drawable* selection_drawable_new      (SceneObject *parent);

G_END_DECLS

#endif
