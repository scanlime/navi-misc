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

    u = random.choice ([u for u in adjacency.iterU ()])
    path.append (u.center)

    for i in range (len):
        choice = None
        weight = 0
        for edge in vertexMap.query (u):
            if edge.u is u and edge.weight >= weight:
                weight = edge.weight
                choice = edge.v

        path.append (choice.center)
        u = choice

    return path


def randomWalk (graph, len):
    """Find a path in graph of langth len by following a random edge."""
    pass


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

print bones

# vim:ts=4:sw=4:et
