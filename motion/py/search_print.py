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

    __slots__ = ['graphs', 'bayes']

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

    grammar = OneOrMore (graph | bayes) | Suppress (comment)

    def __init__ (self, f):
        self.graph.setParseAction (self.setGraph)
        self.bayes.setParseAction (self.setBayes)
        self.grammar.parseFile (f)

    def setGraph (self, s, loc, toks):
        pass

    def setBayes (self, s, loc, toks):
        pass


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


# Option parser in case this script gets more complicated
options = OptionParser (usage="%prog <graphs>")
opts, args = options.parse_args ()

graphs = {}

# Check the arguments
if len (args) != 1:
    options.error ("Incorrect number of arguments")

input = Input (args[0])

# vim: ts=4:sw=4:et
