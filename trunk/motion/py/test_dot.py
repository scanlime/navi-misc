#!/usr/bin/env python
#
# test_dot.py - simple test for dot printing
#
# Copyright (C) 2005 David Trowbridge
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
#

from Graph import Graph, VertexMap, Edge
from GraphAlgorithms import DotPrint

nodes = [1, 2, 3, 4, 5]

graph = Graph ()
map = VertexMap (graph)

class DotEdge (Edge):
    def __init__ (self, u, v):
        Edge.__init__ (self, u, v)
        self.dot_label = ''

edges = [
        DotEdge (nodes[0], nodes[2]),
        DotEdge (nodes[1], nodes[4]),
        DotEdge (nodes[2], nodes[3]),
        ]

graph.addList (edges)

DotPrint (map)
