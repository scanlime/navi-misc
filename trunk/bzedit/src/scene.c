/*
 * scene.c - Set of all the objects this view sees, organized into rendering
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

#include "scene.h"
#include "renderpass.h"
#include "plugins.h"
#include "view.h"

static void   scene_class_init  (SceneClass *klass);
static void   scene_init        (Scene *self);
static void   scene_view_redraw (SceneObject *object, View *view);

GType
scene_get_type (void)
{
  static GType scene_type = 0;
  if (!scene_type)
  {
    static const GTypeInfo scene_info =
    {
	sizeof (SceneClass),
	NULL,               /* base init */
	NULL,               /* base finalize */
	(GClassInitFunc) scene_class_init,
	NULL,               /* class finalize */
	NULL,               /* class data */
	sizeof (Scene),
	0,                  /* n preallocs */
	(GInstanceInitFunc) scene_init,
    };

    scene_type = g_type_register_static (G_TYPE_OBJECT, "Scene", &scene_info, 0);
  }

  return scene_type;
}

static void
scene_class_init (SceneClass *klass)
{
}

static void
tree_store_reordered (GtkTreeModel *treemodel, GtkTreePath *arg1, GtkTreeIter *arg2, gpointer arg3, Scene *scene)
{
  g_print ("reordered!\n");
}

static void
scene_init (Scene *self)
{
  GList *render_pass_types, *type;

  self->objects = g_hash_table_new (g_direct_hash, g_direct_equal);
  self->dirty = FALSE;

  self->render_passes = NULL;
  render_pass_types = find_type_children (RENDER_PASS_TYPE);
  for (type = render_pass_types; type; type = type->next)
  {
    RenderPass *pass = RENDER_PASS (g_object_new (GPOINTER_TO_UINT (type->data), NULL));
    self->render_passes = g_list_append (self->render_passes, (gpointer) pass);
  }

  self->views = NULL;

  /* ----------------------------------------- object name -- object icon ---- scene pointer - object pointer */
  self->element_store = gtk_tree_store_new (4, G_TYPE_STRING, GDK_TYPE_PIXBUF, G_TYPE_POINTER, G_TYPE_POINTER);
  g_signal_connect (G_OBJECT (self->element_store), "rows-reordered", G_CALLBACK (tree_store_reordered), self);
}

Scene*
scene_new ()
{
  return SCENE (g_object_new (scene_get_type (), NULL));
}

void
scene_erase (Scene *self)
{
  g_hash_table_destroy (self->objects);
  self->objects = g_hash_table_new (g_direct_hash, g_direct_equal);
  gtk_tree_store_clear (self->element_store);
  self->dirty = TRUE;
}

void
scene_add (Scene *self, SceneObject *object)
{
  GList *drawables, *view;
  SceneObjectClass *klass = SCENE_OBJECT_CLASS (G_OBJECT_GET_CLASS (object));

  drawables = scene_object_get_drawables (object);

  gtk_tree_store_append (self->element_store, &object->iter, NULL);
  gtk_tree_store_set (self->element_store, &object->iter, 0, g_type_name (G_TYPE_FROM_INSTANCE (object)), 1, klass->get_icon (), 2, self, 3, object, -1);

  g_hash_table_insert (self->objects, g_object_ref(object), (gpointer) drawables);
  self->dirty = TRUE;

  for (view = self->views; view; view = view->next)
  {
    g_signal_connect (object, "dirty", G_CALLBACK (scene_view_redraw), view->data);
    view_render (VIEW (view->data));
  }
}

static gint
render_priority_compare (RenderPass *p1, RenderPass *p2)
{
  RenderPassClass *c1, *c2;

  c1 = RENDER_PASS_CLASS (G_OBJECT_GET_CLASS (p1));
  c2 = RENDER_PASS_CLASS (G_OBJECT_GET_CLASS (p2));

  return (c2->render_priority - c1->render_priority);
}

static gint
filter_priority_compare (RenderPass *p1, RenderPass *p2)
{
  RenderPassClass *c1, *c2;

  c1 = RENDER_PASS_CLASS (G_OBJECT_GET_CLASS (p1));
  c2 = RENDER_PASS_CLASS (G_OBJECT_GET_CLASS (p2));

  return (c2->filter_priority - c1->filter_priority);
}

static void
scene_preprocess_iterate (SceneObject *object, GList *drawables, GList *passes)
{
  GList *drawable, *pass;

  for (drawable = drawables; drawable; drawable = drawable->next) {
    for (pass = passes; pass; pass = pass->next) {
      if (render_pass_filter (RENDER_PASS (pass->data), DRAWABLE (drawable->data))) {
        render_pass_add (RENDER_PASS (pass->data), DRAWABLE (drawable->data));
	break;
      }
    }
  }
}

void
scene_preprocess (Scene *self)
{
  GList *i;

  /* rebuilds rendering passes. This operation clears the 'dirty' flag, and
   * is called automatically when a dirty scene needs rendering
   */

  self->render_passes = g_list_sort (self->render_passes, (GCompareFunc) filter_priority_compare);
  for (i = self->render_passes; i; i = i->next)
  {
    RenderPass *pass = RENDER_PASS (i->data);
    render_pass_erase (pass);
  }

  /* divy up the drawables into rendering passes using the passes' filter function */
  g_hash_table_foreach (self->objects, (GHFunc) scene_preprocess_iterate, (gpointer) self->render_passes);

  self->render_passes = g_list_sort (self->render_passes, (GCompareFunc) render_priority_compare);
  /* give each pass a chance to preprocess */
  for (i = self->render_passes; i; i = i->next)
  {
    RenderPass *pass = RENDER_PASS (i->data);
    render_pass_preprocess (pass);
  }

  self->dirty = FALSE;
}

void
scene_render (Scene *self, RenderState *rstate)
{
  GList *pass;

  if (self->dirty)
    scene_preprocess (self);

  glClearDepth (1.0);
  glDepthFunc (GL_LESS);
  glShadeModel (GL_SMOOTH);
  glDisable (GL_BLEND);
  glEnable (GL_DEPTH_TEST);
  glEnable (GL_CULL_FACE);
  glEnable (GL_COLOR_MATERIAL);
  glEnable (GL_LIGHTING);
  glDisable (GL_LINE_SMOOTH);
  glEnable (GL_NORMALIZE);
  glColor4f (1.0, 1.0, 1.0, 1.0);

  for (pass = self->render_passes; pass; pass = pass->next)
  {
    RenderPass *rpass = RENDER_PASS (pass->data);
    if (!render_pass_is_empty (rpass))
      render_pass_render (rpass, rstate);
  }
}

static void
scene_view_redraw (SceneObject *object, View *view)
{
  view_render (view);
}

static void
scene_add_view_iterate (GObject *object, GList *drawables, View *view)
{
  g_signal_connect (object, "dirty", G_CALLBACK (scene_view_redraw), (gpointer) view);
}

void
scene_add_view (Scene *self, gpointer view)
{
  self->views = g_list_append (self->views, view);
  g_hash_table_foreach (self->objects, (GHFunc) scene_add_view_iterate, view);
  view_render (view);
}

SceneObject*
scene_pick (Scene *self, RenderState *rstate, guint position[2])
{
}
