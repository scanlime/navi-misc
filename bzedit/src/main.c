#include <gtk/gtk.h>
#include <glade/glade.h>
#include "gldrawingarea.h"
#include "parameter-editor.h"
#include <GL/gl.h>

int main(int argc, char **argv) {
  GladeXML *xml;
  GtkWidget *widget, *darea, *peditor;
  GdkGLConfig *config;

  gtk_init(&argc, &argv);
  gtk_gl_init(&argc, &argv);

  xml = glade_xml_new("data/bzedit.glade", NULL, NULL);
  if(!xml)
    return 1;

  config = gdk_gl_config_new_by_mode(GDK_GL_MODE_RGBA | GDK_GL_MODE_DEPTH | GDK_GL_MODE_DOUBLE);

  widget = glade_xml_get_widget (xml, "event box");
  darea = gl_drawing_area_new (config);
  gtk_container_add(GTK_CONTAINER(widget), darea);

  widget = glade_xml_get_widget (xml, "side vbox");
  peditor = gtk_drawing_area_new();
  gtk_box_pack_start(GTK_BOX(widget), peditor, FALSE, TRUE, 0);

  widget = glade_xml_get_widget (xml, "editor window");
  gtk_widget_show_all (widget);

  while(g_main_context_pending(NULL))
    g_main_context_iteration(NULL, TRUE);

  gl_drawing_area_make_current(GL_DRAWING_AREA(darea));

  glClearColor (0.0, 0.0, 0.0, 0.0);
  glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  gl_drawing_area_swap_buffers(GL_DRAWING_AREA(darea));

  gtk_main();
}
