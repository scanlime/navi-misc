""" ODE

ODE solvers and related classes.
"""

import Numeric, math


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

    def _step (self, last, h, t):
        """ Computations performed at each iteration of the ODE. """
        k1 = self.f (last, t, h)
        k2 = self.f (last + h/2. * k1, t + h/2., h)
        k3 = self.f (last + h/2. * k2, t + h/2., h)
        k4 = self.f (last + h * k3, t + h, h)

        return last + h/6.*(k1 + 2.*k2 + 2.*k3 + k4)

    def __call__ (self, ic, n, h):
        """ Solve the system. """
        result = [Numeric.array (ic)]

        for i in range (n):
            step = self._step (result[i], h, i*h)
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
