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

void encoding_pattern(float s, float t, float* color) {
  int x = s * 3 + 0.5;
  int y = (1-t) * 3 + 0.5;
  int i = x + y*4;

  if (i < 8)
    color[0] = (1 << ( 7-(i-8) + 1)) / 255.0;
  else
    color[0] = 2.0 / 255.0;

  color[2] = i==0 ? 1.0 : 0.0;
}

void lsb_pattern(float s, float t, float* color) {
  int x = s * 255 + 0.5;
  x &= 0xFE;
  if (t > 0.5)
    x += 1;
  color[0] = x / 255.0;
}

void scene_init() {
  GLhandleARB program;
  int encoding_pattern_lut, lsb_pattern_lut;

  float diffuse[] = {0.5, 0.5, 0.52, 0};
  float specular[] = {1.0, 1.0, 0.8, 0};
  float position[] = {1, 1, -1, 1};
  float m_ambient[]  = {0, 0, 0, 0};
  float m_diffuse[]  = {1, 1, 1, 0};
  float m_specular[] = {0.8, 0.8, 0.8, 0};

  /* Compile, link, and activate our shaders */
  program = shader_link_program(shader_compile_from_files(GL_FRAGMENT_SHADER_ARB,
							  DATADIR "/test.frag",
							  NULL),
				0);

  encoding_pattern_lut = shader_create_lookup_table(4, 4, GL_REPEAT, GL_NEAREST, encoding_pattern);
  lsb_pattern_lut = shader_create_lookup_table(256, 2, GL_CLAMP, GL_NEAREST, lsb_pattern);

  glUseProgramObjectARB(program);
  shader_install_texture(program, "EncodingPattern", 0, encoding_pattern_lut);
  shader_install_texture(program, "LSBPattern", 1, lsb_pattern_lut);

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

void background_draw() {
  const float depth = 1.0;

  glDisable(GL_LIGHTING);
  glMatrixMode(GL_PROJECTION);
  glPushMatrix();
  glLoadIdentity();
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  glBegin(GL_QUADS);
  glColor3f(0, 0, 0.2);
  glVertex3f(-1,-1, depth);
  glVertex3f( 1,-1, depth);
  glColor3f(0.5, 0.5, 0.5);
  glVertex3f( 1, 1, depth);
  glVertex3f(-1, 1, depth);
  glEnd();

  glMatrixMode(GL_PROJECTION);
  glPopMatrix();
  glMatrixMode(GL_MODELVIEW);
  glEnable(GL_LIGHTING);
}

void scene_draw(float dt) {
  glClear(GL_DEPTH_BUFFER_BIT);

  glLoadIdentity();
  glTranslatef(0, 0, -9);

  glRotatef(model_y, 1, 0, 0);
  glRotatef(model_x, 0, 1, 0);

  glRotatef(-90, 1, 0, 0);
  model_draw();

  /* Draw the background last, to reduce overdraw */
  background_draw();

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
    printf("\r%8.02f FHz  (Effective fill rate: %.02f Pixel MHz)", fhz, fhz * screen->w * screen->h / 1000000.0);
    fflush(stdout);

    n_frames = 0;
    start_time = now;
  }
}

int handle_events(float delta_t) {
  SDL_Event event;
  static float model_vx = 3;
  static float model_vy = 0;
  static int spin_active = 1;
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
      model_vx = model_vy = 0;
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
