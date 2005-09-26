""" Sequence

Map a sequence to a chaotic attractor and shuffle it.
"""

import Numeric, math
from ODE import RK4, Time

__all__ = ["Sequence"]


class Sequence:
    """This class represents a dance sequence mapped to an attractor."""
    def __init__ (self, system, ic, t, data):
        """ Each Sequence object requires an ordinary differential equation
            solver and some motion capture data (data).
            """
        self.mapping = {}
        self.ode = RK4 (system)

        if isinstance (t, Time):
            traj = self.ode (ic, t)
        else:
            traj = self.ode (ic, Time (t[0], t[1], t[2]))

        step = len (traj) / len (data)
        length = len (data) * step

        # Map each frame evenly over the trajectory.
        for i in range (length):
            self.mapping[traj[i * step]] = _Frame (i, data)

    def __getitem__ (self, frame):
        """ Use [] on a Sequence object to get a frame. """
        return self.mapping.items () [frame]

    def _distance (self, x, y):
        """ Calculate the Euclidean distance between the points x and y. """
        return math.sqrt (Numeric.sum ((y - x)**2))

    def _findNearest (self, point):
        traj = self.mapping.keys ()
        frames = self.mapping.items ()
        dist = self._distance (traj[0], point)
        winner = frames[0]

        for i in range (1, len (traj)):
            delta = self._distance (traj[i], point)
            if delta < dist:
                dist = delta
                winner = frames[i]

        return winner

    def shuffle (self, ics):
        """ Takes a set of initial conditions (ics) and returns a new
            dance sequence.
            """
        shuffled = []
        traj = self.ode (ics)
        step = len (traj) / len (self.mapping.keys ())
        length = len (traj) * step

        for i in range (length):
            shuffled.append (self._findNearest (traj[i * step]))

        return shuffled

    def save (self):
        """ Store the sequence to a file. """
        # FIXME
        pass


class _Frame:
    """ A frame from the AMC file. """
    def __init__ (self, index, data):
        self.__bones = {}

        for bone, values in data.iteritems ():
            self.__bones[bone] = values[index]

    def __getitem__ (self, bone):
        return self.__bones[bone]


# vim:ts=4:sw=4:et:tw=80
