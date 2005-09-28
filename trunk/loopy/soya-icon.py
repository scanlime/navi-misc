#
# Demo of an icon with drop shadow.
#
# Note that the icon used is an original work Copyright (C) 2005 Micah Dowty.
# This example is public domain code, but please don't use icons without asking :)
#

import os, math, random, sys
import loopy, soya, Numeric
import soya.pudding as pudding
import soya.opengl as GL

class Icon(pudding.control.Control):
    def __init__(self, parent, image, aspect=1):
        pudding.control.Control.__init__(self, parent)
        self.image = image
        self.material = soya.Material(image)
        self.width = int(self.image.width * aspect)
        self.height = self.image.height

    def on_resize(self):
        # FIXME: Centered on the screen just for testing
        self.left = (self.parent.width - self.width) / 2
        self.top = (self.parent.height - self.height) / 2

    def render_self(self):
        GL.glEnable(GL.GL_BLEND)
        self.material.activate()
        GL.glBegin(GL.GL_QUADS)
        w, h = self.width, self.height
        for x, y, color in self.kernel():
            GL.glColor4f(*color)
            GL.glTexCoord2f(0.0, 0.0); GL.glVertex2i(x,   y)
            GL.glTexCoord2f(0.0, 1.0); GL.glVertex2i(x,   y+h)
            GL.glTexCoord2f(1.0, 1.0); GL.glVertex2i(x+w, y+h)
            GL.glTexCoord2f(1.0, 0.0); GL.glVertex2i(x+w, y)
        GL.glEnd()
        GL.glDisable(GL.GL_BLEND)
        soya.DEFAULT_MATERIAL.activate()

    def kernel(self):
        n = 40
        a = 1.0 / n
        s = 4
        for i in xrange(n):
            x = random.uniform(0, 10)
            y = random.uniform(0, 10)
            yield (x, y, (0,0,0,gaussian(x+y, 5)))
        yield (0,0, (1,1,1,1))

def gaussian(x, s):
    return math.exp(-x*x/(2*s*s)) / (s*math.sqrt(2*math.pi))

class Overlay(loopy.Overlay):
    def createWidgets(self):
        root = pudding.core.RootWidget()

        icon = Icon(root, soya.Image.get("icon_ps2.png"), aspect=0.983)

        root.resize(0, 0, *self.resolution)
        return root

    def resized(self):
        soya.set_video(self.resolution[0], self.resolution[1], False, True)
        self.root.resize(0, 0, *self.resolution)

    def setup(self):
        soya.init("Loopy Demo", self.resolution[0], self.resolution[1])
        soya.path.append(os.path.join(os.path.dirname(sys.argv[0]), "data"))
        pudding.init()
        self.root = self.createWidgets()

    def render(self):
        self.root.render()

if __name__ == "__main__":
    loopy.overlays.append(Overlay())
