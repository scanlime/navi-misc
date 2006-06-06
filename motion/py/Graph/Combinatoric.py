"""Data structures for building graphs by combining multiple graphs.

A module for manipulating combinatoric graphs. It's easier on memory to keep
the individual graphs separate and pretend that there is a giant graph composed
of all possible combinations of the nodes in the individual graphs. These
classes allow you to manipulate this pretend graph as though it actually
existed in memory.

Classes:
    - Graph
    - AdjacencyList
    - VertexMap
    - EdgeList
"""

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

import Data

class Graph (Data.Graph):
    """A combinatoric graph."""
    def __init__ (self, graphs):
        pass

    def addGraph (self, name, graph):
        pass


class AdjacencyList (Data.GraphRepresentation):
    """Combinatoric adjacency list."""

    __slots__ = ["adjacency"]

    def __init__ (self, graphs):
        self.adjacency = {}
        for name, graph in graphs:
            self.adjacency[name] = graphs.representations[Data.AdjacencyList]

    def __iter__ (self):
        """Iterate over all the edges in the graph."""
        def combine (names):
            """Recursive function for combining the edges in the adjacency
               lists of all graphs.
               """
            adj = self.adjacency[names[0]]
            for edge in adj:
                if len(names) == 1:
                    u = {names[0]:edge.u}
                    v = {names[0]:edge.v}
                    yield adj.graph.EdgeClass (u, v)
                    continue

                for e in combine(names[1:]):
                    e.u[names[0]] = edge.u
                    e.v[names[0]] = edge.v
                    yield e

        combine (self.adjacency.keys ())

    def __contains__ (self, edge):
        """Returns true if edge exists in the adjacency list."""
        pass

    def iterU (self):
        """Iterate over all the 'u' vertices in the edges of the adjacency
           list.
           """
        def combine (names):
            """Recursive function to combine the 'u' vertices into dictionaries
               mapping the graph name to the vertex.
               """
            adj = self.adjacency[names[0]]
            for u in adj.iterU ():
                if len(names) == 1:
                    yield {names[0]:u}
                    continue
                
                for dictU in combine(names[1:]):
                    dictU[names[0]] = u
                    yield dictU

        combine (self.adjacency.keys ())

    def query (self, u):
        """Iterate over all edges going out from vertex 'u'."""
        def combine (names):
            """Recursively combine all the edges from the graphs going out from
               'u'.
               """
            adj = self.adjacency[names[0]]
            for v in adj.query(u[names[0]]):
                if len(names) == 1:
                    yield {names[0]:v}
                    continue

                for node in combine (names[1:]):
                    node[names[0]] = v
                    yield node

        combine (self.adjacency.keys ())


class VertexMap (Data.GraphRepresentation):
    """Maps each vertex to a hash of all the edges connected to that vertex."""

    __slots__ = ["vertexMap"]

    def __init__ (self, graphs):
        self.vertexMap = {}
        for name, graph in graphs.iteritems ():
            self.vertexMap[name] = graph.representations[Data.VertexMap]

    def query (self, u):
        pass


class EdgeList (Data.GraphRepresentation):
    """A simple graph representation that maps (u,v) pairs to edge objects."""

    __slots__ = ["edgeLists"]

    def __init__ (self, graphs):
        self.edgeLists = {}
        for name, graph in graphs.iteritems ():
            self.edgeLists[name] = graph.representations[Data.EdgeList]

    def __iter__ (self):
        pass

    def query (self, u, v):
        pass


# vim: ts=4:sw=4:et
