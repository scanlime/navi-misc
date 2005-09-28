/* -*- Mode: C; c-basic-offset: 4; indent-tabs-mode: nil -*-
 *
 * blank-slate - This is a simple OpenGL app that draws a gray
 *               Gimp-style checkerboard. It's intended as a simple
 *               background app to use when developing Loopy overlays.
 */

#include <SDL.h>
#include <SDL_opengl.h>

int main(int argc, char **argv)
{
    int w = 640, h = 480, flags = SDL_OPENGL;
    float tw, th;
    int done = 0;
    SDL_Event event;
    unsigned char checkers[] = {
        0x55, 0x55, 0x55, 0xFF,  0xAA, 0xAA, 0xAA, 0xFF,
        0xAA, 0xAA, 0xAA, 0xFF,  0x55, 0x55, 0x55, 0xFF,
    };

    if (!SDL_SetVideoMode(w, h, 0, flags)) {
        fprintf(stderr, "SDL Video Error: %s\n", SDL_GetError());
        return 1;
    }

    SDL_WM_SetCaption("Loopy - Blank Slate", NULL);
    glViewport(0, 0, w, h);

    /* Calculate texture coordinates- sets the tile size */
    tw = w / 32.0;
    th = h / 32.0;

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, 3, 2, 2, 0,
                 GL_RGBA, GL_UNSIGNED_BYTE, checkers);
    glEnable(GL_TEXTURE_2D);

    while (!done) {
        glBegin(GL_QUADS);
        glTexCoord2f(0,  0);  glVertex2i(-1, -1);
        glTexCoord2f(0,  th); glVertex2i(-1,  1);
        glTexCoord2f(tw, th); glVertex2i( 1,  1);
        glTexCoord2f(tw, 0);  glVertex2i( 1, -1);
        glEnd();
        SDL_GL_SwapBuffers();

        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT)
                done = 1;
        }
    }

    SDL_Quit();
    return 0;
}
