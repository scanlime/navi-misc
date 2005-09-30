from __future__ import division
import loopy, pygame, os, sys, math
from OpenGL import GL

datadir = os.path.join(os.path.dirname(sys.argv[0]), "data")

def loadTexture(name):
    texturefile = os.path.join(datadir, name)
    textureSurface = pygame.image.load(texturefile)
    textureData = pygame.image.tostring(textureSurface, "RGBX", 1)
    texture = GL.glGenTextures(1)
    GL.glBindTexture(GL.GL_TEXTURE_2D, texture)
    GL.glTexImage2D(GL.GL_TEXTURE_2D, 0, GL.GL_RGBA,
                    textureSurface.get_width(), textureSurface.get_height(), 0,
                    GL.GL_RGBA, GL.GL_UNSIGNED_BYTE, textureData )
    GL.glTexParameterf(GL.GL_TEXTURE_2D, GL.GL_TEXTURE_MAG_FILTER, GL.GL_LINEAR)
    GL.glTexParameterf(GL.GL_TEXTURE_2D, GL.GL_TEXTURE_MIN_FILTER, GL.GL_LINEAR)
    return texture

class Overlay(loopy.Overlay):
    def resized(self):
        GL.glViewport(0, 0, *self.resolution)
        GL.glMatrixMode(GL.GL_PROJECTION)
        GL.glLoadIdentity()
        GL.glMatrixMode(GL.GL_MODELVIEW)
        GL.glLoadIdentity()

    def setup(self):
        self.src = loadTexture("images/icon_ps2.png")

        self.t1 = loopy.RenderTarget(256, 256)
        self.t1.bind(GL.glGenTextures(1))
        self.t2 = loopy.RenderTarget(256, 256)
        self.t2.bind(GL.glGenTextures(1))

        self.square = GL.glGenLists(1)
        GL.glNewList(self.square, GL.GL_COMPILE)
        GL.glBegin(GL.GL_QUADS)
        GL.glTexCoord2f(0.0, 0.0); GL.glVertex2f(-1.0, -1.0)
        GL.glTexCoord2f(0.0, 1.0); GL.glVertex2f(-1.0,  1.0)
        GL.glTexCoord2f(1.0, 1.0); GL.glVertex2f( 1.0,  1.0)
        GL.glTexCoord2f(1.0, 0.0); GL.glVertex2f( 1.0, -1.0)
        GL.glEnd()
        GL.glEndList()

        # Loopy disables color buffer clearing in our context, by default
        self.glState.clearMask = -1
        GL.glEnable(GL.GL_TEXTURE_2D)
        GL.glEnable(GL.GL_BLEND)

        self.resized()


    t = 0
    def render(self):
        self.t += 0.02
        blurAmount = abs(math.sin(self.t)) * 0.2
        nSteps = 16
        a = 0.01

        # Draw to buffer t1
        # The first stage masks off the source's RGB (using glColor)
        # alpha-blending blackness onto a white buffer.
        self.t1.lock()
        GL.glViewport(0, 0, self.t1.w, self.t1.h)
        GL.glClearColor(1,1,1,1)
        GL.glClear(GL.GL_COLOR_BUFFER_BIT)
        GL.glBindTexture(GL.GL_TEXTURE_2D, self.src)
        GL.glBlendFunc(GL.GL_SRC_ALPHA, GL.GL_ONE_MINUS_SRC_ALPHA)
        GL.glColor(0,0,0,a)

        # Vertical box filter
        for i in xrange(nSteps):
            GL.glLoadIdentity()
            GL.glTranslatef(0, 2.0 * blurAmount / (nSteps-1) * i - blurAmount, 0)
            GL.glScalef(1, 1-blurAmount, 1)
            GL.glCallList(self.square)

        # Draw to t2 buffer
        self.t1.unlock()
        self.t2.lock()
        GL.glClearColor(1,1,1,1)
        GL.glClear(GL.GL_COLOR_BUFFER_BIT)
        GL.glBindTexture(GL.GL_TEXTURE_2D, self.t1.texture)
        GL.glBlendFunc(GL.GL_DST_COLOR, GL.GL_ZERO)
        GL.glColor(1,1,1,1)

        # Horizontal box filter
        for i in xrange(nSteps):
            GL.glLoadIdentity()
            GL.glTranslatef(2.0 * blurAmount / (nSteps-1) * i - blurAmount, 0, 0)
            GL.glScalef(1-blurAmount, 1, 1)
            GL.glCallList(self.square)

        # Back to main buffer
        self.t2.unlock()
        GL.glViewport(0, 0, *self.resolution)

        GL.glBlendFunc(GL.GL_ZERO, GL.GL_SRC_COLOR)
        GL.glLoadIdentity()
        GL.glScalef(0.75, 0.75, 0.75)
        GL.glBindTexture(GL.GL_TEXTURE_2D, self.t2.texture)
        GL.glCallList(self.square)

if __name__ == "__main__":
    loopy.overlays.append(Overlay())

