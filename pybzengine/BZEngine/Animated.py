""" BZEngine.Animated

Classes for implementing values that change over time.
This includes vectors and values that move according to
physical models or other formulas.
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

from __future__ import division
import time, math
from Numeric import *


class TimeMaster:
    """Singleton that keeps track of the master time step that all Timekeepers follow.
       This is the default implementation, but it may be overridden in situations
       when the rendering engine is not running in realtime. For example, to record
       to a movie file at a constant rate, or to synchronize with a server's time.
       """
    def __init__(self):
        self.then = None
        self.now = time.time()
        self.stepNumber = 0
        self.stepTime = 0

    def update(self):
        """Take a global time step. This is generally called by the viewport,
           but if you are using it in a nonvisual environment, you will have to
           call it yourself each time you want to take a separate timestep.
           """
        self.now = time.time()
        if self.then:
            self.stepTime = self.now - self.then
        else:
            self.stepTime = 0
        self.then = self.now
        self.stepNumber += 1

    def step(self, lastStep=None):
        """Return a (stepTime, stepNumber) tuple for the current time step.
           Clients to this function should pass that step number back as
           lastStep the next time this function is called.
           """
        if lastStep == self.stepNumber:
            return (0, self.stepNumber)
        else:
            return (self.stepTime, self.stepNumber)

    def time(self):
        """Return the current time, according to the master clock"""
        return self.now

    def jump(self):
        """This should be called after an unusually time consuming operation that
           all running animations should ignore. All time between the last update()
           and this call to jump() will be ignored in the next update().
           """
        self.then = time.time()

defaultTimeMaster = TimeMaster()
currentTimeMaster = defaultTimeMaster


class Timekeeper:
    """A class that keeps track of the amount of time elapsed between steps.
       This is a local abstraction that makes it easy for a particular subsystem
       to manage its integration and be sure of not integrating twice on the same
       timestep. All timekeeper instances advance in sync when the TimeMaster takes
       a step.
       """
    def __init__(self):
        self.lastStep = None

    def time(self):
        """Return the current time, according to the master clock"""
        global currentTimeMaster
        return currentTimeMaster.time()

    def step(self):
        """Take a step, return the number of seconds since the last step."""
        global currentTimeMaster
        (step, self.lastStep) = currentTimeMaster.step(self.lastStep)
        return step


class Value:
    """A value that changes over time according to a function.
       The given function should take the old value, the time step, and the vector index as parameters.
       The vector index will always be None if this value isn't used in a vector.
       This module provides several callable classes that be used in place of an actual function.
       """
    def __init__(self, f, value=0):
        self.value = value
        self.f = f

    def integrate(self, dt, index=None):
        self.value = self.f(self.value, dt, index)

    def __float__(self):
        return float(self.value)


class Vector:
    """A vector of Animated.Values.
       The given function should take the old value, the time step, and the vector index as parameters.
       This module provides several callable classes that be used in place of an actual function.
       The vector contents can be accessed transparently from this class.
       """
    def __init__(self, f, value=(0,0,0)):
        self.f = f
        self.values = [Value(self.f, v) for v in value]

    def integrate(self, dt):
        for i in xrange(len(self.values)):
            self.values[i].integrate(dt, i)

    def __getitem__(self, i):
        return self.values[i].value

    def __setitem__(self, i, v):
        self.values[i].value = v

    def set(self, value):
        for i in xrange(len(value)):
            self.values[i].value = value[i]

    def get(self):
        return [v.value for v in self.values]


class Velocity:
    """A class that can be used as an integration function for Value and Vector.
       The given velocity should match the data type and number of elements.
       The velocity vector contents can be accessed transparently from this class.
       """
    def __init__(self, value):
        self.value = value

    def __call__(self, oldValue, dt, index=None):
        if index is None:
            return oldValue + self.value * dt
        else:
            return oldValue + self.value[index] * dt

    def __getitem__(self, i):
        return self.value[i]

    def __setitem__(self, i, v):
        self.value[i] = v


class Approach:
    """Abstract base class for animation evaluators that approach a target.
       Subclasses of this can be used as an integration function for Value and Vector.
       """
    def __init__(self, target):
        self.target = target

    def __call__(self, value, dt, index=None):
        if index is None:
            t = self.target
        else:
            t = self.target[index]
        return self.f(value, t, dt)

    def f(self, value, target, dt):
        """Approach function. Subclasses must override this."""
        pass


class LogApproach(Approach):
    """Approaches a target, fast at first but exponentially slowing down."""
    def __init__(self, target, speed, epsilon=0.00001):
        Approach.__init__(self, target)
        self.speed = speed
        self.epsilon = epsilon

    def f(self, value, target, dt):
        delta = target - value
        if delta > self.epsilon or delta < -self.epsilon:
            return value + (target - value) * dt * self.speed
        else:
            return target


class LinearApproach(Approach):
    """Approaches a target at a constant speed"""
    def __init__(self, target, speed):
        Approach.__init__(self, target)
        self.speed = speed

    def f(self, value, target, dt):
        if target > value:
            nvalue = value + dt * self.speed
            if nvalue > target:
                # We just stepped over the target- stop now
                return target
            return nvalue
        else:
            nvalue = value - dt * self.speed
            if nvalue < target:
                # We just stepped over the target- stop now
                return target
            return nvalue


def sigmoid(x):
    """The sigmoid function. Approaches zero as x approaches -inf,
       approaches 1 as x approaches +inf, 0.5 at x=0.
       As x increases, y rises slowly at first, then fast,
       then finally slow. This produces a very natural animation.
       """
    return 1 / (1 + math.exp(-x))


def invSigmoid(y):
    """Inverse of the sigmoid function"""
    return -math.log(1/y - 1)


class SigmoidApproach(LinearApproach):
    """A linear approach transformed through the sigmoid function.
       The supplied range will be mapped to a (0.02, 0.98) range
       on the sigmoid function.
       """
    def __init__(self, target, speed, range):
        self.range = range
        LinearApproach.__init__(self, target, speed)

    def map(self, x):
        normalized = (x - self.range[0]) / (self.range[1] - self.range[0])
        if normalized > 1:
            normalized = 1
        if normalized < 0:
            normalized = 0
        return invSigmoid(normalized * 0.96 + 0.02)

    def unmap(self, x):
        return self.range[0] + (sigmoid(x) - 0.02)/0.96 * (self.range[1] - self.range[0])

    def f(self, value, target, dt):
        return self.unmap(LinearApproach.f(self, self.map(value), self.map(target), dt))


class PeriodicFunction:
    """Abstract base class for animation evaluators that evaluate a periodic
       function over time, scaled to the given range.
       """
    def __init__(self, period=1, range=(0,1)):
        self.period = period
        self.range  = range
        self.cycleTime = 0

    def __call__(self, value, dt, index=None):
        self.cycleTime = (self.cycleTime + dt) % self.period
        return self.f(self.cycleTime / self.period) * \
               (self.range[1] - self.range[0]) + self.range[0]

    def f(self, x):
        """Implementation of the periodic function. Should have a domain
           of [0,1] and a range of [0,1], which will be scaled to the values
           our instantiator wants.
           """
        pass


class SineFunction(PeriodicFunction):
    def f(self, x):
        return math.sin(x * math.pi * 2) * 0.5 + 0.5


class RampFunction(PeriodicFunction):
    def f(self, x):
        return x


class PerlinNoise:
    """Animation evaluator for perlin noise with a specified amplitude,
       offset, frequency, number of octaves, and random seed. This produces
       natural-looking noise in up to 3 dimensions.
       """
    def __init__(self, amplitude=1, frequency=1, offset=0, octaves=3, persistence=0.5, seed=None):
        from BZEngine import Noise
        self.amplitude = amplitude
        self.frequency = frequency
        self.offset = offset
        if seed is None:
            self.noise = (Noise.PerlinNoise(octaves=octaves, persistence=persistence),
                          Noise.PerlinNoise(octaves=octaves, persistence=persistence),
                          Noise.PerlinNoise(octaves=octaves, persistence=persistence))
        else:
            self.noise = (Noise.PerlinNoise(seed=seed+1, octaves=octaves, persistence=persistence),
                          Noise.PerlinNoise(seed=seed+2, octaves=octaves, persistence=persistence),
                          Noise.PerlinNoise(seed=seed+3, octaves=octaves, persistence=persistence))
        self.time = 0

    def __call__(self, value, dt, index=None):
        self.time += dt
        if not index:
            index = 0
        return self.noise[index].get(self.time * self.frequency) * self.amplitude + self.offset

### The End ###
