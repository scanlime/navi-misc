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
  vertices = malloc(sizeof(float) * 3 * num_vertices);
  SDL_RWread(rw, vertices, 1, sizeof(float) * 3 * num_vertices);
  glVertexPointer(3, GL_FLOAT, 0, vertices);
  glEnableClientState(GL_VERTEX_ARRAY);

  /* Inside a display list, draw all triangles */
  list = glGenLists(1);
  glNewList(list, GL_COMPILE);
  glBegin(GL_TRIANGLES);
  for (i=0; i<num_triangles; i++) {
    int a,b,c;
    float *a_vert, *b_vert, *c_vert;
    float ab_vert[3], ac_vert[3], normal[3];

    a = SDL_ReadBE32(rw);
    b = SDL_ReadBE32(rw);
    c = SDL_ReadBE32(rw);

    /* Calculate a face normal for now (AB x AC) */
    a_vert = &vertices[a * 3];
    b_vert = &vertices[b * 3];
    c_vert = &vertices[c * 3];
    ab_vert[0] = b_vert[0] - a_vert[0];
    ab_vert[1] = b_vert[1] - a_vert[1];
    ab_vert[2] = b_vert[2] - a_vert[2];
    ac_vert[0] = c_vert[0] - a_vert[0];
    ac_vert[1] = c_vert[1] - a_vert[1];
    ac_vert[2] = c_vert[2] - a_vert[2];
    normal[0] =  ab_vert[2] * ac_vert[3] - ab_vert[3] * ac_vert[2];
    normal[1] = -ab_vert[0] * ac_vert[3] + ab_vert[3] * ac_vert[0];
    normal[2] =  ab_vert[0] * ac_vert[1] - ab_vert[1] * ac_vert[0];

    

    glNormal3fv(normal);
    glArrayElement(a);
    glArrayElement(b);
    glArrayElement(c);
  }
  glEnd();
  glEndList();

  glDisableClientState(GL_VERTEX_ARRAY);
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
  glTranslatef(0, -0.105, -0.38);

  glEnable(GL_LIGHTING);
  glEnable(GL_NORMALIZE);
  {
    float diffuse[] = {0.7, 0.5, 0.7, 0};
    float specular[] = {1.0, 1.0, 1.0, 0};
    float position[] = {1, 1, -1, 1};
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, specular);
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

  glRotatef(0.2, 0, 1, 0);
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
