#!/usr/bin/env python
from BZEngine.UI import Viewport, ThreeDRender, HUD, Layout
from BZEngine import Event
import cPickle

loop = Event.EventLoop()
viewport = Viewport.OpenGLViewport(loop)
view = ThreeDRender.View(viewport)
viewport.setCaption("Wasabi")

viewport.mode = Viewport.GL.ClearedMode(clearColor=(0, 0, 0, 1))

view.camera.position = (0,0,3)
view.camera.elevation = 0
view.camera.distance = 7
view.camera.jump()

title = HUD.Text(viewport.region(viewport.rect), "Wasabi", color=(1,1,1,1), fontSize=70, alignment=(0.5, 0.75))

view.scene.add(cPickle.load(open("data/green_flame.particle")))

loop.run()
