/*
 * main.c - SDL frontend for the HDR encoding test
 *
 * Copyright (C) 2004 Micah Dowty <micah@navi.cx>
 *
 */

#include "hdr_test.h"
#include <math.h>

SDL_Surface *screen;
float model_x = 0;
float model_y = 0;


void scene_init() {
  GLhandleARB program;

  float diffuse[] = {0.5, 0.5, 0.52, 0};
  float specular[] = {1.0, 1.0, 0.8, 0};
  float position[] = {1, 1, -1, 1};
  float m_ambient[]  = {0, 0, 0, 0};
  float m_diffuse[]  = {1, 1, 1, 0};
  float m_specular[] = {0.8, 0.8, 0.8, 0};

  program = shader_link_program(shader_compile_from_files(GL_FRAGMENT_SHADER_ARB,
							  DATADIR "/test.frag",
							  NULL),
				0);

  glUseProgramObjectARB(program);

  glViewport(0, 0, screen->w, screen->h);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(30.0, ((float)screen->w) / screen->h, 0.1, 100.0);

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  glClearDepth(1);
  glDepthFunc(GL_LESS);
  glShadeModel(GL_SMOOTH);
  glFrontFace(GL_CCW);
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);
  glClearColor(0, 0, 0, 1);

  glEnable(GL_LIGHTING);
  glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
  glLightfv(GL_LIGHT0, GL_SPECULAR, specular);
  glLightfv(GL_LIGHT0, GL_POSITION, position);
  glEnable(GL_LIGHT0);

  glMaterialfv(GL_FRONT, GL_AMBIENT, m_ambient);
  glMaterialfv(GL_FRONT, GL_DIFFUSE, m_diffuse);
  glMaterialfv(GL_FRONT, GL_SPECULAR, m_specular);
  glMaterialf(GL_FRONT, GL_SHININESS, 30);
}

void scene_draw(float dt) {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glLoadIdentity();
  glTranslatef(0, 0, -9);

  glRotatef(model_y, 1, 0, 0);
  glRotatef(model_x, 0, 1, 0);

  glRotatef(-90, 1, 0, 0);
  model_draw();

  SDL_GL_SwapBuffers();
}

void fhz_report(Uint32 now) {
  /* Measure and report the number of frames per second. Called every frame. */
  static Uint32 start_time = 0;
  const Uint32 interval = 1000;
  static int n_frames = 0;
  float fhz;

  n_frames++;
  if (now > start_time + interval) {
    fhz = n_frames * 1000.0 / (now - start_time);
    printf("\r%8.02f FHz ", fhz);
    fflush(stdout);

    n_frames = 0;
    start_time = now;
  }
}

int handle_events(float delta_t) {
  SDL_Event event;
  static float model_vx, model_vy;
  static int spin_active = 0;
  const float mouse_speed = 0.6;

  while (SDL_PollEvent(&event)) {
    switch (event.type) {

    case SDL_QUIT:
      return 0;
      break;

    case SDL_KEYDOWN:
      if (event.key.keysym.sym == SDLK_SPACE)
	model_switch();
      break;

    case SDL_MOUSEBUTTONDOWN:
      /* Stop it spinning when the user clicks */
      spin_active = 0;
      break;

    case SDL_MOUSEBUTTONUP:
      /* Allow spinning again when the user isn't draggin */
      spin_active = 1;
      break;

    case SDL_MOUSEMOTION:
      if (event.motion.state) {
	model_vy = event.motion.yrel * mouse_speed;
	model_vx = event.motion.xrel * mouse_speed;
	model_y += model_vy;
	model_x += model_vx;
      }
      break;
    }
  }

  if (spin_active) {
    model_x += model_vx * delta_t * 10;
    model_y += model_vy * delta_t * 10;
  }

  return 1;
}

int main() {
  float delta_t;
  Uint32 now = 0, then = 0;

  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    printf("Error initializing SDL: %s\n", SDL_GetError());
    return 1;
  }

  screen = SDL_SetVideoMode(640, 480, 0, SDL_OPENGL);
  if (!screen) {
    printf("Error setting video mode: %s\n", SDL_GetError());
    SDL_Quit();
    return 1;
  }

  SDL_WM_SetCaption("Hardware HDR encoding test", "hdrtest");

  scene_init();

  do {
    then = now;
    now = SDL_GetTicks();
    delta_t = (now - then) / 1000.0;
    fhz_report(now);

    scene_draw(delta_t);

  } while (handle_events(delta_t));

  printf("\n");
  SDL_Quit();
  return 0;
}

/* The End */
