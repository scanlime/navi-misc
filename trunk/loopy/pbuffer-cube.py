from __future__ import division
import loopy
from OpenGL import GL
from OpenGL import GLU

class Overlay(loopy.Overlay):
    def resized(self):
        width, height = self.resolution

        GL.glViewport(0, 0, width, height)
        GL.glMatrixMode(GL.GL_PROJECTION)
        GL.glLoadIdentity()
        GLU.gluPerspective(45, width / height, 0.1, 100.0)
        GL.glMatrixMode(GL.GL_MODELVIEW)
        GL.glLoadIdentity()

    def setup(self):
        self.texture = GL.glGenTextures(1)
        self.buffer = loopy.RenderTarget(256, 256)
        self.buffer.bind(self.texture)

        self.xrot, self.yrot, self.zrot = 0,0,0
        GL.glClearDepth(1.0)
        GL.glEnable(GL.GL_DEPTH_TEST)
        GL.glDepthFunc(GL.GL_LEQUAL)

        # Loopy disables color buffer clearing in our context, by default
        GL.glClearColor(0.2, 0.2, 0.2, 1)
        self.glState.clearMask = -1

        self.resized()

    def render(self):
        self.renderTexture()

        GL.glClear(GL.GL_DEPTH_BUFFER_BIT)
        GL.glColor4f(1,1,1,1)
        GL.glEnable(GL.GL_TEXTURE_2D)
        self.drawCube(GL.GL_QUADS)
        GL.glDisable(GL.GL_TEXTURE_2D)

        self.xrot += 0.2
        self.yrot += 0.2
        self.zrot += 0.2

    def renderTexture(self):
        self.buffer.lock()
        GL.glViewport(0, 0, self.buffer.w, self.buffer.h)

        GL.glClear(GL.GL_COLOR_BUFFER_BIT | GL.GL_DEPTH_BUFFER_BIT);
        GL.glColor4f(0.5, 1.0, 0.5, 1.0)
        self.drawCube(GL.GL_LINE_LOOP)

        self.buffer.unlock()
        GL.glViewport(0, 0, self.resolution[0], self.resolution[1])

    def drawCube(self, t):
        GL.glLoadIdentity()
        GL.glTranslatef(0.0,0.0,-5.0)
        GL.glRotatef(self.xrot, 1.0,0.0,0.0)
        GL.glRotatef(self.yrot, 0.0,1.0,0.0)
        GL.glRotatef(self.zrot, 0.0,0.0,1.0)

        GL.glBegin(t)
        for tx, ty, x, y, z in (
            (0.0, 0.0, -1.0, -1.0,  1.0),
            (1.0, 0.0,  1.0, -1.0,  1.0),
            (1.0, 1.0,  1.0,  1.0,  1.0),
            (0.0, 1.0, -1.0,  1.0,  1.0),

            (1.0, 0.0, -1.0, -1.0, -1.0),
            (1.0, 1.0, -1.0,  1.0, -1.0),
            (0.0, 1.0,  1.0,  1.0, -1.0),
            (0.0, 0.0,  1.0, -1.0, -1.0),

            (0.0, 1.0, -1.0,  1.0, -1.0),
            (0.0, 0.0, -1.0,  1.0,  1.0),
            (1.0, 0.0,  1.0,  1.0,  1.0),
            (1.0, 1.0,  1.0,  1.0, -1.0),

            (1.0, 1.0, -1.0, -1.0, -1.0),
            (0.0, 1.0,  1.0, -1.0, -1.0),
            (0.0, 0.0,  1.0, -1.0,  1.0),
            (1.0, 0.0, -1.0, -1.0,  1.0),

            (1.0, 0.0,  1.0, -1.0, -1.0),
            (1.0, 1.0,  1.0,  1.0, -1.0),
            (0.0, 1.0,  1.0,  1.0,  1.0),
            (0.0, 0.0,  1.0, -1.0,  1.0),

            (0.0, 0.0, -1.0, -1.0, -1.0),
            (1.0, 0.0, -1.0, -1.0,  1.0),
            (1.0, 1.0, -1.0,  1.0,  1.0),
            (0.0, 1.0, -1.0,  1.0, -1.0),
            ):
            GL.glTexCoord2f(tx, ty)
            GL.glVertex3f(x, y, z)
        GL.glEnd()

if __name__ == "__main__":
    loopy.overlays.append(Overlay())

