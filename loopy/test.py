import loopy
from OpenGL import GL

class MyOverlay(loopy.Overlay):
    def setup(self):
        # Demonstrate voodoo-like tracking of the local GL state
        print self.glState.capabilities
        GL.glEnable(GL.GL_LIGHTING)
        print self.glState.capabilities
        GL.glDisable(GL.GL_LIGHTING)
        print self.glState.capabilities

loopy.overlays.append(MyOverlay())
