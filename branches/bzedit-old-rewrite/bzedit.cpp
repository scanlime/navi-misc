#include <gnome.h>
#include "application.h"
#include "globals.h"

int main(int argc, char **argv)
{
  GnomeProgram *program;
  program = gnome_program_init("bzedit", "3.0", LIBGNOMEUI_MODULE, argc, argv, NULL);

  doc = new Document;

  App app;

  gtk_main();
}
