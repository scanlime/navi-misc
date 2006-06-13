"""A collection of Ordinary Differential Equation (ODE) solvers.

This module provides a variety of ODE solvers all based on a generic class so
that they may be used interchangeably.

:Authors:
    Evan Sheehan <Wallace.Sheehan@gmail.com>
"""

import Numeric, math


class ODE:
    """Abstract ODE solver object.

    This class is abstract. It's main purpose is to require all ODE solvers to
    be callable by implementing the `__call__` method.
   
    Members:
        - ``f``     The ODE for the solver to solve
    """

    def __init__(self, func):
        """Create a solver with an ODE to solve.

        Arguments:
            - ``func``      The ODE to be solved
        """
        self.f = func

    def __call__(self):
        """Abstract `__call__` method.

        All `ODE`\s should be callable, so all subclasses of `ODE` should
        override this method. If they don't, an exception is raised when the
        object is called.
        """
        raise Exception ("Not implemented in class %s" % (self.__class__))


class RK4(ODE):
    """Runge-Kutte 4 ODE solver."""

    def _step(self, last, h, t):
        """Execute a single RK4 step."""
        k1 = self.f(last, t, h)
        k2 = self.f(last + h/2. * k1, t + h/2., h)
        k3 = self.f(last + h/2. * k2, t + h/2., h)
        k4 = self.f(last + h * k3, t + h, h)

        return last + h/6.*(k1 + 2.*k2 + 2.*k3 + k4)

    def __call__(self, ic, n, h):
        """Solve the system and return the result.

        Arguments:
            - ``ic``    The initial conditions
            - ``n``     The number of iterations
            - ``h``     The step size
        """
        result = [Numeric.array(ic)]

        for i in range(n):
            step = self._step(result[i], h, i*h)
            result.append(step)

        return result


class ARK4(RK4):
    """An Adaptive Runge-Kutte 4 ODE solver."""

    def __init__(self, func, eps=.001):
        """Create an ARK4 object with an error tolerance.

        Arguments:
            - ``func``      The ODE to be solved
            - ``eps``       The error tolerance used to adjust the step size
        """
        RK4.__init__(self, func)
        # Error tolerance.
        self.epsilon = eps

    def _step(self, last, i):
        """Overridden from RK4."""
        # Take one step.
        single = RK4._step(self, last, i)

        # Take two half steps.
        checker = RK4(self.f, last, i.time, 2, .5*i.h)
        checker()

        # Find the difference.
        diff = checker.results[2] - single

        # If the difference is too large, decrease the time step.
        delta = math.sqrt(Numeric.dot(diff, diff))
        if delta > self.epsilon:
            i.h *= .5
            return self._step(last, i)
        # If the difference is less then our tolerance try doubling h.
        elif delta < self.epsilon:
            checker = RK4(self.f, last, i.time, 2, 2*i.h)
            checker()

            diff = checker.results[2] - single
            delta = math.sqrt(Numeric.dot(diff, diff))
            # Is it ok to double h?
            if delta < self.epsilon:
                i.h *= 2
                return checker.results[2]

        return single


# vim:ts=4:sw=4:et
