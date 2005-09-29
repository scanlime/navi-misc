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
        self.buffer = loopy.RenderTarget(256, 256)
        self.buffer.bind(0)

        self.xrot, self.yrot, self.zrot = 0,0,0
        GL.glClearDepth(1.0)
        GL.glEnable(GL.GL_DEPTH_TEST)
        GL.glDepthFunc(GL.GL_LEQUAL)
        GL.glEnable(GL.GL_TEXTURE_2D)

        self.resized()

    def render(self):
        self.buffer.lock()
        GL.glClear(GL.GL_DEPTH_BUFFER_BIT | GL.GL_COLOR_BUFFER_BIT)
        self.buffer.unlock()

        GL.glClear(GL.GL_DEPTH_BUFFER_BIT)
        GL.glLoadIdentity()
        GL.glTranslatef(0.0,0.0,-5.0)

        GL.glRotatef(self.xrot, 1.0,0.0,0.0)
        GL.glRotatef(self.yrot, 0.0,1.0,0.0)
        GL.glRotatef(self.zrot, 0.0,0.0,1.0)

        GL.glBegin(GL.GL_QUADS)
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

        self.xrot += 0.2
        self.yrot += 0.2
        self.zrot += 0.2

if __name__ == "__main__":
    loopy.overlays.append(Overlay())

