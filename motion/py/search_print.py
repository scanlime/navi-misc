#!/usr/bin/env python

from Dance.MotionGraph import fix360, fixnegative
from Graph import algorithms_c
from Graph.Algorithms import HeuristicPrint
from Graph.Data import AdjacencyList, VertexMap
from Motion import AMC
from optparse import OptionParser
import Numeric, pickle

def findNode (node):
    """Return the vertex in the graph containing ``node``."""
    vmap = graph["root"].representations[VertexMap]
    node = [Numeric.remainder (d, 360.0) for d in node]
    node = tuple (map (fix360, map (fixnegative, node)))
    for vertex in vmap:
        if vertex.inside (node):
            return vertex

def cost (path, goal):
    adj = graph["root"].representations[AdjacencyList]
    g = len (path)
    h = algorithms_c.dijkstraSearch (adj, path[-1], goal)

    return (g + h)

def succ (graph, node):
    adj = graph.representations[AdjacencyList]
    return adj.query (node)

# Create the option parser and parse the command
options = OptionParser (usage="%prog <graph> <bayes net> <input amc> <output amc> <start frame> <end frame>")

opts, args = options.parse_args ()

if len (args) < 6:
    options.error ("Missing arguments")

# Load all the files specified on the command line
graph = pickle.load (file (args[0]))
bayes = pickle.load (file (args[1]))
samc = AMC.from_file (args[2])
# Convert the strings to ints for the frame indices
start = int (args[-2])
end = int (args[-1])

source = findNode (samc.bones["root"][start,3:6])
goal = findNode (samc.bones["root"][end,3:6])

path = HeuristicPrint (graph["root"], cost, succ, source, goal)

print path


# vim: ts=4:sw=4:et
