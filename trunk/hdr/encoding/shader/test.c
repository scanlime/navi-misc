#include <stdlib.h>
#include <SDL.h>
#include <GL/gl.h>
#include <Cg/cg.h>
#include <Cg/cgGL.h>

SDL_Surface *screen;

int load_mesh(const char *filename) {
  /* Load a .mesh file into a display list
   */
  SDL_RWops* rw;
  int num_vertices, num_triangles, list, i;
  float* vertices;

  rw = SDL_RWFromFile(filename, "rb");
  if (!rw) {
    printf("Error opening file '%s'\n", filename);
    exit(1);
  }

  /* Read the tiny header */
  num_vertices = SDL_ReadBE32(rw);
  num_triangles = SDL_ReadBE32(rw);

  /* Read vertices directly into a glInterleavedArray */
  vertices = malloc(sizeof(float) * 6 * num_vertices);
  SDL_RWread(rw, vertices, sizeof(float) * 6, num_vertices);
  glInterleavedArrays(GL_N3F_V3F, 0, vertices);
  glEnableClientState(GL_VERTEX_ARRAY);
  glEnableClientState(GL_NORMAL_ARRAY);

  /* Inside a display list, draw all triangles */
  list = glGenLists(1);
  glNewList(list, GL_COMPILE);
  glBegin(GL_TRIANGLES);
  for (i=0; i<num_triangles; i++) {
    glArrayElement(SDL_ReadBE32(rw));
    glArrayElement(SDL_ReadBE32(rw));
    glArrayElement(SDL_ReadBE32(rw));
  }
  glEnd();
  glEndList();

  glDisableClientState(GL_VERTEX_ARRAY);
  glDisableClientState(GL_NORMAL_ARRAY);
  free(vertices);
  SDL_FreeRW(rw);
  return list;
}

void scene_init() {
  glClearDepth(1.0);
  glDepthFunc(GL_LESS);
  glEnable(GL_DEPTH_TEST);
  glClearColor(0.25, 0.25, 0.25, 1.0);

  glViewport(0, 0, screen->w, screen->h);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(30.0, ((float)screen->w) / screen->h, 0.1, 100);

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  glEnable(GL_LIGHTING);
  {
    float diffuse[] = {0.6, 0.6, 0.5, 0};
    float specular[] = {1.0, 1.0, 1.0, 0};
    float position[] = {1, 1, -1, 1};
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, specular);
    glLightfv(GL_LIGHT0, GL_POSITION, position);
    glEnable(GL_LIGHT0);
  }
}

void scene_draw() {
  static int bunny = 0;
  static float angle = 0;
  if (!bunny)
    bunny = load_mesh("stanford_bunny.mesh");

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  glTranslatef(0, -0.3, -9);

  glRotatef(angle += 1.0, 0, 1, 0);
  glRotatef(-90, 1, 0, 0);
  glCallList(bunny);

  SDL_GL_SwapBuffers();
}

int main() {
  int done = 0;
  SDL_Event event;

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

  while (!done) {
    while (SDL_PollEvent(&event)) {
      switch (event.type) {

      case SDL_QUIT:
	done = 1;
	break;

      }
    }
    scene_draw();
  }

  SDL_Quit();
  return 0;
}

/* The End */
