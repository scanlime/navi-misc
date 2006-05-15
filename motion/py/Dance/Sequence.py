"""Map a sequence to a chaotic attractor and shuffle it.

Provides classes for mapping a dance sequence to a chaotic trajectory and
shuffling it.  The dance sequence is contained in an AMC object.

Classes:
    - Sequence      Maps a sequence to a trajectory
    - Frame         Represents a single frame in the sequence
"""

import Motion, Numeric, math
from ODE import RK4

__all__ = ["Sequence", "Frame"]


class Sequence:
    """Represent a dance sequence mapped to a trajectory.

    Members:
        - ode               The ODE solver
        - mapping           The mapping of a coordinate to a frame in the dance
        - original          The original dance sequence

    Methods:
        - shuffle           Shuffle the dance sequence
        - insert            Insert a frame into the dance sequence
        - save              Write the sequence to an AMC file
        - __getitem__       Access individual frames using the [] operator
    """

    def __init__ (self, data, system, ic, n=10000, h=.001):
        """Create a new sequence from a chaotic system and a dance sequence.

        Arguments:
            - data      The motion data of the sequence
            - system    The chaotic system to map the sequence to
            - ic        The initial conditions for the chaotic system
            - n         The number of iterations for the trajectory (default 10000)
            - h         The step size for the trajectory (default .001)
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

        self.original = data

    def shuffle (self, ic, n=10000, h=.001):
        """Create a shuffled sequence from this sequence.

        Generate a new trajectory from the chaotic system used in the initial
        mapping to shuffle the sequence.  Each time the new trajectory passes
        near a point in the original that is mapped to a frame, add that frame
        to the new sequence.  If the initial conditions given here are the same
        as those given when the Sequence object was created, the 'shuffled'
        sequence should be identical to the original.

        Arguments:
            - ic        The initial conditions of the new chaotic trajectory
            - n         The number of iterations of the trajectory (default 10000)
            - h         The step size of the trajectory (default .001)
        """
        shuffled = []
        frames = len (self.mapping.keys ())
        traj = self.ode (ic, n, h)
        step = len (traj) / frames

        for i in range (frames):
            shuffled.append (self._findNearest (traj[i*step]))

        self.shuffled = shuffled

        # Parse the shuffled sequence looking for chunk boundaries.
        self._markChunks ()

    def insert (self, data, index):
        """Insert a frame into the sequence at index.
       
        Arguments:
            - data      The Frame object to be inserted
            - index     The index at which the frame will be inserted
        """
        # FIXME - should probably do some error checking to ensure that the
        # Frame or dictionary passed in is compatible with the existing Frames
        # in the sequence.
        if isinstance (data, dict): data = Frame (data)
        assert (isinstance (data, Frame))
        self.shuffled.insert (index, data)

    def save (self, filename, format):
        """Write the sequence to a file.
        
        Arguments:
            - filename      The filename to write to
            - format        The format of the AMC file
        """
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
        """Create a list of Frames from an AMC object."""
        data = []
        n = len (amc.bones.values ()[0])

        for i in range (n):
            data.append (Frame (amc.bones, i))

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

    def _markChunks (self):
        """Go over the shuffled sequence looking for chunk boundaries that
           require interpolation. The boundaries list contains indices that mark
           the beginning of a new chunk in the shuffled sequence. So if
           self.boundaries = [x, y], shuffled[x] and shuffled[y] are both the
           beginning of new chunks. This means that interpolation is needed
           between shuffled[x-1] and shuffled[x], and shuffled[y-1] and
           shuffled[y].
           """
        self.boundaries = []
        i = self.original.index (self.shuffled[0])

        for j in range (len (self.shuffled)):
            # If we've run off the end of the original sequence, or the frame
            # in shuffled doesn't match the frame in original, we've got the
            # first index of a new chunk.
           if i >= len (self.original) or self.shuffled[j] != self.original[i]:
               self.boundaries.append (j)
               # Add one to i because j will be incremented the next time
               # around. At this point we know that shuffled[j] == original[i]
               # because we set i to a value that makes that statement true.
               i = self.original.index (self.shuffled[j]) + 1
           else:
               i += 1


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


class Frame:
    """A frame in an AMC file.
    
    AMC objects store all their data in a dictionary mapping bone names to a
    Numeric array.  The Numeric array has 2 dimensions so that the first index
    is the frame number and the second is the degree of freedom of the joint.
    Frame objects separate each individual frame out of these arrays.

    Members:
        - bones             A dictionary mapping bone name to a single position

    Methods:
        - __getitem__       Access a single bone in the frame using the []
                            operator
    """
    def __init__ (self, data, index=None):
        """Create a Frame object from data.

        Arguments:
            - data      A dictionary mapping bone names to Numeric arrays
                        (usually from an AMC object).  It should have the form:
                        data["bone name"][frame, dof]
            - index     The frame in data to represent in this Frame object
        """
        # Map bone names to a list of values in a single frame. Each value
        # corresponds to a degree of freedom for that bone. index gives the
        # frame number and data is a dictionary mapping bones to motion data.
        # The movement data is stored as follows: data["bone name"][frame, dof].
        if index != None:
            self.bones = {}

            for bone, values in data.iteritems ():
                self.bones[bone] = values[index]
        else:
            self.bones = data

    #def __cmp__ (self, other):
    #   """Compare dictionaries when comparing frames."""
    #   return self.bones.__cmp__ (other.bones)

    def __getitem__ (self, bone):
        """Return the position for bone in this frame.

        Arguments:
            - bone      A bone name as a string
        """
        return self.bones[bone]

# vim:ts=4:sw=4:et:tw=80
