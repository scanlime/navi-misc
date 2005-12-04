#!/usr/bin/env python

from optparse import OptionParser
from Dance import Sequence, MotionGraph
from Graph.Data import AdjacencyList, VertexMap
import random, pickle

def clicheWalk (graph, len):
    """Find a path in graph of length len by following the edges with the
       highest probabilities.
       """
    path = []
    vertexMap = graph.representations[VertexMap]

    # Choose a random starting point.
    u = random.choice ([v for v in vertexMap])
    path.append (u.center)

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


parser = OptionParser ("usage: %prog <graph pickle> <output amc>")
parser.add_option ("--cliche", dest="cliche", default=False,
        action="store_true", help="Choose edges based on probability")
parser.add_option ("-l", "--len", dest="len", default=1000,
        type="int", help="Set length of paths")

opts, args = parser.parse_args ()

if len (args) != 2: parser.error ("graph and output file required")

print 'Reading graph'
graphs = pickle.load (open (args[0]))
bones  = {}

for bone in graphs.keys ():
    print 'Walking', bone
    if opts.cliche: bones[bone] = clicheWalk (graphs[bone], opts.len)
    else: bones[bone] = randomWalk (graphs[bone], opts.len)

for key,val in bones.iteritems ():
    print key, val

# vim:ts=4:sw=4:et
