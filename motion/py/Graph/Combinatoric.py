"""Data structures for building graphs by combining multiple graphs.
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
        pass

    def iterU (self):
        def combine (names):
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
        def combine (names):
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

    __slots__ = ["vertexMap"]

    def __init__ (self, graphs):
        self.vertexMap = {}
        for name, graph in graphs.iteritems ():
            self.vertexMap[name] = graph.representations[Data.VertexMap]

    def query (self, u):
        pass


class EdgeList (Data.GraphRepresentation):

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
