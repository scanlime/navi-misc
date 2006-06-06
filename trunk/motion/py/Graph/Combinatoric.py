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

    __slots__ = ["adjacency"]

    def __init__ (self, graphs):
        self.adjacency = {}
        for name, graph in graphs:
            self.adjacency[name] = graphs.representations[Data.AdjacencyList]

    def __iter__ (self):
        """Iterate over all the edges in the graph."""
        self._combine (self.adjacency.keys())

    def _combine (self, names):
        adj = self.adjacency[names[0]]
        for edge in adj:
            if len(names) == 1:
                u = {names[0]:edge.u}
                v = {names[0]:edge.v}
                yield adj.graph.EdgeClass (u, v)
                continue

            for e in self._combine(names[1:]):
                e.u[names[0]] = edge.u
                e.v[names[0]] = edge.v
                yield e


class VertexMap (Data.GraphRepresentation):
    def __init__ (self, graphs):
        pass


# vim: ts=4:sw=4:et
