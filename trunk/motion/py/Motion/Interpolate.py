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


class GraphSearch:
    """A class for interpolating by searching a motion graph."""

    __slots__ = ["graphs", "adjacency", "order", "epsilon"]

    def __init__(self, graphs, asf, epsilon=0.3**29):
        """Create the GraphSearch object with the graphs."""
        self.graphs = graphs
        self.epsilon = epsilon

        # Build the dictionary of adjacency lists. If a graph doesn't have an
        # AdjacencyList, raise an exception.
        for bone, graph in graphs.iteritems():
            try:
                self.adjacency[bone] = graph.representations[AdjacencyList]
            except KeyError:
                raise Exception("%s graph doesn't have an AdjacencyList representation" % (bone))

        # Initialize the build order for the successor function
        build_order(asf)

    def __call__(self, start, end):
        """Execute the graph search.
        
        Interpolate between the frames 'start' and 'end'. Returns a path from
        'start' to 'end', or None if there isn't one.
        """
        path = None
        source = {}
        goal = {}

        for bone in start.iterkeys():
            # Temporary storage for building graph nodes from frame data
            s = start[bone]
            e = end[bone]

            # Only use graph search for orientation of root, not position
            if bone is "root":
                s = s[3:6]
                e = e[3:6]

            # Make all angles between 0 and 360 degrees
            s = [Numeric.remainder(d, 360.0) for d in s]
            e = [Numeric.remainder(d, 360.0) for d in e]

            # Apply fix360 and fixnegative so that all angles are in the range
            # [0, 360)
            s = tuple(map(fix360, map(fixnegative, s)))
            e = tuple(map(fix360, map(fixnegative, e)))

            # Find the node in the graph corresponding to this joint position
            source[bone] = self.find_node(self.graphs[bone], s)
            goal[bone] = self.find_node(self.graphs[bone], e)

        # Run the search
        path = Heuristic(self.graphs, CNode(source), CNode(goal), self.f, self.successor).run()

        return path

    def build_order(self, asf):
        """Set the build order for the successor function."""
        # Build order always starts at the root
        self.order = ["root"]
        pos = 0

        # Create the build order based on the hierarchy specified in the ASF
        # file. The hierarchy in the ASF file looks like: "parent child child..."
        # This loop appends the list of children to the build order from each
        # line.
        while pos < len(order):
            for group in asf.hierarchy:
                if len(group) and group[0] == self.order[pos]:
                    self.order.extend(group[1:])
            pos += 1

    def comb(bones, items, position=0, current=[], current_probability=1.0):
        results = []
        bone = bones[position]

        net = bayes_net[bone]
        if bone in parents:
            parent = parents[bone]

            parent_pos = bones.index(parent)
            pbone = current[parent_pos]

        for item in items[position]:
            if not (bone in parents and \
                    (asf.bones[bone].dof or \
                    (parent in asf.bones and \
                    asf.bones[parent].dof))) or \
            parent_pos == -1:
                # We're either on the root or a bone whose parent has no DOF.
                # The bayes net has entries for these, just they're histograms
                # of the bone's position, and so the probabilities are kind of
                # low.  Treat them as if they all pass
                new_current = current + [item]
                if position == len(bones) - 1:
                    results.append(new_current)
                else:
                    children = comb(bones, items, position + 1, new_current, current_probability)
                    if len(children):
                        results.extend(children)
                    pass
            else:
                spot = (tuple(pbone.mins), tuple(item.mins))
                if spot in net:
                    probability = net[spot]
                    new_current = current + [item]
                    new_prob = current_probability * probability
                    if new_prob > EPSILON and probability > 0.1:
                        if position == len(bones) - 1:
                            results.append(new_current)
                        else:
                            children = comb(bones, items, position + 1, new_current, new_prob)
                            if len(children):
                                results.extend(children)

        res = len(results)
        if position == 0:
            print '%d likely successors' % res
        return results

    def successor (graphs, node):
        """Generate successors of a combinatoric node."""
        immediate_successors = {}
        # Create a dictionary mapping bone name to the list of successors for that
        # bone in its current position.
        for bone, n in node.iteritems ():
            adj = graphs[bone].representations[AdjacencyList]
            immediate_successors[bone] = [edge.v for edge in adj.query (n)]

        # Return a list of the combinatoric nodes
        items = []
        bones = []
        for bone in comb_order:
            if bone in immediate_successors:
                bones.append(bone)
                items.append(immediate_successors[bone])

        retval = []
        for succ in comb(bones, items):
            retsucc = {}
            for pos in range(len(succ)):
                retsucc[bones[pos]] = succ[pos]
            retval.append(CNode(retsucc))
        return retval

    def find_node (graph, pos):
        vertex_map = graph.representations[VertexMap]
        for vertex in vertex_map:
            if vertex.inside (pos):
                return vertex

    def linear_interp(start, end, pos, length):
        result = []
        for i in range(len(start)):
            compstart = start[i]
            compend   = end[i]

            pos = compstart + ((compend - compstart) * (float(pos) / float(length)))
            result.append(pos)
        return result

    def f (path, goal):
        end = path[-1]
        g = len (path)
        h = 0

        for bone in end.iterkeys():
            adj = adjacency[bone]
            endb = end.get(bone)
            goalb = goal.get(bone)

            if bone not in cached_costs:
                cached_costs[bone] = {}
            if (endb, goalb) in cached_costs[bone]:
                h += cached_costs[bone][(endb, goalb)]
                continue

            path = algorithms_c.dijkstraSearch(adj, endb, goalb)
            h += len(path)

            for i in range(1, len(path)):
                x = path[i]
                cached_costs[bone][(endb, x)] = i + 1

            for i in range(0, len(path) - 1):
                x = path[i]
                cached_costs[bone][(x, goalb)] = i + 2

        return (g + h)

# vim: ts=4:sw=4:et
