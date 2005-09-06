""" Sequence

Map a sequence to a chaotic attractor and shuffle it.
"""

import Motion

__all__ = ["Sequence"]

class Sequence:
    """ This class represents a dance sequence mapped to an attractor.
        """
    def __init__ (self, solver, data):
        """ Each Sequence object requires an ODE solver (solver) and some
            motion capture data (data).
            """
        self.solver = solver
        self.solver.solve ()
        self.mapping = self._map (solver.results, data)

    def _map (self, traj, data):
        """ Map the frames from the AMC file to the chaotic attractor. """
        result = {}
        step = len (traj) / len (data)
        length = len (len) * step

        for i in range (length):
            result[traj[i * step]] = data[i]

        return result

    def shuffle (self, ics):
        """ Takes a set of initial conditions (ics) and returns a new
            sequence object containing the shuffled dance.
            """
        pass

    def save (self):
        """ Store the sequence to a file. """
        pass

# FIXME Not convinced this is necessary...
class _Frame:
    """ A frame from the AMC file mapped to a point on a chaotic attractor. """
    def __init__ (self, index, data):

# vim:ts=4:sw=4:et:tw=80
