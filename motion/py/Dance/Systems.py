"""A collection of chaotic systems.

These classes represent chaotic systems.  They are intended for use with the
ODE classes in ODE.py.

Classes:
    - System        Interface for all systems
    - Lorenz        The Lorenz system
    - Rossler       The Rossler system
"""

import Numeric


class System:
    """Interface implemented by all systems.

    Methods:
        - __call__      Abstract function for executing the system
    """

    def __call__(self, args, time, step):
        """Calculate a value in the system."""
        pass


class Lorenz(System):
    """The Lorenz system.

    Members:
        - a, r, b       The parameters of the system

    Overridden Methods:
        - __call__      Calculate a value in the system
    """

    def __init__(self, a, r, b):
        """Create a Lorenz object with parameters a, r, and b."""
        self.a = a
        self.r = r
        self.b = b

    def __call__(self, args, time, step):
        """Calculate a value in the system.

        Arguments:
            - args      A List or Tuple with 3 values
            - time      IGNORED
            - step      IGNORED

        Returns:
            A Numeric array that is the solution given the 3 values in args.
        """
        x, y, z = args

        return Numeric.array([self.a*(y - x),
                               self.r*x - y - x*z,
                               x*y - self.b*z])


class Rossler(System):
    """The Rossler system.

    Members:
        - a, b, c       Parameters of the system
        - deltaMatrix

    Overridden Methods:
        - __call__      Calculate a value in the system
    """

    def __init__(self, a, b, c, delta=False):
        """Create a Rossler object.

        Arguments:
            - a, b, c       Parameters of the system
            - delta
        """
        self.a = a
        self.b = b
        self.c = c
        self.deltaMatrix = delta

    def __call__(self, args, time, step):
        """Calculate a value in the system.

        Arguments:
            - args      A List or Tuple of 3 values
            - time      IGNORED
            - step      IGNORED

        Returns:
            A Numeric array that is the solution given the values in args.
        """
        x, y, z = args

        if self.deltaMatrix:
            matrix = [
             [self.a*(y-x), self.r*x-y-x*z, x*y-self.b*z],
             [-self.a*xx+self.a*xy, -self.a*yx+self.a*yy,
                -self.a*zx + self.a*zy],
             [xx*(self.r-z)-xy-x*xz, yx*(self.r-z)-yy-x*yz,
                zx*(self.r-z)-zy-x*zz],
             [y*xx+x*xy-self.b*xz, y*yx+x*yy-self.b*yz, y*zx+x*zy-self.b*zz]]
        else:
             matrix = [-(y + z),
                        x + self.a*y,
                        self.b + z*(x - self.c)]

        return Numeric.array(matrix)


# vim:ts=4:sw=4:et
