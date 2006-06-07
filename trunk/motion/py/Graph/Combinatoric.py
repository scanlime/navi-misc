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

__all__ = ["AdjacenceyList", "VertexMap", "EdgeList"]

class CombinatoricRepresentation (Data.GraphRepresentation):
    def __init__ (self, graph):
        Data.GraphRepresentation.__init__ (self, graph)
        self.data = []

    def onAdd (self, data):
        try:
            if len (data) != 2:
                raise ValueError ("Combinatoric graph representations expect list or tuple of the form (name, graph), got: %s" % (data))
        except TypeError:
            raise TypeError ("Combinatoric graph representations expect a list or tuple, got: %s" % (data))



class AdjacencyList (CombinatoricRepresentation):
    """Combinatoric adjacency list."""

    def __iter__ (self):
        """Iterate over all the edges in the graph."""
        def combine (graphs):
            """Recursive function for combining the edges in the adjacency
               lists of all graphs.
               """
            name, adj = graphs[0]
            for edge in adj:
                if len(graphs) == 1:
                    u = {name: edge.u}
                    v = {name: edge.v}
                    yield self.graph.edgeClass (u, v)
                    continue

                for e in combine(graphs[1:]):
                    e.u[name] = edge.u
                    e.v[name] = edge.v
                    yield e

        for edge in combine (self.data):
            yield edge

    def __contains__ (self, edge):
        """Returns true if edge exists in the adjacency list."""
        names = []
        for (name, adj) in self.data:
            # Try to create an edge for this just this graph. If we get a
            # KeyError here, 'edge' doesn't have data for one of our graphs so
            # return False.
            try:
                e = self.edgeClass (edge.u[name], edge.v[name])
            except KeyError:
                return False

            # If this graph doesn't contain 'e', we don't contain 'edge'
            if e not in adj:
                return False

            names.append (name)

        # If 'edge' has data for a graph we don't have, we don't contain 'edge'
        if names != edge.u.keys ():
            return False
                   
        # If nothing has failed, assume we contain 'edge'
        return True

    def onAdd (self, data):
        CombinatoricRepresentation.onAdd (self, data)
       
        name, graph = data
        adj = graph.representations[Data.AdjacencyList]

        if (name, adj) in self.data:
            raise ValueError ("Duplicate graph %s" % data)
        self.data.append ((name, adj))

    def onRemove (self, graph):
        graph = tuple (graph)
        self.data.remove (graph)

    def iterU (self):
        """Iterate over all the 'u' vertices in the edges of the adjacency
           list.
           """
        def combine (graphs):
            """Recursive function to combine the 'u' vertices into dictionaries
               mapping the graph name to the vertex.
               """
            name, adj = graphs[0]
            for u in adj.iterU ():
                if len(graphs) == 1:
                    yield {name: u}
                    continue
                
                for dictU in combine(graphs[1:]):
                    dictU[name] = u
                    yield dictU

        for u in combine (self.data):
            yield u

    def query (self, u):
        """Iterate over all edges going out from vertex 'u'."""
        def combine (graphs):
            """Recursively combine all the edges from the graphs going out from
               'u'.
               """
            name, adj = self.adjacency[names[0]]
            for v in adj.query(u[name]):
                if len(graphs) == 1:
                    yield {name: v}
                    continue

                for node in combine (graphs[1:]):
                    node[name] = v
                    yield node

        for v in combine (self.adjacency.keys ()):
            yield self.graph.edgeClass (u, v)


class VertexMap (CombinatoricRepresentation):
    """Maps each vertex to a hash of all the edges connected to that vertex."""

    def __iter__ (self):
        """Iterate over all the vertices in the graph."""
        def combine (graphs):
            """Recursively combine the individual vertices into the
               combinatoric nodes.
               """
            name, map = graphs[0]
            for vertex in map:
                if len(names) == 1:
                    yield {name: vertex}
                    continue

                for v in combine (graphs[1:]):
                    v[name] = vertex
                    yield v

        for v in combine (self.data):
            yield v 

    def onAdd (self, data):
        CombinatoricRepresentation.onAdd (self, data)

        name, graph = data
        map = graph.representations[Data.VertexMap]

        if (name, map) in self.data:
            raise ValueError ("Duplicate graph %s" % (data))

        self.data.append ((name, map))

    def onRemove (self, graph):
        pass

    def query (self, node):
        """Iterate over the edges containing the vertex 'u'."""
        def combineU (graphs):
            name, map = graphs[0]

            for edge in map.query (node[name]):
                if edge.u == node[name]:
                    if len (graphs) == 1:
                        yield {name: edge.u}
                        continue

                    for u in combineU (graphs[1:]):
                        u[name] = edge.u
                        yield u

        def combineV (graphs):
            name, map = graphs[0]

            for edge in map.query (node[name]):
                if edge.u == node[name]:
                    if len (graphs) == 1:
                        yield {name: edge.v}
                        continue

                    for v in combineV (graphs[1:]):
                        v[name] = edge.v
                        yield v

        for u in combineU (self.data):
            yield self.graph.edgeClass (u, node)

        for v in combineV (self.data):
            yield self.graph.edgeClass (node, v)


class EdgeList (Data.GraphRepresentation):
    """A simple graph representation that maps (u,v) pairs to edge objects."""

    __slots__ = ["edgeLists", "edgeClass"]

    def __init__ (self, graph):
        self.edgeLists = {}
        self.edgeClass = graph.items ()[0]
        for name, graph in graph.iteritems ():
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

    def onAdd (self, graph):
        pass

    def onRemove (self, graph):
        pass

    def query (self, u, v):
        """Return the edge from 'u' to 'v'."""
        returnEdge = self.edgeClass ({}, {})
        for name in u.iterkeys ():
            edges = self.edgeLists[name]
            edge = edges.query (u[name], v[name])
            returnEdge.u[name] = edge.u
            returnEdge.v[name] = edge.v

        return returnEdge


# vim: ts=4:sw=4:et
