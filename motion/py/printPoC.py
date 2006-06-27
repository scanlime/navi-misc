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
    _cached = {}

    def __new__ (cls, data, **kwargs):
        if data not in Node._cached:
            Node._cached[data] = object.__new__ (cls)

        return Node._cached[data]

    def __init__ (self, data, **kwargs):
        Dot.Node.__init__ (self, **kwargs)
        if 'label' not in self.dotAttrs:
            self.dotAttrs['label'] = str (data)
        self.data = data

    def __eq__ (self, other):
        return self.data == other.data

    def __ne__ (self, other):
        return self.data != other.data

    def __hash__ (self):
        return hash (self.data)

    def __str__ (self):
        return str (self.data)

    def __repr__ (self):
        return "Node(%s)" % self.data


def cost (path, goal):
    g = len (path)
    h = 0
    for name, x in graphs.iteritems ():
        print "dijkstra from %s to %s" % (path[-1][name], goal[name])
        a = x.representations[AdjacencyList]
        h += len (algorithms_c.dijkstraSearch (a, path[-1][name], goal[name]))
    return (g + h)

def startGraph (match):
    graph = Graph ()
    adj = AdjacencyList (graph)
    vMap = VertexMap (graph)
    eList = EdgeList (graph)
    graphs[match.group(1)] = graph

def startEdge (match):
    u, v = match.groups ()
    edges.add (Edge (Node (u), Node (v)))

def endGraph (match):
    global edges
    graphs[match.group (1)].addList (edges)
    edges = set ([])


options = OptionParser (usage="%prog <graphs>")
opts, args = options.parse_args ()

if len (args) != 1:
    options.error ("Incorrect number of arguments")

callbacks = {re.compile (r'^\[(\w*)\]$'): startGraph, \
        re.compile (r'^(\w*) -> (\w*)$'): startEdge, \
        re.compile (r'^\[/(\w*)\]$'): endGraph}

# Parse the file on the command line
graphs = {}
edges = set ([])
f = file (args[0], 'r')
for line in f:
    for regex, callback in callbacks.iteritems ():
        m = regex.search (line)
        if m:
            callback (m)
            break
f.close ()

cgraph = Graph ()
#adj = Combinatoric.AdjacencyList (graph)
cmap = Combinatoric.VertexMap (cgraph)
cedges = Combinatoric.EdgeList (cgraph)

cgraph.addList (graphs.items ())

graph = Graph ()
adj = AdjacencyList (graph)
map = VertexMap (graph)
edges = EdgeList (graph)

graph.addList ([e for e in cedges])

for name, g in graphs.iteritems ():
    print "saving", name
    f = file ('graphs/%s.dot' % name, 'w')
    DotPrint (g, f)
    f.close ()

print "saving combined"
f = file ('graphs/all.dot', 'w')
DotPrint (graph, f)
f.close ()

source = random.choice ([v for v in cmap])
p = [source]
p.append (random.choice ([v for v in adj.query (p[-1])]).v)

HeuristicPrint (graph, cost, source, p[-1])
            
# vim: ts=4:sw=4:et
