#!/usr/bin/env python
# Copyright (C) 2006 W. Evan Sheehan
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

from Graph import algorithms_c, Combinatoric, Dot
from Graph.Algorithms import HeuristicPrint, DotPrint
from Graph.Data import Graph, Edge, AdjacencyList, EdgeList, VertexMap
from optparse import OptionParser
import random, re

class Node (object, Dot.Node):
    """Basic node class for printing faked data.
   
    The class uses the flyweight pattern because algorithms_c.dijkstraSearch
    uses a direct hash of the memory address of the node for comparison, so we
    don't want to equal nodes having different memory addresses.
    """

    _cached = {}

    def __new__ (cls, data, **kwargs):
        """Return a Node. If one doesn't already exist for this piece of data,
        create a new one.
        """
        if data not in Node._cached:
            Node._cached[data] = object.__new__ (cls)

        return Node._cached[data]

    def __init__ (self, data, **kwargs):
        """Initialize the node."""
        Dot.Node.__init__ (self, **kwargs)
        if 'label' not in self.dotAttrs:
            self.dotAttrs['label'] = str (data)
        self.data = data

    def __eq__ (self, other):
        """Nodes are equal when their data is equal."""
        return self.data == other.data

    def __ne__ (self, other):
        """Nodes are unequal when their data is unequal."""
        return self.data != other.data

    def __hash__ (self):
        """The only identifying attribute of the node is its data."""
        return hash (self.data)

    def __str__ (self):
        return str (self.data)

    def __repr__ (self):
        return "Node(%s)" % self.data


def cost (path, goal):
    """Return the cost of ``path``.
    
    Cost of a given path is the length of the path, plus the sum of each
    shortest path from the end of it to the goal.
    """
    g = len (path)
    h = 0
    for name, x in graphs.iteritems ():
        print "dijkstra from %s to %s" % (path[-1][name], goal[name])
        a = x.representations[AdjacencyList]
        h += len (algorithms_c.dijkstraSearch (a, path[-1][name], goal[name]))
    return (g + h)

def startGraph (match):
    """Create a new graph and add it to the dictionary of graphs using its name
    as the key.
    """
    graph = Graph ()
    adj = AdjacencyList (graph)
    vMap = VertexMap (graph)
    eList = EdgeList (graph)
    graphs[match.group(1)] = graph

def startEdge (match):
    """Create an edge."""
    u, v = match.groups ()
    edges.add (Edge (Node (u), Node (v)))

def endGraph (match):
    """Add the ``edges`` set to the current graph and clear ``edges``."""
    global edges
    graphs[match.group (1)].addList (edges)
    edges = set ([])

def setStart (match):
    global start
    start = match.group (1)

def setEnd (match):
    global end
    end = match.group (1)


# Option parser in case this script gets more complicated
options = OptionParser (usage="%prog <graphs>")
opts, args = options.parse_args ()

# Check the arguments
if len (args) != 1:
    options.error ("Incorrect number of arguments")

# Set up the regexs and callbacks used for parsing the file of graphs
callbacks = {re.compile (r'^\[(\w*)\]$'): startGraph, \
        re.compile (r'^(\w*) -> (\w*)$'): startEdge, \
        re.compile (r'^\[/(\w*)\]$'): endGraph, \
        re.compile (r'^start\W+([\w,]*)$'): setStart, \
        re.compile (r'^end\W+([\w,]*)$'): setEnd}

start = end = None

# Parse the file on the command line
graphs = {}
edges = set ([])
f = file (args[0], 'r')
for line in f:
    # When we find a matching regex, call the corresponding function
    for regex, callback in callbacks.iteritems ():
        m = regex.search (line)
        if m:
            callback (m)
            break
f.close ()

# If more than one graph is specified in the input, build a combinatoric graph.
if len (graphs) > 1:
    # Build a combinatoric graph from all the graphs found in the input file.
    cgraph = Graph ()
    cedges = Combinatoric.EdgeList (cgraph)

    cgraph.addList (graphs.items ())

    # Create a non-combinatoric graph from the combinatoric graph.
    graph = Graph ()
    adj = AdjacencyList (graph)
    map = VertexMap (graph)

    graph.addList ([e for e in cedges])
    
    # Save a copy of the combinatoric graph
    print "saving combined"
    f = file ('graphs/all.dot', 'w')
    DotPrint (graph, f)
    f.close ()
else:
    graph = graphs.values ()[0]

# Save a copy of each individual graph
for name, g in graphs.iteritems ():
    print "saving", name
    f = file ('graphs/%s.dot' % name, 'w')
    DotPrint (g, f)
    f.close ()

if start and end:
    source = goal = None
    for v in graph.representations[VertexMap]:
        if v.data == start:
            source = v
        elif v.data == end:
            goal = v
        if source and goal:
            break
    HeuristicPrint (graph, cost, source, goal)

# vim: ts=4:sw=4:et
