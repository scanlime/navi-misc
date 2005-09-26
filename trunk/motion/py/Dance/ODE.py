""" ODE

ODE solvers and related classes.
"""

import Numeric, math

class Time:
    """ A time object for use with ODE solvers. """
    def __init__ (self, initTime, iterations, stepsize):
        """ Start with a start time, time step and number of iterations. """
        self.initial = initTime
        self.time = initTime
        self.n = iterations
        self.h = stepsize
        self.iteration = -1

    def __iter__ (self):
        """ Make this object iterable. """
        return self

    def next (self):
        self.iteration += 1

        if self.iteration >= self.n: raise StopIteration

        self.time += self.h
        return self


class ODE:
    """ Genereic ODE solver object. Just in case we end up using more than
        just RK4 (doubtful). All ODEs need a function, initial conditions,
        and some kind of time constraints.
        """
    def __init__ (self, func):
        self.f = func

    def __call__ (self):
        pass


class RK4 (ODE):
    """ RK4 object. """
    def __init__ (self, func):
        ODE.__init__ (self, func)

    def _step (self, last, i):
        """ Computations performed at each iteration of the ODE. """
        k1 = self.f (last, i.time, i.h)
        k2 = self.f (last + i.h/2. * k1, i.time + i.h/2., i.h)
        k3 = self.f (last + i.h/2. * k2, i.time + i.h/2., i.h)
        k4 = self.f (last + i.h * k3, i.time + i.h, i.h)

        return last + i.h/6.*(k1 + 2.*k2 + 2.*k3 + k4)

    def __call__ (self, ic, time):
        """ Solve the system. """
        result = [Numeric.array (ic)]

        for i in time:
            step = self._step (result[i.iteration], i)
            result.append (step)

        return result


class ARK4 (RK4):
    """ An adaptive RK4 object. """
    def __init__ (self, func, eps=.001):
        RK4.__init__ (self, func)
        # Error tolerance.
        self.epsilon = eps

    def _step (self, last, i):
        # Take one step.
        single = RK4._step (self, last, i)

        # Take two half steps.
        checker = RK4 (self.f, last, i.time, 2, .5*i.h)
        checker()

        # Find the difference.
        diff = checker.results[2] - single

        # If the difference is too large, decrease the time step.
        delta = math.sqrt (Numeric.dot (diff, diff))
        if delta > self.epsilon:
            i.h *= .5
            return self._step (last, i)
        # If the difference is less then our tolerance try doubling h.
        elif delta < self.epsilon:
            checker = RK4 (self.f, last, i.time, 2, 2*i.h)
            checker()

            diff = checker.results[2] - single
            delta = math.sqrt (Numeric.dot (diff, diff))
            # Is it ok to double h?
            if delta < self.epsilon:
                i.h *= 2
                return checker.results[2]

        return single


# vim:ts=4:sw=4:et
