#!/usr/bin/env python
from BZEngine.UI import Viewport, ThreeDRender, ThreeDControl
from BZEngine import Event
import cPickle

loop = Event.EventLoop()
viewport = Viewport.OpenGLViewport(loop)
view = ThreeDRender.View(viewport)
control = ThreeDControl.Viewing(view, viewport)
viewport.setCaption("Sparks")

viewport.mode = Viewport.GL.ClearedMode(clearColor=(0, 0, 0, 1))

view.camera.position = (0,0,5)
view.camera.elevation = 0
view.camera.distance = 17
view.camera.jump()

view.scene.add(cPickle.load(open("data/welding_sparks.particle")))
view.scene.add(cPickle.load(open("data/smoke.particle")))

loop.run()
