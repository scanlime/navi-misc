#!/usr/bin/env python
#
# Simple viewer for the 3D models PyBZEngine uses, in a subset of the VRML format.
# Give the file name or URL to a .wrl model on the command line.
#
from BZEngine.UI import Viewport, ThreeDRender, ThreeDControl
from BZEngine.UI.Drawable import VRML
from BZEngine import Event, optik
import sys, os


# Command line processing
parser = optik.OptionParser(usage = "usage: %prog [options] model.wrl")
(options, args) = parser.parse_args()

try:
    fileName = args[0]
except IndexError:
    parser.error("A model filename must be specified on the command line.")

viewClass = ThreeDRender.View


# Set up a quick 3D renderer view with a controller attached so we can spin the model around
loop = Event.EventLoop()
viewport = Viewport.OpenGLViewport(loop)
viewport.setCaption("Model Viewer - %s" % fileName)
view = viewClass(viewport)
control = ThreeDControl.Viewing(view, viewport)

# modify the clear color
viewport.mode = Viewport.GL.ClearedMode(clearColor=(0.2, 0.2, 0.2, 1))

# Move the camera a little closer in, and down to the origin
view.camera.position = (0,0,0)
view.camera.distance = 60
view.camera.jump()

# Read in the VRML file, putting each mesh in a separate object.
# Note the abspath()- model and texture names are relative to our data directory
# by default, this makes them relative to our caller's current directory.
meshes = VRML.load(os.path.abspath(fileName))
class MeshObject:
    def __init__(self, name):
        self.name = name
    def __repr__(self):
        return "<%s %r>" % (self.__class__.__name__, self.name)
    def getDrawables(self):
        return [meshes[self.name]]
for name, drawable in meshes.items():
    view.scene.add(MeshObject(name))


# Let the user pick objects with the left mouse button, printing the mesh name
def onMouseButtonDown(event):
    if event.button == 1:
        print 'picked: %r' % view.pick(event.pos)
viewport.onMouseButtonDown.observe(onMouseButtonDown)

loop.run()
