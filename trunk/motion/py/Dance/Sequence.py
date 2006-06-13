"""Map a sequence to a chaotic attractor and shuffle it.

Provides classes for mapping a dance sequence to a chaotic trajectory and
shuffling it.  The dance sequence is contained in an `AMC` object.

:Authors:
    Evan Sheehan <Wallace.Sheehan@gmail.com>
"""

import Motion, Numeric, math
from ODE import RK4

class Sequence:
    """Represent a dance sequence mapped to a trajectory.

    Each frame in a dance sequence is represented as a dictionary mapping a bone
    name to a position (stored in an array). The mapped sequence is a dictionary
    mapping a point in a trajectory (as a tuple) to a frame dictionary. Each
    `Sequence` object represents a single mapping of a dance onto a trajectory.
    `Sequence`\s can be shuffled multiple times without affecting the `Sequence`
    object.

    Members:
        - ``ode``               The ODE solver
        - ``mapping``           The mapping of a coordinate to a frame in the dance
    """

    __slots__ = ["ode", "mapping", "original", "n", "h"]

    def __init__(self, data, system, ic, n=10000, h=.001):
        """Create a new sequence from a chaotic system and a dance sequence.

        Arguments:
            - ``data``      The motion data of the sequence
            - ``system``    The chaotic system to map the sequence to
            - ``ic``        The initial conditions for the chaotic system
            - ``n``         The number of iterations for the trajectory(default 10000)
            - ``h``         The step size for the trajectory(default .001)
        """
        # If we got passed an AMC object, create a list from it. It's unlikely
        # that anything other than an AMC object will be useful here, but for
        # the purpose of testing, allow this class to work with any list of
        # data.
        if isinstance(data, Motion.AMC): data = self._dataFromAMC(data)

        self.ode = RK4(system)
        self.n = n
        self.h = h

        traj = self.ode(ic, n, h)

        frames = len(data)
        step = len(traj) / frames
        length = frames * step

        # Map each frame evenly over the trajectory.
        self.mapping = {}
        for i in range(frames):
            self.mapping[tuple(traj[i * step])] = data[i]

        self.original = data

    def shuffle(self, ic):
        """Create a shuffled sequence from this sequence.

        Generate a new trajectory from the chaotic system used in the initial
        mapping to shuffle the sequence.  Each time the new trajectory passes
        near a point in the original that is mapped to a frame, append that frame
        to the new sequence.  If the initial conditions given here are the same
        as those given when the Sequence object was created, the 'shuffled'
        sequence should be identical to the original.

        Arguments:
            - ``ic``    The initial conditions of the new chaotic trajectory

        Returns:
            A list of frames (represented as dictionaries)
        """
        shuffled = []
        frames = len(self.mapping.keys())
        traj = self.ode(ic, self.n, self.h)
        step = len(traj) / frames

        for i in range(frames):
            shuffled.append(self._findNearest(traj[i*step]))

        return shuffled

    def markChunks(self, sequence):
        """Return a list of chunk boundaries in 'sequence'.
        
        Go over the shuffled sequence looking for chunk boundaries that
        require interpolation. The boundaries list contains indices that mark
        the beginning of a new chunk in the shuffled sequence. So if
        self.boundaries = [x, y], shuffled[x] and shuffled[y] are both the
        beginning of new chunks. This means that interpolation is needed
        between shuffled[x-1] and shuffled[x], and shuffled[y-1] and
        shuffled[y].

        Arguments:
            - ``sequence``      A list of frames to be compared to the
              `self.sequence`

        Returns:
            A list of indices indicating chunk boundaries in ``sequence``
        """
        boundaries = []
        i = self.original.index(sequence[0])

        for j in range(len(sequence)):
            # If we've run off the end of the original sequence, or the frame
            # in shuffled doesn't match the frame in original, we've got the
            # first index of a new chunk.
           if i >= len(self.original) or sequence[j] != self.original[i]:
               boundaries.append(j)
               # Add one to i because j will be incremented the next time
               # around. At this point we know that shuffled[j] == original[i]
               # because we set i to a value that makes that statement true.
               i = self.original.index(sequence[j]) + 1
           else:
               i += 1

        return boundaries

    def insert(self, data, index):
        """Insert a frame into the shuffled sequence at index.
       
        Arguments:
            - ``data``      A dictionary to be inserted into the shuffled sequence
            - ``index``     The index at which the frame will be inserted
        """
        # FIXME - should probably do some error checking to ensure that the
        # Frame or dictionary passed in is compatible with the existing Frames
        # in the sequence.
        assert(isinstance(data, dict))
        self.shuffled.insert(index, data)

    def _dataFromAMC(self, amc):
        """Create a list of Frames from an `AMC` object.
        
        Arguments:
            - ``amc``       An `AMC` object

        Returns:
            A list of dictionaries representing the dance sequence
        """
        def buildFrame(data, index):
            frame = {}
            for bone, frames, in data.iteritems():
                frame[bone] = frames[index]

            return frame

        data = []
        n = len(amc.bones.values()[0])

        for i in range(n):
            data.append(buildFrame(amc.bones, i))

        return data

    def _findNearest(self, point):
        """Find a point in the trajectory nearest to the coordinate given.

        When shuffling a dance sequence, we need to discover which point in the
        original trajectory used in the mapping is closest to each point in the
        new trajectory.
        
        Arguments:
            - ``point``     The point for whom a "nearest neighbor" is desired

        Returns:
            The point in the original trajectory nearest to ``point``
        """
        def distance(x, y):
            return math.sqrt(Numeric.sum((y - x)**2))

        # Initialize the distance to infinity
        dist = 1e300000
        winner = None

        for coord, frame in self.mapping.iteritems():
            delta = distance(coord, point)
            if delta < dist:
                dist = delta
                winner = frame

        return winner


# vim:ts=4:sw=4:et:tw=80
