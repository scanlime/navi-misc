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
from Graph.Algorithms import HeuristicPrint, DotPrint, Heuristic
from Graph.Data import Graph, Edge, AdjacencyList, EdgeList, VertexMap
from optparse import OptionParser
from pyparsing import *
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


class Input:
    """Parser for the input files of the script."""

    __slots__ = ['graphs', 'nets', 'source', 'goal']

    # Define the grammar
    node    = Word (alphanums)
    name    = Word (alphanums)
    edge    = Group (node + Suppress ('->') + node)
    graph   = Suppress ('[graph') + name + Suppress (']') + OneOrMore (edge) + \
              Suppress ('[/graph]')

    prob    = Combine (Optional (Word ('01')) + '.' + Word (nums))
    entry   = Group (Group (Optional (node + Suppress (','), '') + node) + \
              Suppress ('=') + prob)
    bayes   = Suppress ('[bayes') + name + Suppress (']') + OneOrMore (entry) + \
              Suppress ('[/bayes]')

    comment = '#' + Optional (restOfLine)
    section = graph | bayes

    grammar = OneOrMore (section) | Suppress (comment)

    def __init__ (self, f):
        self.graphs = {}
        self.nets = {}
        self.source = None
        self.goal = None
        self.graph.setParseAction (self.setGraph)
        self.bayes.setParseAction (self.setBayes)
        self.grammar.parseFile (f)

    def setGraph (self, s, loc, toks):
        g = Graph ()
        AdjacencyList (g)
        VertexMap (g)
        EdgeList (g)
        
        g.addList ([Edge (Node (u), Node (v)) for u, v in toks[1:]])

        self.graphs[toks[0]] = g

    def setBayes (self, s, loc, toks):
        b = {}
        
        for entry in toks[1:]:
            b[tuple (entry[0])] = entry[1]

        self.nets[toks[0]] = b


def cost (path, goal):
    """Return the cost of ``path``.
    
    Cost of a given path is the length of the path, plus the sum of each
    shortest path from the end of it to the goal.
    """
    g = len (path)
    h = 0
    done = []
    undone = []
    for name, x in input.graphs.iteritems ():
        print "dijkstra from %s to %s" % (path[-1][name], goal[name])
        a = x.representations[AdjacencyList]
        l = len (algorithms_c.dijkstraSearch (a, path[-1][name], goal[name]))
        if path[-1][name] == goal[name]:
            done.append (l)
        else:
            undone.append (l)
        h += l

    for i in done:
        for j in undone:
            h += j - i

    return (g + h)


def build_path (graph, length):
    nodes = [n for n in graph.representations[VertexMap]]
    start = random.choice (nodes)
    if not length:
        end = random.choice (nodes)
    else:
        adj = graph.representations[AdjacencyList]
        end = start
        for i in range (length):
            end = random.choice ([e for e in adj.query (end)]).v

    return (start, end)


# Set up and parse the command-line options for this script
options = OptionParser (usage="%prog <graphs>")
options.add_option ('-s', dest='searchPrint', action='store_true', default=False,
        help='Print each step of the search to a .dot file')
options.add_option ('-p', dest='graphPrint', action='store_true', default=False,
        help='Print each graph unmolested to a .dot file')
options.add_option ('-c', '--csv',
        help='Store benchmark data in a comma separated value file')
options.add_option ('-l', '--length', type=int,
        help='Specify a path length for the searches')
options.add_option ('-i', '--iterations', type=int, default=5,
        help='Specify the number of searches to run (default 5)')
opts, args = options.parse_args ()

# Check the arguments
if len (args) != 1:
    options.error ("Incorrect number of arguments")

# Parse the input file
input = Input (args[0])

if opts.graphPrint:
    for name, g in input.graphs.iteritems ():
        # Save a copy of each graph, unmolested
        print "saving %s graph" % name
        f = file ('graphs/%s.dot' % name, 'w')
        DotPrint (g, f)
        f.close ()

# Build a combinatoric graph
cgraph = Graph ()
cEdges = Combinatoric.EdgeList (cgraph)
cgraph.addList (input.graphs.items ())

# Build a graph with regular representations because a combinatoric
# representation generates new nodes each time it's accessed.
graph = Graph ()
AdjacencyList (graph)
EdgeList (graph)
VertexMap (graph)
graph.addList ([e for e in cEdges])

if opts.graphPrint:
    # Print the combined graph
    print "saving combined graph"
    f = file ('graphs/combined.dot', 'w')
    DotPrint (graph, f)
    f.close ()

results = {}
for i in range (opts.iterations):
    s, e = build_path (graph, opts.length)
    # Go!
    startTime = time.clock ()
    if opts.searchPrint:
        res = HeuristicPrint (graph, cost, s, e)
    else:
        res = Heuristic (graph, cost, s, e)
    endTime = time.clock ()

    results[res] = (endTime - startTime)


# vim: ts=4:sw=4:et
