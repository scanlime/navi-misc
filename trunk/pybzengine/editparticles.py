#!/usr/bin/env python
#
# A very simple particle system editor. If a filename is specified on the command
# line, the particle system will be loaded and/or saved to that file.
#
from BZEngine.UI import Drawable, Viewport, ThreeDRender, ThreeDControl, Environment, Tweak
from BZEngine import Event
import sys, cPickle

loop = Event.EventLoop()
viewport = Viewport.OpenGLViewport(loop)
view = ThreeDRender.View(viewport)
control = ThreeDControl.Viewing(view, viewport)
viewport.setCaption("Particle System Test")

viewport.mode = Viewport.GL.ClearedMode(clearColor=(0.2, 0.2, 0.2, 1))

view.camera.position = (0,0,6)
view.camera.distance = 20
view.camera.jump()

sky = Environment.Sky()
view.scene.add(sky)
view.scene.add(Drawable.Ground(400))

try:
    # Load an existing system if we can
    filename = sys.argv[1]
    particles = cPickle.load(open(filename))
except:
    # Or create a blank one
    particles = Drawable.SimpleParticles((0,0,3))
view.scene.add(particles)

Tweak.Window(particles.getTweakControls(),
             Tweak.Separator(),
             sky.getTweakControls())

try:
    Tweak.run(loop)
finally:
    # Save on exit
    cPickle.dump(particles, open(filename, "w"))
