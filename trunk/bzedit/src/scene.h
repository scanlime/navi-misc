/*
 * scene.h - Set of all the objects this view sees, organized into rendering
 *           passes and sorted by texture. Multiple rendering passesare necessary
 *           to deal with blended objects, and texture sorting is an important
 *           OpenGL optimization.
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

#ifndef __SCENE_H__
#define __SCENE_H__

#include <gtk/gtk.h>
#include "renderstate.h"
#include "sceneobject.h"
#include "renderpass.h"

G_BEGIN_DECLS

#define SCENE_TYPE            (scene_get_type ())
#define SCENE(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), SCENE_TYPE, Scene))
#define SCENE_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), SCENE_TYPE, SceneClass))
#define IS_SCENE(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), SCENE_TYPE))
#define IS_SCENE_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), SCENE_TYPE))

typedef struct _Scene      Scene;
typedef struct _SceneClass SceneClass;

struct _Scene
{
  GObject parent;

  GList* render_passes;
  GHashTable* objects;
  gboolean dirty;
  GList* views;
  gboolean frozen;

  GtkTreeStore *element_store;
};

struct _SceneClass
{
  GObjectClass parent_class;
};

GType        scene_get_type      (void) G_GNUC_CONST;
Scene*       scene_new           (void);
void         scene_erase         (Scene *self);
void         scene_add           (Scene *self, SceneObject *object);
void         scene_preprocess    (Scene *self);
void         scene_render        (Scene *self, RenderState *rstate);
void         scene_add_view      (Scene *self, gpointer view);
void         scene_freeze        (Scene *self);
void         scene_thaw          (Scene *self);
SceneObject* scene_pick          (Scene *self, RenderState *rstate, guint position[2]);

G_END_DECLS

#endif
