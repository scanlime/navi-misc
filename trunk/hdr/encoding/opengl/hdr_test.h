#ifndef __HDR_TEST_H
#define __HDR_TEST_H

#define GL_GLEXT_PROTOTYPES

#include <stdio.h>
#include <stdlib.h>
#include <SDL.h>
#include <SDL/SDL_endian.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glext.h>

/* model.c */
void model_draw();
void model_switch();

/* shader.c */
GLhandleARB shader_compile_from_files(GLenum shaderType, ...);
GLhandleARB shader_link_program(GLhandleARB first_shader, ...);
void shader_show_info_log(GLhandleARB object);

#endif /* __HDR_TEST_H */
