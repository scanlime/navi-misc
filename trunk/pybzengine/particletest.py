#!/usr/bin/env python
from BZEngine.UI import Drawable, Viewport, ThreeDRender, ThreeDControl, Environment, Tweak
from BZEngine import Event

if __name__ == '__main__':
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

    particleTest = Drawable.SimpleParticles((0,0,3))
    view.scene.add(particleTest)

    Tweak.Window(particleTest.getTweakControls(),
                 Tweak.Separator(),
                 sky.getTweakControls())

    Tweak.run(loop)
