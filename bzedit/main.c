#include <gtk/gtk.h>
#include <gtk/gtkgl.h>
#include <glade/glade.h>
#include <GL/gl.h>

int main(int argc, char **argv) {
  GladeXML *xml;
  GtkWidget *widget;
  GdkGLConfig *config;
  GdkGLContext *context;
  GdkGLDrawable *drawable;

  gtk_init(&argc, &argv);
  gtk_gl_init(&argc, &argv);

  xml = glade_xml_new("bzedit.glade", NULL, NULL);
  if(!xml)
    return 1;

  widget = glade_xml_get_widget(xml, "drawing area");

  config = gdk_gl_config_new_by_mode(GDK_GL_MODE_RGBA | GDK_GL_MODE_DEPTH | GDK_GL_MODE_DOUBLE);
  gtk_widget_set_gl_capability(widget, config, NULL, TRUE, GDK_GL_RGBA_TYPE);
  gtk_widget_add_events(widget, GDK_VISIBILITY_NOTIFY_MASK);

  if(gtk_widget_is_gl_capable(widget))
    g_print("gl config succeeded\n");

  widget = glade_xml_get_widget(xml, "window1");
  gtk_widget_show_all(widget);

  widget = glade_xml_get_widget(xml, "drawing area");

  context = gtk_widget_get_gl_context(widget);
  drawable = gtk_widget_get_gl_drawable(widget);
  gdk_gl_drawable_make_current(drawable, context);

  glClearColor(0.0, 0.0, 0.0, 0.0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  gdk_gl_drawable_swap_buffers(drawable);

  gtk_main();
}
