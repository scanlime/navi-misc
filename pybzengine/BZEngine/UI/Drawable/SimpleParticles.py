""" BZEngine.UI.Drawable.SimpleParticles

A simple tweakable and pickleable particle system drawable, using
the most common particle affectors.
"""
#
# Python BZEngine Package
# Copyright (C) 2003 Micah Dowty <micahjd@users.sourceforge.net>
#
#  This library is free software; you can redistribute it and/or
#  modify it under the terms of the GNU Lesser General Public
#  License as published by the Free Software Foundation; either
#  version 2.1 of the License, or (at your option) any later version.
#
#  This library is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
#  Lesser General Public License for more details.
#
#  You should have received a copy of the GNU Lesser General Public
#  License along with this library; if not, write to the Free Software
#  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
#
from Sprite import SpriteArray
from BZEngine.UI import ParticleSystem, Texture
from BZEngine import Animated
from OpenGL.GL import *

class SimpleParticles(SpriteArray):
    def __init__(self, position=(0,0,0), numParticles=250):
        print "init"
        self.numParticles = numParticles
        self.model = ParticleSystem.SpriteFountain(numParticles)
        SpriteArray.__init__(self, numParticles, allowPointSprite=False)
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
        """Return a tuple of Tweak controls applicable to this drawable"""
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
        SpriteArray.draw(self, rstate)
        glEnable(GL_LIGHTING)
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA)

    def __getstate__(self):
        """Called during pickling, returns the information to be pickled.
           To avoid storing all the arrays representing this particle system's
           exact state, we return only the exact values we're interested in saving.

           This returns a list of object, attribute, value tuples.
           """
        state = []
        for object, attribute in [
            ('self.texProxy', 'targetName'),
            ('self', 'glowing'),
            ('self.renderer', 'zSort'),
            ('self.emitter', 'spawnRate'),
            ('self.emitter', 'speedRange'),
            ('self.emitter', 'direction'),
            ('self.emitter', 'directionRandomness'),
            ('self.emitter', 'position'),
            ('self.lifespan', 'lifespan'),
            ('self.fader', 'sizeRange'),
            ('self.fader', 'colorRange'),
            ('self.constAccel', 'vector'),
            ]:
            state.append(( object, attribute, getattr(eval(object), attribute) ))
        return state

    def __setstate__(self, state):
        """Called during unpickling, with the object, attribute, value tuples
           saved above.
           """
        self.__init__()
        for (object, attribute, value) in state:
            setattr(eval(object), attribute, value)

### The End ###
