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

    def genTempBuffer(self, size=(256,256)):
        b = loopy.RenderTarget(*size)
        b.bind(GL.glGenTextures(1))
        GL.glTexParameterf(GL.GL_TEXTURE_2D, GL.GL_TEXTURE_WRAP_S, GL.GL_CLAMP)
        GL.glTexParameterf(GL.GL_TEXTURE_2D, GL.GL_TEXTURE_WRAP_T, GL.GL_CLAMP)
        return b

    def setup(self):
        self.src = loadTexture("images/icon_ps2.png")

        self.t1 = self.genTempBuffer()
        self.t2 = self.genTempBuffer()

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

    def drawIcons(self, offset=(0,0), scale=(0.3, 0.4), shadow=False):
        GL.glBindTexture(GL.GL_TEXTURE_2D, self.src)
        GL.glColor(1,1,1,1)
        if shadow:
            GL.glBlendFunc(GL.GL_ZERO, GL.GL_ONE_MINUS_SRC_ALPHA)
        else:
            GL.glBlendFunc(GL.GL_SRC_ALPHA, GL.GL_ONE_MINUS_SRC_ALPHA)

        GL.glLoadIdentity()
        GL.glTranslatef(offset[0], offset[1], 0)
        GL.glScalef(scale[0], scale[1], 1)
        GL.glCallList(self.square)

    def drawShadowMask(self, buffer):
        """In the given buffer, draw a mask which will eventually
           be multiplied by the image to generate a shadow.
           """
        GL.glViewport(0, 0, buffer.w, buffer.h)
        buffer.lock()

        GL.glClearColor(1,1,1,1)
        GL.glClear(GL.GL_COLOR_BUFFER_BIT)

        self.drawIcons(shadow=True)

        buffer.unlock()
        GL.glViewport(0, 0, *self.resolution)

    def drawBlurredShadowMask(self, buffer, tempBuffer,
                              boxSize=0.005, boxSteps=5, iters=2, opacity=0.6):
        self.drawShadowMask(buffer)

        # Set up accumulation
        a = 1.0 / boxSteps
        GL.glViewport(0, 0, buffer.w, buffer.h)
        GL.glColor(a,a,a,a)
        GL.glClearColor(0,0,0,0)
        GL.glBlendFunc(GL.GL_ONE, GL.GL_ONE)
        GL.glLoadIdentity()
        GL.glMatrixMode(GL.GL_TEXTURE)

        for j in xrange(iters):
            # Vertical
            tempBuffer.lock()
            GL.glClear(GL.GL_COLOR_BUFFER_BIT)
            GL.glBindTexture(GL.GL_TEXTURE_2D, buffer.texture)
            for i in xrange(boxSteps):
                GL.glLoadIdentity()
                GL.glTranslatef(0, (i - boxSteps / 2.0) * boxSize, 0)
                GL.glCallList(self.square)
            tempBuffer.unlock()

            # Horizontal
            buffer.lock()
            GL.glClear(GL.GL_COLOR_BUFFER_BIT)
            GL.glBindTexture(GL.GL_TEXTURE_2D, tempBuffer.texture)
            for i in xrange(boxSteps):
                GL.glLoadIdentity()
                GL.glTranslatef((i - boxSteps / 2.0) * boxSize, 0, 0)
                GL.glCallList(self.square)
            buffer.unlock()

        # Blend a white square onto the shadow mask to limit its overall
        # opacity. FIXME: Is there a way to do this while blending the
        # completed shadow mask onto the scene below?
        buffer.lock()
        GL.glColor(1,1,1,1-opacity)
        GL.glBlendFunc(GL.GL_SRC_ALPHA, GL.GL_ONE_MINUS_SRC_ALPHA)
        GL.glDisable(GL.GL_TEXTURE_2D)
        GL.glCallList(self.square)
        GL.glEnable(GL.GL_TEXTURE_2D)
        buffer.unlock()

        GL.glLoadIdentity()
        GL.glMatrixMode(GL.GL_MODELVIEW)
        GL.glViewport(0, 0, *self.resolution)

    t = 0
    def render(self):
        self.t += 1
        h = (math.sin(self.t * 0.1) + 1) * 0.05

        self.drawBlurredShadowMask(self.t1, self.t2, boxSize= h * 0.1)

        # Blend the completed shadow mask
        GL.glBindTexture(GL.GL_TEXTURE_2D, self.t1.texture)
        GL.glBlendFunc(GL.GL_DST_COLOR, GL.GL_ZERO)
        GL.glColor(1,1,1,1)
        GL.glLoadIdentity()
        GL.glCallList(self.square)

        self.drawIcons(offset=(-h, h))

if __name__ == "__main__":
    loopy.overlays.append(Overlay())

