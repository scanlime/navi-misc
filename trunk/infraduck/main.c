#include <pgserver/common.h>
#include <pgserver/init.h>
#include <pgserver/requests.h>
#include <pgserver/os.h>
#include <pgserver/handle.h>
#include <pgserver/configfile.h>
#include <GL/gl.h>
#include <stdio.h>

void gameloop_iteration(void) {
  /* This be where the thingamabobs shall live */
}

g_error protected_main(int argc, char **argv) {
  g_error e;

  e = set_param_str("pgserver", "video", "sdlgl");
  errorcheck;
  e = set_param_str("pgserver", "themedir", "data");
  errorcheck;
  e = set_param_str("pgserver", "themes", "gl_lucid.th");
  errorcheck;
  e = set_param_str("pgserver", "font_engine", "ftgl");
  errorcheck;
  e = set_param_str("font-ftgl", "path", "data/fonts");
  errorcheck;
  e = set_param_str("pgserver", "dragsolid", "1");
  errorcheck;
  e = set_param_str("video-sdlgl", "caption", "Infraduck XP++ Professional Edition");
  errorcheck;
  e = set_param_str("opengl", "continuous", "1");
  errorcheck;

  e = pgserver_init(PGINIT_NO_CONFIGFILE, argc, argv);
  errorcheck;
  pgserver_mainloop_start();

  while (pgserver_mainloop_is_running()) {
    e = pgserver_mainloop_iteration();
    errorcheck;
    gameloop_iteration();
  }

  e = pgserver_shutdown();
  errorcheck;

  return success;
}

int main(int argc, char **argv) {
  g_error e;
  e = protected_main(argc, argv);
  if(iserror(e)) {
    os_show_error(e);
    return 1;
  }
  return 0;
}
