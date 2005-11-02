""" Sequence

Map a sequence to a chaotic attractor and shuffle it.
"""

import Motion, Numeric, math
from ODE import RK4

__all__ = ["Sequence"]


class Sequence:
    """This class represents a dance sequence mapped to an attractor."""
    def __init__ (self, data, system, ic, n=10000, h=.001):
        """Each Sequence object requires an ordinary differential equation
           solver and some motion capture data (data).
           """
        # If we got passed an AMC object, create a list from it. It's unlikely
        # that anything other than an AMC object will be useful here, but for
        # the purpose of testing, allow this class to work with any list of
        # data.
        if isinstance (data, Motion.AMC): data = self._dataFromAMC (data)

        self.ode = RK4 (system)

        traj = self.ode (ic, n, h)

        frames = len (data)
        step = len (traj) / frames
        length = frames * step

        # Map each frame evenly over the trajectory.
        self.mapping = {}
        for i in range (frames):
            self.mapping[_Coordinate (traj[i * step])] = data[i]

    def shuffle (self, ic, n=10000, h=.001):
        """Takes a set of initial conditions (ics) and returns a new
           dance sequence.
           """
        shuffled = []
        traj = self.ode (ic, n, h)
        step = len (traj) / len (self.mapping.keys ())

        for i in range (len (traj)):
            shuffled.append (self._findNearest (traj[i]))

        self.shuffled = shuffled

    def save (self, filename, format):
        """Store the sequence to a file."""
        amc = Motion.AMC ()
        amc.format = format

        bones = {}
        for bone in self.shuffled[0].bones.iterkeys ():
            bones[bone] = [frame[bone] for frame in self.shuffled]

        amc.bones = dict ([(bone, Numeric.array (data)) for bone,data in bones.iteritems ()])

        amc.save (filename)

    def __getitem__ (self, frame):
        """Use [] on a Sequence object to get a frame."""
        return self.mapping.items () [frame]

    def _dataFromAMC (self, amc):
        """Create a list of _Frames from an AMC object."""
        data = []
        n = len (amc.bones.values ()[0])

        for i in range (n):
            data.append (_Frame (i, amc.bones))

        return data

    def _distance (self, x, y):
        """Calculate the Euclidean distance between the points x and y."""
        return math.sqrt (Numeric.sum ((y - x)**2))

    def _findNearest (self, point):
        """Find a point in the trajectory nearest to the coordinate given."""
        traj = [Numeric.array (key.coordinates) for key in self.mapping.keys ()]
        frames = self.mapping.values ()
        dist = self._distance (traj[0], point)
        winner = frames[0]

        for i in range (1, len (traj)):
            delta = self._distance (traj[i], point)
            if delta < dist:
                dist = delta
                winner = frames[i]

        return winner


class _Coordinate:
    """A hashable coordinate object."""
    def __init__ (self, coords):
        self.coordinates = Numeric.array (coords)

    def __cmp__ (self, other):
        if math.sqrt (Numeric.sum (self.coordinates**2)) < \
                math.sqrt (Numeric.sum (other.coordinates**2)):
            return -1
        else:
            i = iter (other.coordinates)
            for coord in self.coordinates:
                if coord != i.next():
                    return 1

        return 0

    def __hash__ (self):
        h = 0
        for coord in self.coordinates:
            h = h ^ hash (coord)
        return h


class _Frame:
    """A frame from the AMC file."""
    def __init__ (self, index, data):
        # Map bone names to a list of values in a single frame. Each value
        # corresponds to a degree of freedom for that bone. index gives the
        # frame number and data is a dictionary mapping bones to motion data.
        # The movement data is stored as follows: data["bone name"][frame, dof].
        self.bones = {}

        for bone, values in data.iteritems ():
            self.bones[bone] = values[index]

    def __getitem__ (self, bone):
        return self.bones[bone]


# vim:ts=4:sw=4:et:tw=80
