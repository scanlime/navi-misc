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

view.camera.position = (1, 0.07, 4)
view.camera.distance = 1.9
view.camera.azimuth = 50.4
view.camera.elevation = 16.3
view.camera.jump()

title = HUD.Text(viewport.region(viewport.rect), "wasabi",
                 color     = (1, 1, 1, 1),
                 fontSize  = 100,
                 alignment = (0.8, 0.55),
                 fontName  = "geodesic.ttf")

view.scene.add(cPickle.load(open("data/green_flame.particle")))

loop.run()
