#include <gtk/gtk.h>
#include <glade/glade.h>
#include "editor.h"
#include <GL/gl.h>

int main(int argc, char **argv) {
  Editor *editor;

  gtk_init(&argc, &argv);
  gtk_gl_init(&argc, &argv);

  editor = editor_new();

  gtk_main();
}
