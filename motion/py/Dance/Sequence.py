""" Sequence

Map a sequence to a chaotic attractor and shuffle it.
"""

import Motion

class Sequence:
    """ This class represents a dance sequence mapped to an attractor.
        """
    def __init__ (self, solver, data):
        """ Each Sequence object requires an ODE solver (solver) and some
            motion capture data (data).
            """
        pass

    def _map (self):
        """ Map the frames from the AMC file to the chaotic attractor. """
        pass

    def shuffle (self, ics):
        """ shuffle takes a set of initial conditions (ics) and returns a new
            sequence object containing the shuffled dance.
            """
        pass

    def save (self):
        """ Store the sequence to a file. """
        pass

class _MappedFrame:
    """ A frame from the AMC file mapped to a point on a chaotic attractor. """
    def __init__ (self):
        pass

# vim:ts=4:sw=4:et:tw=80
