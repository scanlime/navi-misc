#include <gnome.h>
#include "../common/fe.h"

int fe_args (int argc, char *argv[]) {
  if(argc > 1) {
    if(!strcasecmp(argv[1], "--version") || !strcasecmp(argv[1], "-v"))
    {
      puts(VERSION);
      return 0;
    }
  }
  gnome_program_init("xchat test", "0.1", LIBGNOMEUI_MODULE, argc, argv, NULL);
  return 1;
}

void fe_init (void) {
}
