#include <gdl/gdl-dock-item.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include "viewbox.h"
#include "globals.h"
#include <iostream>

int ViewBox::number = 1;

ViewBox::ViewBox()
{
  ready = false;
  int glargs[] = { GDK_GL_RGBA,
                   GDK_GL_DOUBLEBUFFER,
                   GDK_GL_DEPTH_SIZE, 1,
                   GDK_GL_NONE
		  };

  if(number == 1)
  {
    dockitem = gdl_dock_item_new("view", "view", GDL_DOCK_ITEM_BEH_NORMAL);
  }
  else
  {
    gchar title[50];
    snprintf(title, 50, "view-%d", number);
    dockitem = gdl_dock_item_new(title, title, GDL_DOCK_ITEM_BEH_NORMAL);
  }
  number++;

  glarea = gtk_gl_area_new(glargs);
  gtk_widget_set_events(glarea, GDK_EXPOSURE_MASK | GDK_BUTTON_PRESS_MASK | GDK_BUTTON3_MOTION_MASK | GDK_KEY_PRESS_MASK | GDK_KEY_RELEASE_MASK | GDK_SCROLL_MASK);
  gtk_signal_connect(GTK_OBJECT(glarea), "realize", GTK_SIGNAL_FUNC(on_glarea_realize), this);
  gtk_signal_connect(GTK_OBJECT(glarea), "expose_event", GTK_SIGNAL_FUNC(on_glarea_expose), this);
  gtk_signal_connect(GTK_OBJECT(glarea), "button_press_event", GTK_SIGNAL_FUNC(on_glarea_button), this);
  gtk_signal_connect(GTK_OBJECT(glarea), "configure-event", GTK_SIGNAL_FUNC(on_glarea_configure), this);
  gtk_signal_connect(GTK_OBJECT(glarea), "motion_notify_event", GTK_SIGNAL_FUNC(on_glarea_motion), this);
  gtk_signal_connect(GTK_OBJECT(glarea), "key_press_event", GTK_SIGNAL_FUNC(on_glarea_keypress), this);
  gtk_signal_connect(GTK_OBJECT(glarea), "key_release_event", GTK_SIGNAL_FUNC(on_glarea_keyrelease), this);
  gtk_signal_connect(GTK_OBJECT(glarea), "scroll_event", GTK_SIGNAL_FUNC(on_glarea_scroll), this);

  gtk_container_add(GTK_CONTAINER(dockitem), glarea);

  gtk_widget_set_size_request(glarea, 400, 300);

  gtk_widget_show_all(dockitem);
}

ViewBox::ViewBox(const ViewBox &share)
{
  ready = false;
  int glargs[] = { GDK_GL_RGBA,
  		   GDK_GL_DOUBLEBUFFER,
		   GDK_GL_DEPTH_SIZE, 1,
		   GDK_GL_NONE
		 };

  if(number == 1)
  {
    dockitem = gdl_dock_item_new("view", "view", GDL_DOCK_ITEM_BEH_NORMAL);
  }
  else
  {
    gchar title[50];
    snprintf(title, 50, "view-%d", number);
    dockitem = gdl_dock_item_new(title, title, GDL_DOCK_ITEM_BEH_NORMAL);
  }
  number++;

  glarea = gtk_gl_area_share_new(glargs, GTK_GL_AREA(share.glarea));
  gtk_widget_set_events(glarea, GDK_EXPOSURE_MASK | GDK_BUTTON_PRESS_MASK | GDK_BUTTON3_MOTION_MASK | GDK_KEY_PRESS_MASK | GDK_KEY_RELEASE_MASK | GDK_SCROLL_MASK);
  gtk_signal_connect(GTK_OBJECT(glarea), "realize", GTK_SIGNAL_FUNC(on_glarea_realize), this);
  gtk_signal_connect(GTK_OBJECT(glarea), "expose_event", GTK_SIGNAL_FUNC(on_glarea_expose), this);
  gtk_signal_connect(GTK_OBJECT(glarea), "button_press_event", GTK_SIGNAL_FUNC(on_glarea_button), this);
  gtk_signal_connect(GTK_OBJECT(glarea), "configure-event", GTK_SIGNAL_FUNC(on_glarea_configure), this);
  gtk_signal_connect(GTK_OBJECT(glarea), "motion_notify_event", GTK_SIGNAL_FUNC(on_glarea_motion), this);
  gtk_signal_connect(GTK_OBJECT(glarea), "key_press_event", GTK_SIGNAL_FUNC(on_glarea_keypress), this);
  gtk_signal_connect(GTK_OBJECT(glarea), "key_release_event", GTK_SIGNAL_FUNC(on_glarea_keyrelease), this);
  gtk_signal_connect(GTK_OBJECT(glarea), "scroll_event", GTK_SIGNAL_FUNC(on_glarea_scroll), this);

  gtk_container_add(GTK_CONTAINER(dockitem), glarea);

  gtk_widget_set_size_request(glarea, 400, 300);

  gtk_widget_show_all(dockitem);
}

void ViewBox::readied()
{
  ready = true;
}

void ViewBox::clear()
{
  if(!ready)
    return;
  make_current();
  glClearColor(0, 0, 0, 0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  swap_buffers();
}

void ViewBox::draw_scene()
{
  if(!ready)
    return;
  make_current();
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  camera.setup();
  doc->world.Render();
  swap_buffers();
}

void ViewBox::make_current()
{
  gtk_gl_area_make_current(GTK_GL_AREA(glarea));
}

void ViewBox::swap_buffers()
{
  gtk_gl_area_swap_buffers(GTK_GL_AREA(glarea));
}

GtkWidget *ViewBox::getWidget()
{
  return dockitem;
}

gint ViewBox::on_glarea_realize(GtkWidget *widget, gpointer data)
{
  ViewBox *b = static_cast<ViewBox*>(data);
  if(!b->ready)
    return FALSE;
  b->make_current();
  GLfloat light_ambient[] = {0.5, 0.5, 0.5, 1.0};
  GLfloat light_diffuse[] = {0.75, 0.75, 0.75, 1.0};
  GLfloat light_position[] = {400, 400, 400, 1.0};

  glEnable(GL_TEXTURE_2D);
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_NORMALIZE);
  glEnable(GL_CULL_FACE);

  glClearColor(0, 0, 0, 0);
  glClearDepth(1.0);
  glDepthFunc(GL_LESS);
  glShadeModel(GL_SMOOTH);
  glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

  glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
  glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
  glLightfv(GL_LIGHT0, GL_POSITION, light_position);
  glEnable(GL_LIGHT0);
  glEnable(GL_LIGHTING);

  glViewport(0, 0, b->glarea->allocation.width, b->glarea->allocation.height);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(45.0, (double) b->glarea->allocation.width / (double) b->glarea->allocation.height, 3.0, 2500.0);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  b->draw_scene();
  return TRUE;
}

gint ViewBox::on_glarea_expose(GtkWidget *widget, GdkEventExpose *event, gpointer data)
{
  ViewBox *b = static_cast<ViewBox*>(data);
  if(!b->ready)
    return FALSE;
  b->draw_scene();
  return TRUE;
}

gint ViewBox::on_glarea_button(GtkWidget *widget, GdkEventButton *event, gpointer data)
{
  ViewBox *b = static_cast<ViewBox*>(data);
  if(!b->ready)
    return FALSE;
  int x = static_cast<int>(event->x);
  int y = static_cast<int>(event->y);
  GLuint viewport[4];
  GLuint *buffer;
  if(event->button == 1) {
    // left button, do picking
    /*
    glGetIntegerv(GL_VIEWPORT, viewport);
    buffer = new GLuint[ * 4];
    glSelectBuffer( * 4, buffer);
    glRenderMode(GL_SELECT);
    glInitNames();
    glPushName(0);
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluPickMatrix((double) x, (double) (viewport[3] - y), 1.0, 1.0, viewport);
    gluPerspective(45.0, (float) (viewport[2] - viewport[0]) / (float) (viewport[3] - viewport[1]), 3.0, 2500.0);
    glMatrixMode(GL_MODELVIEW);
    // render
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    GLint hits = glRenderMode(GL_RENDER);
    if(hits > 0) {
      int choose = buffer[3];
      int depth = buffer[1];
      for(int i = 0; i < hits; i++) {
	if(buffer[i * 4 + 1] < depth) {
	  choose = buffer[i * 4 + 3];
	  depth = buffer[i * 4 + 1];
	}
      }
      choose--;
      // select(choose)
    } else {
      // unselect-all
    }
    delete buffer;
    */
  } else if(event->button == 2) {
    // middle button, do pick and center
  } else if(event->button == 3) {
    // right button, start roaming
    b->mouse_x = event->x_root;
    b->mouse_y = event->y_root;
  }
  return TRUE;
}

gint ViewBox::on_glarea_configure(GtkWidget *widget, GdkEventExpose *event, gpointer data)
{
  ViewBox *b = static_cast<ViewBox*>(data);
  if(!b->ready)
    return FALSE;
  b->make_current();
  glViewport(0, 0, b->glarea->allocation.width, b->glarea->allocation.height);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(45.0, static_cast<double>(b->glarea->allocation.width) / static_cast<double>(b->glarea->allocation.height), 3.0, 2500.0);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  b->draw_scene();

  return TRUE;
}

gint ViewBox::on_glarea_motion(GtkWidget *widget, GdkEventMotion *event, gpointer data)
{
  ViewBox *b = static_cast<ViewBox*>(data);
  if(!b->ready)
    return FALSE;

  double deltax = event->x_root - b->mouse_x;
  double deltay = event->y_root - b->mouse_y;
  b->mouse_x = event->x_root;
  b->mouse_y = event->y_root;

  b->camera.rot += deltax / 5;
  b->camera.ele += deltay / 5;
  if(b->camera.ele > 0)
    b->camera.ele = 0;
  if(b->camera.ele < -90)
    b->camera.ele = -90;
  b->draw_scene();
  return TRUE;
}

gint ViewBox::on_glarea_keypress(GtkWidget *widget, GdkEventExpose *event, gpointer data)
{
  ViewBox *b = static_cast<ViewBox*>(data);
  if(!b->ready)
    return FALSE;
  return TRUE;
}

gint ViewBox::on_glarea_keyrelease(GtkWidget *widget, GdkEventExpose *event, gpointer data)
{
  ViewBox *b = static_cast<ViewBox*>(data);
  if(!b->ready)
    return FALSE;
  return TRUE;
}

gint ViewBox::on_glarea_scroll(GtkWidget *widget, GdkEventScroll *event, gpointer data)
{
  ViewBox *b = static_cast<ViewBox*>(data);
  if(!b->ready)
    return FALSE;
  if(event->direction == GDK_SCROLL_UP) {
    if(b->camera.d > 10)
      b->camera.d -= 40;
    b->draw_scene();
  } else if(event->direction == GDK_SCROLL_DOWN) {
    if(b->camera.d < 1500)
      b->camera.d += 40;
    b->draw_scene();
  }
  return TRUE;
}
