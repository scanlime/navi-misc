#!/usr/bin/env python
from BZFlag.Event import EventLoop
from BZFlag.UI import Viewport, GLNoise, Texture, ThreeDRender, ThreeDControl, Drawable
from BZFlag import Animated

loop = EventLoop()
viewport = Viewport.OpenGLViewport(loop)
viewport.setCaption("OpenGL Noise Experiment")
view = ThreeDRender.View(viewport)
control = ThreeDControl.Viewing(view, viewport)

viewport.mode = Viewport.GL.ClearedMode((0.25, 0.25, 0.5, 1))
view.camera.position = (0,0,0)
view.camera.elevation = 90
view.camera.distance = 150
view.camera.jump()

class TestObject:
    def __init__(self):
        ground = Drawable.Ground(400)
        ground.baseTexRepeats = 4
        ground.render.textures = (GLNoise.PerlinTexture(frameDuration=0),)
        self.drawables = [ground]

    def getDrawables(self):
        return self.drawables

view.scene.add(TestObject())
loop.run()
