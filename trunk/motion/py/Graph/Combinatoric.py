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

        for edge in combine (self.adjacency.keys ()):
            yield edge

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

        for u in combine (self.adjacency.keys ()):
            yield u

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

        edgeClass = self.adjacency.items ()[0].EdgeClass
        for v in combine (self.adjacency.keys ()):
            yield edgeClass (u, v)


class VertexMap (Data.GraphRepresentation):
    """Maps each vertex to a hash of all the edges connected to that vertex."""

    __slots__ = ["vertexMap"]

    def __init__ (self, graphs):
        self.vertexMaps = {}
        for name, graph in graphs.iteritems ():
            self.vertexMaps[name] = graph.representations[Data.VertexMap]

    def __iter__ (self):
        """Iterate over all the vertices in the graph."""
        def combine (names):
            """Recursively combine the individual vertices into the
               combinatoric nodes.
               """
            map = self.vertexMaps[names[0]]
            for vertex in map:
                if len(names) == 1:
                    yield {names[0]:vertex}
                    continue

                for v in combine (names[1:]):
                    v[names[0]] = vertex
                    yield v

        for edge in combine (self.vertexMaps.keys ()):
            yield edge

    def query (self, node):
        """Iterate over the edges containing the vertex 'u'."""
        def combineU (names):
            name = names[0]
            map = self.vertexMaps[name]

            for edge in map.query (node[name]):
                if edge.v == node[name]:
                    if len (names) == 1:
                        yield {name:edge.v}
                        continue

                    for u in combineU (names[1:]):
                        u[name] = edge.u
                        yield u

        def combineV (names):
            name = names[0]
            map = self.vertexMaps[name]

            for edge in map.query (node[name]):
                if edge.u == node[name]:
                    if len (names) == 1:
                        yield {name:edge.v}
                        continue

                    for v in combineV (names[1:]):
                        v[name] = edge.v
                        yield v

        for u in combineU (self.vertexMaps.keys ()):
            yield EdgeClass (u, node)

        for v in combineV (self.vertexMaps.keys ()):
            yield EdgeClass (node, v)


class EdgeList (Data.GraphRepresentation):
    """A simple graph representation that maps (u,v) pairs to edge objects."""

    __slots__ = ["edgeLists", "edgeClass"]

    def __init__ (self, graphs):
        self.edgeLists = {}
        self.edgeClass = graphs.items ()[0]
        for name, graph in graphs.iteritems ():
            self.edgeLists[name] = graph.representations[Data.EdgeList]

    def __iter__ (self):
        """Iterate over the edges of the graph."""
        def combine (names):
            name = names[0]
            edges = self.edgeLists[name]

            for edge in edges:
                if len (names) == 1:
                    u = {name:edge.u}
                    v = {name:edge.v}
                    yield edge
                    continue

                for e in combine (names[1:]):
                    e.u[name] = edge.u
                    e.v[name] = edge.v
                    yield e

        for edge in combine (self.edgeLists.keys ()):
            yield edge

    def query (self, u, v):
        pass


# vim: ts=4:sw=4:et
