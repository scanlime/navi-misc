import loopy, sys
from OpenGL import GL

class MyOverlay(loopy.Overlay):
    def setup(self):
        print "argv: " + repr(sys.argv)

        import pygame
        pygame.init()
        print pygame.display.Info()
        print pygame.display.set_mode((128,64), 0)

        # Demonstrate voodoo-like tracking of the local GL state
        print self.glState.capabilities
        GL.glEnable(GL.GL_LIGHTING)
        print self.glState.capabilities
        GL.glDisable(GL.GL_LIGHTING)
        print self.glState.capabilities

loopy.overlays.append(MyOverlay())
