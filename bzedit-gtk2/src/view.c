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

static void view_class_init  (ViewClass *klass);
static void view_init        (View *view);
static void init_lighting    (View *view);
static void reset_lighting   (View *view);
static void default_lighting (View *view);
static void view_resize      (View *view);

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

    view_type = g_type_register_static (GTK_TYPE_VBOX, "View", &view_info, 0);
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

static void
view_resize (View *view)
{
  gint width, height;
  gdk_window_get_size (GTK_WIDGET(view->context)->window, &width, &height);

  gl_drawing_area_make_current (view->context);

  glViewport(0, 0, width, height);
  glMatrixMode (GL_PROJECTION);
  glLoadIdentity ();
  gluPerspective (45.0, (float)width / (float)height, 0.1, 2500.0);
  glMatrixMode (GL_MODELVIEW);
  glLoadIdentity ();
  glDepthRange (0.1, 2000.0);
}

static gboolean
view_configure (GLDrawingArea *widget, GdkEventConfigure *event, View *view)
{
  view_resize (view);
  view_render (view);
  return TRUE;
}

static gboolean
view_scroll (GLDrawingArea *widget, GdkEventScroll *event, View *view)
{
  if (event->direction == GDK_SCROLL_UP)
  {
    if (view->camera->distance > 10)
      view->camera->distance -= 40;
    view_render (view);
  }
  else if (event->direction == GDK_SCROLL_DOWN)
  {
    if (view->camera->distance < 1500)
      view->camera->distance += 40;
    view_render (view);
  }

  return TRUE;
}

static gboolean
view_click (GLDrawingArea *widget, GdkEventButton *event, View *view)
{
  if (event->button == 2)
  {
    /* middle button, start roaming */
    view->mouse[0] = event->x;
    view->mouse[1] = event->y;
  }
  else if (event->button == 3)
  {
    /* right button, do picking */
    guint pos[2];
    SceneObject *object;

    pos[0] = event->x;
    pos[1] = event->y;
    object = view_pick (view, pos);
    if (object)
      scene_object_select (object);
  }

  return TRUE;
}

static gboolean
view_motion (GLDrawingArea *widget, GdkEvent *event, View *view)
{
  double delta_x, delta_y;
  double current_x, current_y;

  /* This could be an actual motion event or it could just be a motion hint
   * from GDK_POINTER_MOTION_HINT_MASK. If it's a hint, we have to ask for
   * the current mouse position. This ensures we're always drawing with the
   * current mouse position, rather than queueing mouse positions.
   */
  if (event->motion.is_hint) {
    gint ix, iy;
    gdk_window_get_pointer(event->motion.window, &ix, &iy, NULL);
    current_x = ix;
    current_y = iy;
  }
  else {
    current_x = event->motion.x;
    current_y = event->motion.y;
  }

  delta_x = current_x - view->mouse[0];
  delta_y = current_y - view->mouse[1];
  view->mouse[0] = current_x;
  view->mouse[1] = current_y;

  view->camera->azimuth -= delta_x / 5;
  view->camera->elevation += delta_y / 5;
  if (view->camera->elevation < 0)
    view->camera->elevation = 0;
  if (view->camera->elevation > 90)
    view->camera->elevation = 90;
  view_render (view);

  return TRUE;
}

static gboolean
view_expose (GLDrawingArea *widget, GdkEventExpose *event, View *view)
{
  view_render (view);

  return TRUE;
}

static void
on_gl_context_realize (GLDrawingArea *context, View *view)
{
  view->camera->azimuth = 45;
  view->camera->elevation = 25;
  view->camera->distance = 900;
  view->camera->position[2] = 4;

  gl_drawing_area_make_current (context);

  g_signal_connect (G_OBJECT (context), "configure-event", G_CALLBACK (view_configure), (gpointer) view);
  g_signal_connect (G_OBJECT (context), "expose-event", G_CALLBACK (view_expose), (gpointer) view);
  g_signal_connect (G_OBJECT (context), "scroll-event", G_CALLBACK (view_scroll), (gpointer) view);
  g_signal_connect (G_OBJECT (context), "button-press-event", G_CALLBACK (view_click), (gpointer) view);
  g_signal_connect (G_OBJECT (context), "motion-notify-event", G_CALLBACK (view_motion), (gpointer) view);

  init_lighting (view);
  scene_add_view (view->scene, (gpointer) view);
  view_resize (view);
  view_render (view);
}

GtkWidget*
view_new (Scene *scene, GLDrawingArea *context)
{
  View *view = VIEW (g_object_new (view_get_type (), NULL));
  GtkWidget *box, *window;

  view->xml = glade_xml_new (GLADEDIR "/view.glade", NULL, NULL);
  if (!view->xml)
    return NULL;
  box = glade_xml_get_widget (view->xml, "view vbox");
  window = glade_xml_get_widget (view->xml, "window");

  gtk_widget_ref (box);
  gtk_container_remove (GTK_CONTAINER (window), box);
  gtk_box_pack_start (GTK_BOX (view), GTK_WIDGET (box), TRUE, TRUE, 0);
  gtk_widget_unref (box);

  gtk_box_pack_start (GTK_BOX (box), GTK_WIDGET (context), TRUE, TRUE, 0);
  gtk_box_reorder_child (GTK_BOX (box), GTK_WIDGET (context), 0);

  if (scene == NULL)
    scene = scene_new ();

  view->scene = scene;
  view->context = context;

  g_signal_connect_after (G_OBJECT (context), "realize", G_CALLBACK (on_gl_context_realize), (gpointer) view);

  return GTK_WIDGET (view);
}

void
view_render (View *view)
{
  RenderState *rstate = render_state_new ();
  gint i;

  gl_drawing_area_make_current (view->context);
  glClearColor (0.0, 0.0, 0.0, 0.0);
  glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  camera_load (view->camera);
  for (i = 0; i < view->nlights; i++)
    light_set (view->lights[i]);
  scene_render (view->scene, rstate);

  gl_drawing_area_swap_buffers (view->context);
  g_object_unref (rstate);
}

SceneObject*
view_pick (View *view, guint pos[2])
{
  RenderState *rstate = render_state_new ();
  GLuint viewport[4];
  gint width, height;

  gdk_window_get_size (GTK_WIDGET(view->context)->window, &width, &height);
  gl_drawing_area_make_current (view->context);

  rstate->picking = TRUE;

  glGetIntegerv (GL_VIEWPORT, viewport);
  glMatrixMode (GL_PROJECTION);
  glPushMatrix ();
  glLoadIdentity ();
  gluPickMatrix ((double)pos[0], (double) (viewport[3] - pos[1]), 1.0, 1.0, viewport);
  gluPerspective (45.0, (float)width / (float)height, 0.1, 2500.0);

  glMatrixMode (GL_MODELVIEW);
  camera_load (view->camera);
  return scene_pick (view->scene, rstate, pos);
}

static void
init_lighting (View *view)
{
  gint i;

  glGetIntegerv (GL_MAX_LIGHTS, &view->nlights);
  view->lights = g_new (Light*, view->nlights);
  for (i = 0; i < view->nlights; i++)
  {
    view->lights[i] = light_new (GL_LIGHT0 + i);
  }
  default_lighting (view);
}

static void
reset_lighting (View *view)
{
  gint i;
  static float ambient[] = {0.0f, 0.0f, 0.0f, 1.0f};

  for (i = 0; i < view->nlights; i++)
    light_reset (view->lights[i]);
  glLightModelfv (GL_LIGHT_MODEL_AMBIENT, ambient);
}

static void
default_lighting (View *view)
{
  reset_lighting (view);

  view->lights[0]->enabled = TRUE;

  view->lights[0]->ambient[0] = 0.25f;
  view->lights[0]->ambient[1] = 0.25f;
  view->lights[0]->ambient[2] = 0.25f;
  view->lights[0]->ambient[3] = 1.0f;

  view->lights[0]->diffuse[0] = 0.65f;
  view->lights[0]->diffuse[1] = 0.65f;
  view->lights[0]->diffuse[2] = 0.65f;
  view->lights[0]->diffuse[3] = 1.0f;

  view->lights[0]->position[0] = 300.0f;
  view->lights[0]->position[1] = 400.0f;
  view->lights[0]->position[2] = 400.0f;
  view->lights[0]->position[3] = 1.0f;

  view->lights[1]->enabled = TRUE;

  view->lights[1]->ambient[0] = 0.05f;
  view->lights[1]->ambient[1] = 0.05f;
  view->lights[1]->ambient[2] = 0.05f;
  view->lights[1]->ambient[3] = 1.0f;

  view->lights[1]->diffuse[0] = 0.85f;
  view->lights[1]->diffuse[1] = 0.85f;
  view->lights[1]->diffuse[2] = 0.85f;
  view->lights[1]->diffuse[3] = 1.0f;

  view->lights[1]->position[0] = 0.0f;
  view->lights[1]->position[1] = 0.0f;
  view->lights[1]->position[2] = 400.0f;
  view->lights[1]->position[3] = 1.0f;
}
