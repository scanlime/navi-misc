/*
 * view.c - A generic 3D view with multiple rendering passes
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

#include "view.h"

static void view_class_init (ViewClass *klass);
static void view_init       (View *view);

GType
view_get_type (void)
{
  static GType view_type = 0;
  if (!view_type)
  {
    static const GTypeInfo view_info =
    {
	sizeof (ViewClass),
	NULL,               /* base init */
	NULL,               /* base finalize */
	(GClassInitFunc)    view_class_init,
	NULL,               /* class finalize */
	NULL,               /* class data */
	sizeof (View),
	0,                  /* n preallocs */
	(GInstanceInitFunc) view_init,
    };

    view_type = g_type_register_static (G_TYPE_OBJECT, "View", &view_info, 0);
  }

  return view_type;
}

static void
view_class_init (ViewClass *klass)
{
}

static void
view_init (View *view)
{
  view->lights = NULL;
  view->camera = camera_new ();
}

View*
view_new (Scene *scene)
{
  View *view = VIEW (g_object_new (view_get_type (), NULL));

  if (scene == NULL)
    scene = scene_new ();

  view->scene = scene;
}

void
view_render (View *view)
{
  RenderState *rstate = render_state_new ();
  camera_load (view->camera);
  scene_render (view->scene, rstate);
}

SceneObject*
view_pick (View *view, guint pos[2])
{
  RenderState *rstate = render_state_new ();
  camera_load (view->camera);
  return scene_pick (view->scene, rstate, pos);
}
