#!/usr/bin/env python
from BZEngine.UI import Viewport, ThreeDRender, ThreeDControl, HUD
from BZEngine import Event, Animated
import cPickle, copy, math

loop = Event.EventLoop()
viewport = Viewport.OpenGLViewport(loop)
view = ThreeDRender.View(viewport)
control = ThreeDControl.Viewing(view, viewport)
viewport.setCaption("Wasabi")

viewport.mode = Viewport.GL.ClearedMode(clearColor=(0, 0, 0, 1))

view.camera.position = (0, 0, 1)
view.camera.distance = 10
view.camera.azimuth = 0
view.camera.elevation = 1.5
view.camera.jump()

title = HUD.Text(viewport.region(viewport.rect), "wasabi",
                 color     = (0, 0, 0, 0.75),
                 fontSize  = viewport.size[1] / 6,
                 alignment = (0.5, 0.5),
                 fontName  = "geodesic.ttf")

flame1 = cPickle.load(open("data/green_flame.particle"))
flame2 = copy.copy(flame1)
view.scene.add(flame1)
view.scene.add(flame2)

radius = 4
angle = Animated.Value(Animated.RampFunction(2, (0, 2*math.pi)))
time = Animated.Timekeeper()
def setupFrame():
    angle.integrate(time.step())
    flame1.emitter.position = (math.cos(angle.value) * radius,
                               math.sin(angle.value) * radius,
                               0)
    flame2.emitter.position = (math.cos(angle.value + math.pi) * radius,
                               math.sin(angle.value + math.pi) * radius,
                               0)

viewport.onSetupFrame.observe(setupFrame)
loop.run()
