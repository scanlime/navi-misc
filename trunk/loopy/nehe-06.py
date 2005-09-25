#!/usr/bin/env python
# pygame + PyOpenGL version of Nehe's OpenGL lesson06
# Paul Furber 2001 - m@verick.co.za
# Loopy version - Micah Dowty

import os, loopy
from OpenGL.GL import *
from OpenGL.GLU import *
import pygame, pygame.image
from pygame.locals import *

class NeheOverlay(loopy.Overlay):
    def __init__(self):
        loopy.Overlay.__init__(self)
        self.xrot = 0.0
        self.yrot = 0.0
        self.zrot = 0.0

        # This nehe demo is a naughy app that doesn't use glGenTextures.
        # Well, a lot of our target apps do this also, so Loopy has to
        # be smart enough to remap textures to prevent collisions.
        self.textures = [0]

    def resized(self):
        width, height = self.resolution

        glViewport(0, 0, width, height)
        glMatrixMode(GL_PROJECTION)
        glLoadIdentity()
        gluPerspective(45, 1.0*width/height, 0.1, 100.0)
        glMatrixMode(GL_MODELVIEW)
        glLoadIdentity()

    def setup(self):
        video_flags = OPENGL|DOUBLEBUF

        # We don't actually need pygame for this demo, but this
        # is an example of how Loopy tricks SDL into working so
        # that 3D engines using pygame can run unmodified.
        pygame.init()
        surface = pygame.display.set_mode((640,480), video_flags)

        # Another example to show off Loopy-
        # normally your overlay wouldn't clear the color buffer,
        # but we tell Loopy to trap that, only allowing us to clear
        # the depth buffer. This can be used to noninvasively prevent
        # an existing 3D engine from clearing over the target app's output.
        self.glState.clearMask = ~GL_COLOR_BUFFER_BIT

        glEnable(GL_TEXTURE_2D)
        self.load_textures()
        glShadeModel(GL_SMOOTH)
        glClearColor(0.0, 0.0, 0.0, 0.0)
        glClearDepth(1.0)
        glEnable(GL_DEPTH_TEST)
        glDepthFunc(GL_LEQUAL)
        glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST)

        glEnable(GL_BLEND)
        glColor(1, 1, 1, 0.2)
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA)

        self.resized()

    def load_textures(self):
        texturefile = os.path.join('data','nehe.bmp')
        textureSurface = pygame.image.load(texturefile)
        textureData = pygame.image.tostring(textureSurface, "RGBX", 1)

        glBindTexture(GL_TEXTURE_2D, self.textures[0])
        glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA,
                      textureSurface.get_width(), textureSurface.get_height(), 0,
                      GL_RGBA, GL_UNSIGNED_BYTE, textureData );
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST)
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST)

    def render(self):
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT)
        glLoadIdentity()
        glTranslatef(0.0,0.0,-5.0)

        glRotatef(self.xrot, 1.0,0.0,0.0)
        glRotatef(self.yrot, 0.0,1.0,0.0)
        glRotatef(self.zrot, 0.0,0.0,1.0)

        glBegin(GL_QUADS)

        # Front Face (note that the texture's corners have to match the quad's corners)
        glTexCoord2f(0.0, 0.0); glVertex3f(-1.0, -1.0,  1.0)	# Bottom Left Of The Texture and Quad
        glTexCoord2f(1.0, 0.0); glVertex3f( 1.0, -1.0,  1.0)	# Bottom Right Of The Texture and Quad
        glTexCoord2f(1.0, 1.0); glVertex3f( 1.0,  1.0,  1.0)	# Top Right Of The Texture and Quad
        glTexCoord2f(0.0, 1.0); glVertex3f(-1.0,  1.0,  1.0)	# Top Left Of The Texture and Quad

        # Back Face
        glTexCoord2f(1.0, 0.0); glVertex3f(-1.0, -1.0, -1.0)	# Bottom Right Of The Texture and Quad
        glTexCoord2f(1.0, 1.0); glVertex3f(-1.0,  1.0, -1.0)	# Top Right Of The Texture and Quad
        glTexCoord2f(0.0, 1.0); glVertex3f( 1.0,  1.0, -1.0)	# Top Left Of The Texture and Quad
        glTexCoord2f(0.0, 0.0); glVertex3f( 1.0, -1.0, -1.0)	# Bottom Left Of The Texture and Quad

        # Top Face
        glTexCoord2f(0.0, 1.0); glVertex3f(-1.0,  1.0, -1.0)	# Top Left Of The Texture and Quad
        glTexCoord2f(0.0, 0.0); glVertex3f(-1.0,  1.0,  1.0)	# Bottom Left Of The Texture and Quad
        glTexCoord2f(1.0, 0.0); glVertex3f( 1.0,  1.0,  1.0)	# Bottom Right Of The Texture and Quad
        glTexCoord2f(1.0, 1.0); glVertex3f( 1.0,  1.0, -1.0)	# Top Right Of The Texture and Quad

        # Bottom Face
        glTexCoord2f(1.0, 1.0); glVertex3f(-1.0, -1.0, -1.0)	# Top Right Of The Texture and Quad
        glTexCoord2f(0.0, 1.0); glVertex3f( 1.0, -1.0, -1.0)	# Top Left Of The Texture and Quad
        glTexCoord2f(0.0, 0.0); glVertex3f( 1.0, -1.0,  1.0)	# Bottom Left Of The Texture and Quad
        glTexCoord2f(1.0, 0.0); glVertex3f(-1.0, -1.0,  1.0)	# Bottom Right Of The Texture and Quad

        # Right face
        glTexCoord2f(1.0, 0.0); glVertex3f( 1.0, -1.0, -1.0)	# Bottom Right Of The Texture and Quad
        glTexCoord2f(1.0, 1.0); glVertex3f( 1.0,  1.0, -1.0)	# Top Right Of The Texture and Quad
        glTexCoord2f(0.0, 1.0); glVertex3f( 1.0,  1.0,  1.0)	# Top Left Of The Texture and Quad
        glTexCoord2f(0.0, 0.0); glVertex3f( 1.0, -1.0,  1.0)	# Bottom Left Of The Texture and Quad

        # Left Face
        glTexCoord2f(0.0, 0.0); glVertex3f(-1.0, -1.0, -1.0)	# Bottom Left Of The Texture and Quad
        glTexCoord2f(1.0, 0.0); glVertex3f(-1.0, -1.0,  1.0)	# Bottom Right Of The Texture and Quad
        glTexCoord2f(1.0, 1.0); glVertex3f(-1.0,  1.0,  1.0)	# Top Right Of The Texture and Quad
        glTexCoord2f(0.0, 1.0); glVertex3f(-1.0,  1.0, -1.0)	# Top Left Of The Texture and Quad

        glEnd();

        self.xrot += 0.2
        self.yrot += 0.2
        self.zrot += 0.2

loopy.overlays.append(NeheOverlay())
