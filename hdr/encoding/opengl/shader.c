/*
 * shader.c - OpenGL Shading Language utilities
 *
 * Copyright (C) 2004 Micah Dowty <micah@navi.cx>
 *
 */

#include "hdr_test.h"
#include <stdarg.h>

static void alloc_string_from_file(const char* filename, GLcharARB **string, GLint *length);

int shader_create_lookup_table(int xres, int yres, int wrap, int filter,
			       void (*callback)(float s, float t, float* color)) {
  GLint texture;
  unsigned char *data, *p;
  int x, y, i;
  float color[4];

  data = malloc(4 * xres * yres);

  glGenTextures(1, &texture);
  glBindTexture(GL_TEXTURE_2D, texture);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter);

  p = data;
  for (y=0; y<yres; y++)
    for (x=0; x<xres; x++) {
      for (i=0; i<4; i++)
	color[i] = 0.0;
      callback(x / (xres - 1.0), y / (yres - 1.0), color);
      for (i=0; i<4; i++) {
	int c = color[i] * 255 + 0.5;
	if (c < 0)
	  c = 0;
	if (c > 255)
	  c = 255;
	*(p++) = c;
      }
    }

  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, xres, yres, 0, GL_RGBA,
	       GL_UNSIGNED_BYTE, data);
  free(data);
  return texture;
}

void shader_install_texture(GLhandleARB program, const char *var_name, int tex_unit, int texture) {
  glActiveTextureARB(GL_TEXTURE0 + tex_unit);
  glBindTexture(GL_TEXTURE_2D, texture);
  glUniform1iARB(glGetUniformLocationARB(program, var_name), tex_unit);
}

/* Create a new program object formed by linking the given 0-terminated list of shaders */
GLhandleARB shader_link_program(GLhandleARB first_shader, ...) {
  GLhandleARB program, shader;
  va_list other_shaders;
  GLint status;

  program = glCreateProgramObjectARB();

  /* Attach all shaders */
  glAttachObjectARB(program, first_shader);
  va_start(other_shaders, first_shader);
  while ((shader = va_arg(other_shaders, GLhandleARB)))
    glAttachObjectARB(program, shader);
  va_end(other_shaders);

  /* Link it and report errors */
  glLinkProgramARB(program);
  glGetObjectParameterivARB(program, GL_OBJECT_LINK_STATUS_ARB, &status);
  if (!status) {
    printf("*** Error linking shader:\n");
    shader_show_info_log(program);
    exit(1);
  }
  return program;
}

/* Load and compile shader from the concatenation of each file given. The list must be NULL-terminated. */
GLhandleARB shader_compile_from_files(GLenum shaderType, ...) {
  GLhandleARB shader;
  va_list files;
  const char *filename;
  GLcharARB **strings;
  GLint *lengths;
  GLint nstrings = 0, i;
  GLint status;

  /* Count the number of files */
  va_start(files, shaderType);
  while ((filename = va_arg(files, const char *)))
    nstrings++;
  va_end(files);

  /* Allocate buffers */
  strings = malloc(sizeof(GLcharARB*) * nstrings);
  lengths = malloc(sizeof(GLint) * nstrings);

  /* Load each file */
  va_start(files, shaderType);
  i = 0;
  while ((filename = va_arg(files, const char *))) {
    alloc_string_from_file(filename, &strings[i], &lengths[i]);
    i++;
  }
  va_end(files);

  /* Create and compile the shader object */
  shader = glCreateShaderObjectARB(shaderType);
  glShaderSourceARB(shader, nstrings, (const GLcharARB**) strings, lengths);
  glCompileShaderARB(shader);

  /* Check status and report errors */
  glGetObjectParameterivARB(shader, GL_OBJECT_COMPILE_STATUS_ARB, &status);
  if (!status) {
    /* Describe the shader that just failed */
    printf("*** Error compiling shader [ ");
    va_start(files, shaderType);
    while ((filename = va_arg(files, const char *)))
      printf("%s ", filename);
    va_end(files);
    printf("]:\n");
    shader_show_info_log(shader);
    exit(1);
  }

  /* Free the strings and the buffers themselves */
  for (i=0; i<nstrings; i++)
    free(strings[i]);
  free(strings);
  free(lengths);

  return shader;
}

void shader_show_info_log(GLhandleARB object) {
  int length;
  glGetObjectParameterivARB(object, GL_OBJECT_INFO_LOG_LENGTH_ARB, &length);
  if (length > 0) {
    GLcharARB *log;

    log = malloc(length+1);
    log[0] = 0;
    glGetInfoLogARB(object, length, NULL, log);
    log[length] = 0;
    printf("%s", log);
  }
}

/* Read a file into a freshly allocated counted string */
static void alloc_string_from_file(const char* filename, GLcharARB **string, GLint *length) {
  FILE *f;

  f = fopen(filename, "r");
  if (!f) {
    printf("Can't open '%s'\n", filename);
    exit(1);
  }

  /* Find the file size */
  fseek(f, 0, SEEK_END);
  *length = ftell(f);

  /* Read it all */
  *string = malloc(*length);
  fseek(f, 0, SEEK_SET);
  fread(*string, 1, *length, f);
}

/* The End */
