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

static void scene_class_init (SceneClass *klass);
static void scene_init       (Scene *self);

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
scene_init (Scene *self)
{
  self->render_passes = NULL;
  self->objects = NULL;
  self->dirty = FALSE;
}

Scene*
scene_new ()
{
  return SCENE (g_object_new (scene_get_type (), NULL));
}

gboolean
scene_has_main_view (Scene *self)
{
}

void
scene_erase (Scene *self)
{
  g_list_free (self->objects);
  self->objects = NULL;
  self->dirty = TRUE;
}

static gint
render_priority_compare (RenderPass *p1, RenderPass *p2)
{
  RenderPassClass *c1, *c2;

  c1 = RENDER_PASS_CLASS (G_OBJECT_GET_CLASS (p1));
  c2 = RENDER_PASS_CLASS (G_OBJECT_GET_CLASS (p2));

  return (c1->render_priority - c2->render_priority);
}

static gint
filter_priority_compare (RenderPass *p1, RenderPass *p2)
{
  RenderPassClass *c1, *c2;

  c1 = RENDER_PASS_CLASS (G_OBJECT_GET_CLASS (p1));
  c2 = RENDER_PASS_CLASS (G_OBJECT_GET_CLASS (p2));

  return (c1->filter_priority - c2->filter_priority);
}

void
scene_preprocess (Scene *self)
{
  GList *render_sort, *filter_sort;
  GList *i;
  /* rebuilds rendering passes. This operation clears the 'dirty' flag, and
   * is called automatically when a dirty scene needs rendering
   */

  render_sort = g_list_sort (self->render_passes, (GCompareFunc) render_priority_compare);
  filter_sort = g_list_sort (self->render_passes, (GCompareFunc) filter_priority_compare);

  for (i = render_sort; i; i = i->next)
  {
    RenderPass *pass = RENDER_PASS (i->data);
    render_pass_erase (pass);
  }

  /* divy up the drawables into rendering passes using the passes' filter function */

  /* give each pass a chance to preprocess */
  for (i = render_sort; i; i = i->next)
  {
    RenderPass *pass = RENDER_PASS (i->data);
    render_pass_preprocess (pass);
  }

  self->dirty = FALSE;

  g_list_free (self->render_passes);
  self->render_passes = render_sort;
  g_list_free (filter_sort);
}

void
scene_render (Scene *self, RenderState *rstate)
{
  GList *pass;

  if (self->dirty)
    scene_preprocess (self);

  glDisable (GL_BLEND);
  glEnable (GL_DEPTH_TEST);
  glEnable (GL_CULL_FACE);
  glEnable (GL_COLOR_MATERIAL);
  glEnable (GL_LIGHTING);
  glDisable (GL_LINE_SMOOTH);
  glColor4f (1.0, 1.0, 1.0, 1.0);

  for (pass = self->render_passes; pass; pass = pass->next)
  {
    RenderPass *rpass = RENDER_PASS (pass->data);
    if (!render_pass_is_empty (rpass))
      render_pass_render (rpass, rstate);
  }
}

SceneObject*
scene_pick (Scene *self, RenderState *rstate, guint position[2])
{
}
