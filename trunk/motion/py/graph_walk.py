#!/usr/bin/env python

from optparse import OptionParser
from Dance import Sequence, MotionGraph
from Graph.Data import AdjacencyList, VertexMap
from LinearAlgebra import inverse
import Motion, Numeric, pickle, random

def clicheWalk (graph, len):
    """Find a path in graph of length len by following the edges with the
       highest probabilities.
       """
    vertexMap = graph.representations[VertexMap]

    # Choose a random starting point.
    u = random.choice ([v for v in vertexMap])
    path = [u.center]

    for i in range (len):
        choice = None
        weight = 0
        # Check all the edges associated with this vertex.
        for edge in vertexMap.query (u):
            # If this vertex is the start of the edge (not the end) and the
            # edge is more likely than any other we've seen, it becomes our next
            # edge.
            if edge.u is u and edge.weight >= weight:
                weight = edge.weight
                choice = edge.v

        # Append the next vertex and set it as the current vertex.
        path.append (choice.center)
        u = choice

    return path


def randomWalk (graph, len):
    """Find a path in graph of langth len by following a random edge."""
    adjacency = graph.representations[AdjacencyList]

    # Choose a random starting place.
    u = random.choice ([v for v in adjacency.iterU ()])
    path = [u.center]

    for i in range (len):
        # Choose a random edge coming out of u.
        choice = random.choice ([edge for edge in adjacency.query (u)])
        # Append the end of that edge to the path.
        path.append (choice.v.center)
        # Set the current vertex to this new vertex
        u = choice.v

    return path


def interpolate (points):
    """Add frames to smooth the animation using cubic natural splines."""
    n = len (points)
    # A semi-arbitrary size limit for the chunk we're interpolating.
    if n > 10:
        mid = n / 2
        return interpolate (points[:mid]) + interpolate (points[mid-1:])

    # n is now the number of intervals over which we interpolate, not the number
    # of points.
    n -= 1
    # Create an list of all the intervals.
    intervals = []
    for i in range (n):
        t = 6 * i
        intervals.append (((t, points[i]), (t + 6, points[i + 1])))

    # Create the matrices
    s = n * 4
    A = Numeric.zeros ((s, s))
    b = Numeric.zeros ((1, s))

    # FIXME - Filling the matrices could probably be optimized more...

    row = 0
    # For each interval, constrain the cubic approximation to equal the
    # endpoints at the endpoints.
    for i in range (n):
        col = 4 * i
        t, y = intervals[i][0]
        A[row, col:col+4] = [1, t, t**2, t**3]
        b[row] = y
        row += 1

        t, y = intervals[i][1]
        A[row, col:col+4] = [1, t, t**2, t**3]
        b[row] = y
        row += 1

    # Except for the end points, constrain the first and second derivatives of
    # each point to be equal across intervals.
    for i in range (n):
        col = 4 * i
        t, y = intervals[i][1]
        A[row, col:col+8] = [0, 1, 2*t, 3*t**2, 0, -1, -2*t, -3*t**2]
        row += 1
        A[row, col:col+8] = [0, 0, 2, 6*t, 0, 0, -2, -6*t]
        row += 1

    # Constrain the second derivatives of the endpoints to be 0.
    t, y = intervals[0][0]
    A[row, :4] = [0, 0, 2, 6*t]
    row += 1
    t, y = intervals[-1][1]
    A[row, -4:] = [0, 0, 2, 6*t]

    # Solve for the coefficients.
    Ainv = inverse (A)
    z    = Numeric.matrixmultiply (Ainv, b)

    # Fill in 5 points between each point we have now to smooth the sequence.
    # FIXME - Implement

    return points


parser = OptionParser ("usage: %prog <graph pickle>")
parser.add_option ("--cliche", dest="cliche", default=False,
        action="store_true", help="Choose edges based on probability")
parser.add_option ("-l", "--len", dest="len", default=1000,
        type="int", help="Set length of paths")

opts, args = parser.parse_args ()

if len (args) != 1: parser.error ("graph file required")

# print 'Reading graph'
graphs = pickle.load (open (args[0]))
bones  = {}

for bone in graphs.keys ():
    # print 'Walking', bone

    if opts.cliche:
        bones[bone] = interpolate (Numeric.array (clicheWalk (graphs[bone], opts.len)))
    else:
        bones[bone] = interpolate (Numeric.array (randomWalk (graphs[bone], opts.len)))

print ":FULLY-SPECIFIED"
print ":DEGREES"

for i in range (opts.len):
    print i
    for bone,frames in bones.iteritems ():
        if bone == "root":
            s = bone + " 0 0 0"
        else:
            s = bone

        for angle in frames[i]:
            a = " %6f" % (angle)
            s += a
        print s


# vim:ts=4:sw=4:et
