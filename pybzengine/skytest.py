#!/usr/bin/env python
from BZEngine.UI import Viewport, ThreeDRender, ThreeDControl, Environment, Drawable
from BZEngine import Event

# Viewport, view, and event loop setup
loop = Event.EventLoop()
viewport = Viewport.OpenGLViewport(loop)
view = ThreeDRender.View(viewport)
control = ThreeDControl.Viewing(view, viewport)

viewport.setCaption("Sky Test")
viewport.mode = Viewport.GL.UnclearedMode()

# Move the camera to the center of the universe
view.camera.position = (0,0,0)
view.camera.distance = 0
view.camera.elevation = 0
view.camera.jump()

# Remap the mouse wheel to FOV zoom
control.bind(ThreeDControl.MouseWheel, -0.05, 'any').observe(control.fovZoom)

# Add the sky
sky = Environment.Sky()
view.scene.add(sky)
viewport.onSetupFrame.observe(sky.update)

loop.run()
