#!/usr/bin/env python
from BZEngine.UI import Viewport, ThreeDRender, ThreeDControl, Drawable
from BZEngine.UI import ParticleSystem, Environment, Tweak, Texture
from BZEngine import Event, Geometry, Noise, Animated
from OpenGL.GL import *
from Numeric import *


class ParticleTest(Drawable.SpriteArray):
    def __init__(self, position=(0,0,0)):
        numParticles = 250
        self.model = ParticleSystem.SpriteFountain(numParticles)
        Drawable.SpriteArray.__init__(self, numParticles, allowPointSprite=False)
        self.model.attachDrawable(self)

        self.texProxy = Texture.ProxyTexture('spark.png', errorStandin='caution.png')
        self.render.textures = (self.texProxy,)

        self.time = Animated.Timekeeper()
        self.render.static = False
        self.render.blended = True
        self.glowing = True

        self.emitter = self.model.add(ParticleSystem.RandomEmitter,
                                      spawnRate           = 100,
                                      speedRange          = (0, 0),
                                      direction           = (0, 0, 1),
                                      directionRandomness = 1.0,
                                      position            = position,
                                      )
        self.lifespan = self.model.add(ParticleSystem.LifespanAffector, 1)
        self.fader = self.model.add(ParticleSystem.FountainFadeAffector)
        self.constAccel = self.model.add(ParticleSystem.ConstantAccelAffector, (0,0,0))

    def getTweakControls(self):
        from BZEngine.UI import Tweak
        return (
            Tweak.Text(self.texProxy, 'targetName'),
            Tweak.Boolean(self, 'glowing'),
            Tweak.Boolean(self.renderer, 'zSort'),
            Tweak.Separator(),
            Tweak.Quantity(self.emitter, 'spawnRate', range=(0,200)),
            Tweak.Quantity(self.emitter, 'speedRange', range=(0,100)),
            Tweak.Quantity(self.emitter, 'direction', range=(-1,1)),
            Tweak.Quantity(self.emitter, 'directionRandomness'),
            Tweak.Quantity(self.emitter, 'position', range=(-10,10)),
            Tweak.Separator(),
            Tweak.Quantity(self.lifespan, 'lifespan', range=(0,10)),
            Tweak.Quantity(self.fader, 'sizeRange', range=(0,10)),
            Tweak.Color(self.fader, 'colorRange'),
            Tweak.Separator(),
            Tweak.Quantity(self.constAccel, 'vector', range=(-100,100)),
            )

    def draw(self, rstate):
        self.model.integrate(self.time.step())
        glDisable(GL_LIGHTING)
        if self.glowing:
            glBlendFunc(GL_SRC_ALPHA, GL_ONE)
        Drawable.SpriteArray.draw(self, rstate)
        glEnable(GL_LIGHTING)
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA)


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

    particleTest = ParticleTest((0,0,3))
    view.scene.add(particleTest)

    Tweak.Window(particleTest.getTweakControls(),
                 Tweak.Separator(),
                 sky.getTweakControls())

    Tweak.run(loop)
