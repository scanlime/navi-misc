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

G_BEGIN_DECLS

#define SCENE_OBJECT_TYPE		(scene_object_get_type ())
#define SCENE_OBJECT(obj)		(G_TYPE_CHECK_INSTANCE_CAST ((obj), SCENE_OBJECT_TYPE, SceneObject))
#define SCENE_OBJECT_CLASS(klass)	(G_TYPE_CHECK_CLASS_CAST ((klass), SCENE_OBJECT_TYPE, SceneObjectClass))
#define IS_SCENE_OBJECT(obj)		(G_TYPE_CHECK_INSTANCE_TYPE ((obj), SCENE_OBJECT_TYPE))
#define IS_SCENE_OBJECT_CLASS(klass)	(G_TYPE_CHECK_CLASS_TYPE ((klass), SCENE_OBJECT_TYPE))

typedef struct _SceneObject      SceneObject;
typedef struct _SceneObjectClass SceneObjectClass;

struct _SceneObject
{
  ParameterHolder parent;
};

struct _SceneObjectClass
{
  ParameterHolderClass parent_class;

  void       (*render)         (SceneObject *self, guint name);
  void       (*serialize)      (SceneObject *self, GIOChannel *out);
  gboolean   (*creatable)      (void);
  GdkPixbuf* (*get_icon)       (void);
};

GType      scene_object_get_type  (void) G_GNUC_CONST;
void       scene_object_render    (SceneObject *self, guint name);
void       scene_object_serialize (SceneObject *self, GIOChannel *out);

G_END_DECLS

#endif
