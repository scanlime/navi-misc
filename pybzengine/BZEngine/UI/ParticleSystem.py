""" BZFlag.UI.ParticleSystem

Simulation models and affectors based on arrays of points. This includes
systems that can be simulated similarly, but aren't traditionally considered
particle systems, like spring and mass simulations.
"""
#
# Python BZFlag Package
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

from __future__ import division
from Numeric import *
from BZFlag.Geometry import *
from BZFlag.World import Scale
from BZFlag import Noise
import random


class Model(object):
    """An abstract particle simulation model. This defines arrays with the
       model's current state, and manages calling affectors to update the
       simulation state.
       """
    def __init__(self, initialState):
        # Initialize arrays for the simulation state
        self.initialState = array(initialState, Float32, savespace=True)
        self.state = array(initialState, Float32, savespace=True)

        # Don't add any affectors by default- let subclasses do that
        self.affectors = []

    def reset(self):
        self.state[...] = self.initialState

    def integrate(self, dt):
        """The default integration function just broadcasts the integration
           request to each affector in sequence. This catches OverflowErrors
           and sends them to integrationError().
           """
        try:
            for affector in self.affectors:
                affector.integrate(dt)
        except OverflowError:
            self.integrationError()

    def integrationError(self):
        """If our simulation got unstable and exploded. Best we can do now is reset it.
           This shouldn't ever happen, but this keeps us from crashing if something goes
           terminally wrong with the simulation.
           """
        self.reset()

    def add(self, cls, *args, **kw):
        """Add an affector to the cloth with the given class and extra
           arguments. Returns the added affector instance.
           """
        inst = cls(self, *args, **kw)
        self.affectors.append(inst)
        return inst

    def attachState(self, array):
        """Copy the model's state into the given array and perform the simulation
           directly in the array in the future. This is good to save from copying
           the simulation state when you want to send the simulation's output to a
           vertex buffer of some sort.
           """
        array[...] = self.state
        self.state = array

    def attachDrawable(self, drawable):
        """Attach this model to a drawable that supports the 'vertices' attribute.
           This may be extended in subclasses to attach other properties of the model,
           such as colors or particle sizes.
           """
        self.attachState(drawable.vertices)


class Affector(object):
    """Abstract base class for an object that affects a simulation model"""
    def __init__(self, model):
        self.model = model

    def integrate(self, dt):
        """Affect the model for one time step"""
        pass


class FixedIntegrationModel(Model):
    """A Model subclass which takes fixed size integration steps. This is used for models
       that can easily become unstable using variable integration steps.

       stepSize      - Size of integration steps taken
       stepTime      - Wallclock time each integration step corresponds to
       """
    def __init__(self, initialState, stepSize, stepTime):
        self.stepSize = stepSize
        self.stepTime = stepTime
        Model.__init__(self, initialState)

        # Time accumulator for keeping track of the temporal error
        # after taking zero or more integration steps in integrate()
        self.dt = 0

    def integrate(self, dt):
        """Convert the caller's timesteps into zero or more fixed-size
           time steps. Returns the number of integration steps performed.
           """
        self.dt += dt
        nSteps = 0
        while self.dt > 0:
            Model.integrate(self, self.stepSize)
            self.dt -= self.stepTime
            nSteps += 1
        return nSteps


class Cloth(FixedIntegrationModel):
    """A cloth is a 2D spring system simulation. Its state is
       represented as a 3-axis Numeric array, a 2D array of
       positions for each point mass in the simulation. This can
       be used directly as a surface array for rendering the cloth.
       All cloths simulated by this class are rectangular.

       This class primarily exists to attach semantics to the
       cloth array. All actual simulation is performed by affectors
       attached to the system. By default the system has an affector
       which exerts spring forces. Extra affectors can be added for
       gravity, wind, and collisions.

       friction      - Damping for each point mass
       stiffness     - Relative amount of force exerted by springs
       stepSize      - Size of integration steps taken
       stepTime      - Wallclock time each integration step corresponds to
       """
    def __init__(self, initialState,
                 friction      = 0.02,
                 stiffness     = 200,
                 stepSize      = 0.04,
                 stepTime      = 0.01,
                 ):
        FixedIntegrationModel.__init__(self, initialState, stepSize, stepTime)
        self.friction = friction
        self.stiffness = stiffness

        self.velocity = zeros(initialState.shape, Float32, savespace=True)

        # Initial affectors to cover the spring forces,
        # friction, and velocity integration
        self.add(ClothSpringAffector)
        self.add(FrictionAffector)
        self.add(VelocityAffector)

    def reset(self):
        FixedIntegrationModel.reset(self)
        self.velocity[...] = 0


class Newtonian(Model):
    """A simple model for particles with position and velocity"""
    def __init__(self, initialState):
        Model.__init__(self, initialState)
        self.velocity = zeros(initialState.shape, Float32, savespace=True)
        self.add(VelocityAffector)

    def reset(self):
        Model.reset(self)
        self.velocity[...] = 0


class Fountain(Newtonian):
    """A newtonian particle model in which each particle has a finite lifetime,
       and all new particles are generated by an emitter. Emitter is a subclass
       of Affector, and can be added with Fountain.add().
       """
    def __init__(self, numParticles):
        # All particles start off dead
        Newtonian.__init__(self, zeros((numParticles, 3)))
        self.life = zeros(numParticles, Float32, savespace=True)

    def reset(self):
        Newtonian.reset(self)
        self.life[...] = 0


class LifespanAffector(Affector):
    """An affector causing particles to age. The aging rate is set so that
       barring any other influence on its life value, a particle will last
       for 'lifespan' seconds.
       """
    def __init__(self, model, lifespan):
        Affector.__init__(self, model)
        self.lifespan = lifespan

    def getLifespan(self):
        return self._lifespan

    def setLifespan(self, lifespan):
        self._lifespan = lifespan
        self.ageRate = -1/lifespan

    lifespan = property(getLifespan, setLifespan)

    def integrate(self, dt):
        add(self.model.life, dt * self.ageRate, self.model.life)


class SpriteFountain(Fountain):
    """A Fountain, extended to support sprite color and size for each particle"""
    def __init__(self, numParticles):
        Fountain.__init__(self, numParticles)
        self.colors = zeros((numParticles, 4), Float32, savespace=True)
        self.sizes = zeros(numParticles, Float32, savespace=True)

    def attachDrawable(self, drawable):
        """This is designed with a SpriteArray drawable in mind"""
        Fountain.attachDrawable(self, drawable)
        drawable.colors[...] = self.colors
        self.colors = drawable.colors
        drawable.sizes[...] = self.sizes
        self.sizes = drawable.sizes


class Emitter(Affector):
    """An affector that scans the model for dead particles and respawns them.
       spawnRate is the maximum particle spawning rate in particles per second.
       """
    def __init__(self, model, spawnRate, position=(0,0,0), velocity=(0,0,0), maxSpawnBudget=5):
        Affector.__init__(self, model)
        self.spawnRate = spawnRate
        self.position = position
        self.velocity = velocity
        self.maxSpawnBudget = maxSpawnBudget
        self.spawnBudget = 0

    def integrate(self, dt):
        """Find all particles with life <= 0. Spawn them, limiting according to our spawn rate"""
        # Add this time step's allowance to our spawn budget
        self.spawnBudget += self.spawnRate * dt

        # Find all the particles with no life left in them
        deadIndices = nonzero(less_equal(self.model.life, 0))

        # Spawn all the particles we're allowed to this time step...
        spawnIndices = deadIndices[:int(self.spawnBudget)]
        if spawnIndices:
            # put() requires flattened indexes, so expand our index list for 3-vectors
            v3indices = (reshape(repeat(spawnIndices, 3), (-1,3)) * 3 + (0,1,2)).flat
            self.spawnBudget -= len(spawnIndices)
            self.respawn(spawnIndices, v3indices)

        # Put an upper limit on our leftover spawnBudget, so we don't get a backlog
        # of particles to emit if our spawnRate was set too high.
        if self.spawnBudget > self.maxSpawnBudget:
            self.spawnBudget = self.maxSpawnBudget

        # ...and store a list of particles that should be hidden.
        #    this will be used by other affectors, probably whichever one is
        #    in charge of the particle's size.
        self.model.hiddenIndices = deadIndices[int(self.spawnBudget):]

    def respawn(self, indices, v3indices):
        """Respawn particles at each model array index in the given list. By default this initializes
           position, velocity, and life according to the emitter. More particle
           qualities for this to set can be added by subclasses.
           indices is a list of particle indices, v3indices is a list of all elements in
           3-vectors corresponding to indices. This is helpful since put() operates on the
           indices of the flattened array.
           """
        numIndices = len(indices)
        put(self.model.state, v3indices, self.newState(numIndices))
        put(self.model.velocity, v3indices, self.newVelocity(numIndices))
        put(self.model.life, indices, self.newLife(numIndices))

    def newState(self, n):
        """Returns state vectors for n new particles. This may return less than n values, and they
           will be repeated as necessary. In particular, it can return a scalar. The other newFoo
           functions here obey the same semantics.
           """
        return self.position

    def newVelocity(self, n):
        return self.velocity

    def newLife(self, n):
        """By default particle lifetimes range from 1 to 0"""
        return 1


class RandomEmitter(Emitter):
    """An emitter that randomly varies the direction and speed of emitted particles.
       speedRange should be a tuple with the minimum and maximum speed.
       directionRandoness should be between 0 and 1. 0 gives exactly the given direction,
       1 gives a completely random direction.
       """
    def __init__(self, model, spawnRate, speedRange, direction, directionRandomness=0, position=(0,0,0)):
        Emitter.__init__(self, model, spawnRate, position)
        self.speedRange = speedRange
        self.position = position

        # We have to initially set the direction and directionRandomness like this,
        # since both are properties that won't properly update without the other.
        self._direction = direction
        self.directionRandomness = directionRandomness

    def updatePremultDirection(self):
        """We store a version of our current direction and directionRandomness that's premultiplied.
           Through the magic of properties, this is automatically updated whenever direction or
           directionRandomness are set normally.
           """
        self.premultDirection = (normalize(self._direction) * (1-self._directionRandomness)).astype(Float32)

    def getDirection(self):
        return tuple(self._direction)

    def setDirection(self, direction):
        self._direction = direction
        self.updatePremultDirection()

    direction = property(getDirection, setDirection)

    def getDirectionRandomness(self):
        return self._directionRandomness[0]

    def setDirectionRandomness(self, directionRandomness):
        self._directionRandomness = array((directionRandomness,), Float32)
        self.updatePremultDirection()

    directionRandomness = property(getDirectionRandomness, setDirectionRandomness)

    def newVelocity(self, n):
        """Generate a new random velocity by blending the given direction with a random unit vector,
           normalizing it, then multiplying by a randomly generated speed.
           """
        randomDirections = Noise.randomVectors((n, 3), type=Float32)
        randomDirections *= self._directionRandomness
        randomDirections += self.premultDirection
        normalize(randomDirections, randomDirections)
        randomSpeeds = repeat(Noise.randomArray((n, 1), type=Float32, range=self.speedRange), 3, -1)
        randomDirections *= randomSpeeds
        return randomDirections


class FountainFadeAffector(Affector):
    """Causes particles in a Fountain to shrink and/or change color over the course of their life.

       Functions can be supplied to map the particle's lifetimes to interpolants. The 'lifeFunction'
       is evaluated first, if present, then its output is passed to 'sizeFunction' and 'colorFunction'.
       All functions default to an identity.
       The functions will be processing all life values at once, so they should be written to accept
       and return Numeric arrays.
       """
    def __init__(self, model,
                 sizeRange     = (0,1),
                 colorRange    = ((0,0,0,0), (1,1,1,1)),
                 lifeFunction  = None,
                 sizeFunction  = None,
                 colorFunction = None,
                 ):
        Affector.__init__(self, model)
        self.sizeRange = sizeRange
        self.colorRange = colorRange
        self.sizeFunction = sizeFunction
        self.colorFunction = colorFunction
        self.lifeFunction = lifeFunction

    def getSizeRange(self):
        return (self.sizeConst, self.sizeConst + self.sizeCoeff)

    def setSizeRange(self, sizeRange):
        self.sizeConst = sizeRange[0]
        self.sizeCoeff = sizeRange[1] - sizeRange[0]

    sizeRange = property(getSizeRange, setSizeRange)

    def getColorRange(self):
        return (self.colorConst, self.colorConst + self.colorCoeff)

    def setColorRange(self, colorRange):
        colorRange = asarray(colorRange)
        self.colorConst = colorRange[0]
        self.colorCoeff = colorRange[1] - colorRange[0]

    colorRange = property(getColorRange, setColorRange)

    def integrate(self, dt):
        if self.lifeFunction:
            life = self.lifeFunction(self.model.life)
        else:
            life = self.model.life

        if self.sizeFunction:
            sizeLife = self.sizeFunction(life)
        else:
            sizeLife = life

        if self.colorFunction:
            colorLife = self.colorFunction(life)
        else:
            colorLife = life

        self.model.sizes[...] = sizeLife * self.sizeCoeff + self.sizeConst
        put(self.model.sizes, self.model.hiddenIndices, 0)

        self.model.colors[...,0] = colorLife * self.colorCoeff[...,0] + self.colorConst[...,0]
        self.model.colors[...,1] = colorLife * self.colorCoeff[...,1] + self.colorConst[...,1]
        self.model.colors[...,2] = colorLife * self.colorCoeff[...,2] + self.colorConst[...,2]
        self.model.colors[...,3] = colorLife * self.colorCoeff[...,3] + self.colorConst[...,3]


class ClothSpringAffector(Affector):
    """Affector that exerts spring forces on a cloth model"""
    def integrateSprings(self, dt, span):
        """Integrate springs with the given x and y span in grid cells"""
        # Select equally shaped slices centered on each spring from the arrays
        # we're interested in. Zeroes in span[] have to be special cases here
        # since -0 does not select an entire axis.
        # This lets us process all equal-length springs at once.
        if span[0] and span[1]:
            pos_s1 = self.model.state[:-span[0], :-span[1]]
            pos_s2 = self.model.state[span[0]:, span[1]:]
            vel_s1 = self.model.velocity[:-span[0], :-span[1]]
            vel_s2 = self.model.velocity[span[0]:, span[1]:]
            init_s1 = self.model.initialState[:-span[0], :-span[1]]
            init_s2 = self.model.initialState[span[0]:, span[1]:]
        elif span[0]:
            pos_s1 = self.model.state[:-span[0], :]
            pos_s2 = self.model.state[span[0]:, :]
            vel_s1 = self.model.velocity[:-span[0], :]
            vel_s2 = self.model.velocity[span[0]:, :]
            init_s1 = self.model.initialState[:-span[0], :]
            init_s2 = self.model.initialState[span[0]:, :]
        else:
            pos_s1 = self.model.state[:, :-span[1]]
            pos_s2 = self.model.state[:, span[1]:]
            vel_s1 = self.model.velocity[:, :-span[1]]
            vel_s2 = self.model.velocity[:, span[1]:]
            init_s1 = self.model.initialState[:, :-span[1]]
            init_s2 = self.model.initialState[:, span[1]:]

        posVec = pos_s2 - pos_s1
        currentLength = magnitude(posVec)
        restLength = magnitude(init_s2 - init_s1)
        direction = normalize(posVec)
        extension = currentLength - restLength

        # This creates a 2D array of vectors, each with X, Y, and Z
        # equal to the extension from the corresponding location
        extensionVector = reshape(repeat(extension, 3, 1), direction.shape)

        # Apply a force to each mass connected to this spring,
        # proportional to the spring extension in the spring's direction
        force = dt * direction * self.model.stiffness * extensionVector
        vel_s1 += force
        vel_s2 -= force

    def integrate(self, dt):
        # The number of springs and their orientation will determine the
        # cloth's stiffness and stability. More springs take longer to
        # simulate, of course.
        self.integrateSprings(dt, (1,0))
        self.integrateSprings(dt, (0,1))
        self.integrateSprings(dt, (1,1))
        self.integrateSprings(dt, (2,0))
        self.integrateSprings(dt, (2,2))
        self.integrateSprings(dt, (0,2))


class VelocityAffector(Affector):
    """Generic affector for applying velocity to particle positions"""
    def integrate(self, dt):
        add(self.model.state, multiply(self.model.velocity, dt), self.model.state)


class FrictionAffector(Affector):
    """Generic affector for applying friction to particle velocities"""
    def integrate(self, dt):
        multiply(self.model.velocity, 1 - self.model.friction, self.model.velocity)


class ConstantAccelAffector(Affector):
    """Generic affector for applying a constant acceleration to particles"""
    def __init__(self, model, vector):
        Affector.__init__(self, model)
        self.setVector(vector)

    def getVector(self):
        return self._vector

    def setVector(self, vector):
        self._vector = asarray(vector)

    vector = property(getVector, setVector)

    def integrate(self, dt):
        add(self.model.velocity, self._vector * dt, self.model.velocity)


class ClothWindAffector(Affector):
    """Affector for applying a wind force to cloth. This must be given a
       reference to the ArraySurface that renders this cloth, since it uses
       the cross products and normals that it has already calculated.
       """
    def __init__(self, model, surface, wind):
        Affector.__init__(self, model)
        self.crossProducts = surface.crossProducts
        self.gridNormals = surface.gridNormals
        self.wind = wind

    def integrate(self, dt):
        """Uses the dot product to calculate a normal force due to wind,
           applied to all particles in the spring system.
           """
        forceMagnitude = dot(self.gridNormals, tuple(self.wind))
        force = self.gridNormals * reshape(repeat(forceMagnitude, 3, 1), self.gridNormals.shape)
        self.model.velocity[:-1,:-1] += force


class ClothAnchorAffector(Affector):
    """Affector that forces the given array positions to remain at their initial values"""
    def __init__(self, model, *positions):
        Affector.__init__(self, model)
        self.positions = positions

    def integrate(self, dt):
        for x,y in self.positions:
            self.model.state[x,y] = self.model.initialState[x,y]

### The End ###
