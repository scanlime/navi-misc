#!/usr/bin/env python

"""Create an animation by traversing a motion graph.

usage: graph_walk.py [options] <graph pickle>
"""

from optparse import OptionParser
from random import random
from Dance import Sequence
from Graph import MotionGraph
from Graph.Data import AdjacencyList, VertexMap
from Motion.Interpolate import spline
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

def weightedWalk(graph, length):
    vertexMap = graph.representations[VertexMap]

    u = random.choice ([v for v in vertexMap])
    path = [u.center]
    for i in range (length):
        edges = []
        weight = random()
        for edge in vertexMap.query (u):
            if edge.u is u and weight < edge.weight:
                edges.append (edge)
        edges.sort(cmp=lambda x,y: cmp(x.weight, y.weight))
        path.append(edges[0].v.center)

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


parser = OptionParser ("%prog [options] <graph pickle>")
parser.add_option("--type", dest="type", default='weighted',
                   help="Select type of walk")
parser.add_option("-l", "--len", dest="len", default=1000,
                   type="int", help="Set length of paths")
parser.add_option("-q", "--quality", dest="quality", default=5,
                  type="int",
                  help="Set the number of points to insert during interpolation")
parser.add_option("-i", "--interpolate", dest="interpolate",
                  help="Interpolate the walk and save it to a file")

opts, args = parser.parse_args()

if len (args) < 1:
    parser.error("graph file required")

# print 'Reading graph'
graphs = pickle.load(open(args[-1]))
bones = {}
smoothed = {}

for bone in graphs.keys ():
    if opts.type == 'cliche':
        bones[bone] = clicheWalk (graphs[bone], opts.len)
    elif opts.type == 'random':
        bones[bone] = randomWalk (graphs[bone], opts.len)
    elif opts.type == 'weighted':
        bones[bone] = weightedWalk (graphs[bone], opts.len)

    if hasattr(opts, "interpolate"):
        smoothed[bone] = spline(bones[bone], opts.quality)

# Write the interpolated data to a file
if hasattr(opts, "interpolate"):
    file = open(opts.interpolate, "w")
    file.write(":FULLY-SPECIFIED\n")
    file.write(":DEGREES\n")

    for i in range(opts.len * opts.quality):
        file.write("%d\n" % (i))

        for bone, frames in smoothed.iteritems():
            if bone == "root":
                s = bone + " 0 0 0"
            else:
                s = bone
            for angle in frames[i]:
                a = " %6f" % (angle)
                s += a
            s += "\n"
            file.write(s)

# FIXME - Hack to print out the data because I had some trouble using AMC.save()
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
