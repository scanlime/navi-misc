#!/usr/bin/env python
from BZEngine.UI import Viewport, ThreeDRender, ThreeDControl, HUD
from BZEngine import Event
import cPickle

loop = Event.EventLoop()
viewport = Viewport.OpenGLViewport(loop)
view = ThreeDRender.View(viewport)
control = ThreeDControl.Viewing(view, viewport)
viewport.setCaption("Wasabi")

viewport.mode = Viewport.GL.ClearedMode(clearColor=(0, 0, 0, 1))

view.camera.position = (0, 0, 4)
view.camera.distance = 2.09
view.camera.azimuth = 0
view.camera.elevation = -25.7
view.camera.jump()

title = HUD.Text(viewport.region(viewport.rect), "wasabi",
                 color     = (0, 0, 0, 0.75),
                 fontSize  = 100,
                 alignment = (0.5, 0.5),
                 fontName  = "geodesic.ttf")

view.scene.add(cPickle.load(open("data/green_flame.particle")))

loop.run()
