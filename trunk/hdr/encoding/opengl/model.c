/*
 * model.c - Cheesy support for loading test meshes from disk,
 *           drawing them, and toggling which model we're currently showing.
 *
 * Copyright (C) 2004 Micah Dowty <micah@navi.cx>
 *
 */

#include "hdr_test.h"

static int load_mesh(const char *filename);

static struct model {
  const char *filename;
  int d_list;
} models[] = {
  {DATADIR "/spiky.mesh",        0},
  {DATADIR "/suzanne.mesh",        0},
  {DATADIR "/stanford_bunny.mesh", 0},
};

static int current_model;

void model_draw() {
  struct model *current = &models[current_model];
  if (!current->d_list)
    current->d_list = load_mesh(current->filename);
  glCallList(current->d_list);
}

void model_switch() {
  current_model = (current_model + 1) % (sizeof(models) / sizeof(models[0]));
}

/* Load a .mesh file into a display list */
static int load_mesh(const char *filename) {
  SDL_RWops* rw;
  int num_vertices, num_triangles, list, i, j;
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
  for (i=0; i<num_triangles; i++)
    for (j=0; j<3; j++)
      glArrayElement(SDL_ReadBE32(rw));
  glEnd();
  glEndList();

  glDisableClientState(GL_VERTEX_ARRAY);
  glDisableClientState(GL_NORMAL_ARRAY);
  free(vertices);
  SDL_FreeRW(rw);
  return list;
}

/* The End */
