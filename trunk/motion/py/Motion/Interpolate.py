"""Interpolation utilities.

Functions:
    - spline    interpolate a trajectory
"""

# Copyright (C) 2005 W. Evan Sheehan
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License
# as published by the Free Software Foundation; either version 2
# of the License, or (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

from Graph import algorithms_c, Data
from Graph.Algorithms import Algorithm, Heuristic
from Graph.Data import AdjacencyList, VertexMap
from Graph.MotionGraph import fix360, fixnegative
from LinearAlgebra import inverse
from Motion import AMC
import Numeric

__all__ = ["spline", "GraphSearch"]

def spline(data, quality):
    """Interpolate a trajectory using natural cubic splines.

    Arguments:
        - data      a list or AMC object containing the data to be interpolated
        - quality   the number of new points to insert between data points
    """
    # Special case: The input data is an AMC object. For each bone in the AMC
    # object create a spline. Return an AMC object.
    if data.__class__ == AMC:
        interpolated = AMC()
        for bone, motion in data.bones.iteritems():
            interpolated.bones[bone] = spline(motion, quality)

        return interpolated

    data = Numeric.array(data)
    quality += 1
    # Assumming a 2-dimensional array.
    # FIXME - Needs error checking?
    length, dof = Numeric.shape(data)
    interpolated = Numeric.empty((length * quality, dof))

    # Range of times we'll be using for the vast majority of the splining process
    times = Numeric.arange(2, 3, 1. / quality)[:-1]

    # For calculating interpolated data points
    f = lambda c: lambda t: c[0] + c[1] * t + c[2] * t**2 + c[3] * t**3

    # Interpolate the data using chunks of the trajectory. Each chunk consists
    # of 4 points. Except for the first and last chunk, interpolate only the
    # inner 2 points of each chunk.
    for frame in range(length - 3):
        # Generate matrices and solve for the constants
        A, b = _getMatrix(data[frame:frame + 4], dof)
        Ainv = inverse(A)
        z = [Numeric.matrixmultiply(Ainv, x) for x in b]

        # Handle each degree of freedom individually
        for degree in range(dof):
            # At the beginning of the trajectory interpolate the first 2 points
            if frame == 0:
                smoothedFrame = frame * quality
                interpolated[smoothedFrame:smoothedFrame + quality, degree] = \
                        map(f(z[degree][:4]), Numeric.arange(1, 2, 1. / quality)[:-1])
            # At the end of the trajectory interpolate the last 2 points
            elif frame == length - 4:
                smoothedFrame = (frame + 2) * quality
                interpolated[smoothedFrame:smoothedFrame + quality, degree] = \
                        map(f(z[degree][-4:]), Numeric.arange(3, 4, 1. / quality)[:-1])

            # Interpolate the middle 2 points
            smoothedFrame = (frame + 1) * quality
            interpolated[smoothedFrame:smoothedFrame + quality, degree] = \
                    map(f(z[degree][4:8]), times)

    return interpolated


def _getMatrix(data, dof):
    """Create the matrices A and b for a spline with 4 data points."""    
    # We need to be sure that we've got exactly 4 data points. 
    assert(Numeric.shape(data)[0] == 4)

    A = Numeric.zeros((12, 12))
    b = [Numeric.zeros((12,)) for i in range(dof)]
    row = 0

    for t in range(1,4):
        # Constrain the spline to fit the 4 points in this chunk of the
        # trajectory.
        col = (t - 1) * 4
        A[row, col:col + 4] = [1, t, t**2, t**3]
        for i in range(dof): b[i][row] = data[t - 1, i]
        row += 1

        A[row, col:col + 4] = [1, t + 1, (t + 1)**2, (t + 1)**3]
        for i in range(dof): b[i][row] = data[t, i]
        row += 1

        # Constrain the first and second derivatives at each of the
        # internal points to be equal.
        if t < 3:
            A[row + 4, col:col + 8] = [0, 1, 2 * t, 3 * t**2, 0, -1, -2 * t, -3 * t**2]
            A[row + 5, col:col + 8] = [0, 0, 2, 6 * t, 0, 0, -2, -6 * t]

    # Constrain the second derivative of the end points of the trajectory to
    # be 0.
    A[-2, :4] = [0, 0, 2, 6]
    A[-1, -4:] = [0, 0, 2, 24]

    return (A, b)


class GraphSearch (Algorithm):
    """A class for interpolating by searching a motion graph."""

    def __init__(self, graph, source, goal):
        """Create the GraphSearch object with a graph.
       
        Arguments:
            - ``graph``     The `Graph` used for the search
            - ``source``    The starting node of the search
            - ``goal``      The ending node of the search
        """
        def fixNode (node):
            """Short helper function that fixes negative angles and insures
            that all angles are on the interval [0, 360).
            """
            n = {}
            for bone, pos in node.iteritems ():
                if bone == "root":
                    pos = pos[3:6]
                pos = [Numeric.remainder (d, 360.0) for d in pos]
                pos = tuple (map (fix360, map (fixnegative, pos)))
                n[bone] = pos
            return self.find_node (n)

        Algorithm.__init__ (self, graph)
        self.source = source
        self.goal = goal
        self.adjacency = dict (graph.representations[AdjacencyList].data) 
        self.search = Heuristic (graph, self.f, fixNode(source), fixNode(goal))
        self.path = self.search.path

    def invalidate (self):
        """Invalidate the algorithm.

        Resets the ``path`` and ``cached_costs`` member variables. Also
        invalidates the `Heuristic` search, if it is present.
        """
        Algorithm.invalidate (self)
        self.path = None
        self.cached_costs = {}
        if hasattr (self, "search"):
            self.search.invalidate ()

    def run (self):
        """Execute the graph search."""
        if self.path:
            return self.path

        self.path = self.search.run ()

        if not self.path:
            return None

        # Construct a list of dictionaries to return. Each dictionary is a
        # frame in the animation.
        for i in range(len(self.path)):
            frame = {}
            for bone in self.path[i].keys():
                node = self.path[i][bone]
                center = node.center

                # Linearly interpolate the position of the root at each frame
                if bone == "root":
                    rootstart = list(self.source["root"][0:3])
                    rootend = list(self.goal["root"][0:3])
                    pos = self.linear_interp(rootstart, rootend, i, len(self.path))
                    center = pos + center

                frame[bone] = center

            self.path[i] = frame

        return self.path

    def find_node (self, pos):
        """Finds the node in ``self.graph`` containing the position ``pos``.

        The vertices in motion graphs are often discretized such that a single
        vertex represents a range of positions. This function finds a vertex in
        ``self.graph`` that contains the position ``pos``.

        Arguments:
            - ``pos``       The body position for which to find a graph node

        Returns:
            A `MotionGraphNode` from ``self.graph`` containing ``pos``
        """
        vertex_map = self.graph.representations[Data.VertexMap]
        for vertex in vertex_map:
            for bone, node in vertex.iteritems ():
                if not node.inside (pos[bone]):
                    break
                return vertex

    def linear_interp(self, start, end, pos, length):
        """Linearly interpolate from 'start' to 'end'.
       
        Returns a list.
        """
        result = []
        for i in range(len(start)):
            compstart = start[i]
            compend   = end[i]

            pos = compstart + ((compend - compstart) * (float(pos) / float(length)))
            result.append(pos)
        return result

    def f (self, path, goal):
        """Return a cost from the end of ``path`` to ``goal``.

        Calculate a cost from ``path`` to ``goal``.  The cost is the length of
        ``path`` plus the length of the shortest path (for each bone) from the
        end of ``path`` to ``goal``.  These costs are cached to minimize runs
        of Dijkstra's shortest path algorithm.

        Arguments:
            - ``path``      A list of nodes that is the path whose cost is
              desired
            - ``goal``      The desired goal of the search

        Returns:
            A number representing the cost of ``path`` in relation to ``goal``
        """
        end = path[-1]
        g = len (path)
        h = 0
        done = []
        notDone = []

        for bone in end.iterkeys():
            if bone not in self.adjacency:
                print "    %s not in adjacency" % bone
                continue
            adj = self.adjacency[bone]
            endb = end[bone]
            goalb = goal[bone]

            if bone not in self.cached_costs:
                self.cached_costs[bone] = {}
            if (endb, goalb) in self.cached_costs[bone]:
                h += self.cached_costs[bone][(endb, goalb)]
                if endb == goalb:
                    done.append(self.cached_costs[bone][(endb, goalb)])
                else:
                    notDone.append(self.cached_costs[bone][(endb, goalb)])
                continue

            path = algorithms_c.dijkstraSearch(adj, endb, goalb)
            h += len(path)

            if endb == goalb:
                done.append(len(path))
            else:
                notDone.append(len(path))

            for i in range(1, len(path)):
                x = path[i]
                self.cached_costs[bone][(endb, x)] = i + 1

            for i in range(0, len(path) - 1):
                x = path[i]
                self.cached_costs[bone][(x, goalb)] = i + 2

        # For each pair of joints where one joint is at its goal and the other
        # is not, add the difference from the not finished one to its goal to
        # h.
        for i in done:
            for j in notDone:
                h += (j - i)

        return (g + h)

# vim: ts=4:sw=4:et
