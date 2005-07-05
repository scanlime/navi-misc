#!/usr/bin/env python
#
# build_motion_graphs.py - builds a set of weighted, directed graphs which
#       captures the probability that a particular joint will move from
#       one position in a certain direction.
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

from Graph.Data import Graph, Edge, AdjacencyList, VertexMap, EdgeMap
from Graph.Algorithms import DotPrint
from Motion import AMC

import Numeric
import MLab

import sys
import math

class ProbabilityEdge (Edge):
    def __init__ (self, u, v):
        Edge.__init__ (self, u, v)
        self.count = 0
        self.weight = None

    def visit (self):
        self.count += 1

    def normalize (self, total):
        self.weight = float (self.count) / total
        self.dot_label = '%f' % self.weight

class MotionGraph (Graph):
    __slots__ = ['nodes']

    def __init__ (self):
        Graph.__init__ (self)

class MotionGraphNode:
    __slots__ = ['mins', 'maxs']

    def __init__ (self, mins, maxs):
        self.mins = mins
        self.maxs = maxs

        extents = []
        for i in range (len (mins)):
            extents.append ('[%f, %f]' % (mins[i], maxs[i]))
        self.dot_label = '\\n'.join (extents)

    def inside (self, point):
        if len(point) != len(self.mins):
            raise AttributeError ("dimension doesn't match!")
        for i in range (len (point)):
            if point[i] < self.mins[i] or point[i] > self.maxs[i]:
                return False
        return True

def build_graph (key, d):
    # if this is the root, we only want the first 3 dof
    if (key == 'root'):
        d = d[:,0:3]

    dof = d.shape[1]
    frames = d.shape[0]

    # degrees covered (angle-wise) within a single node
    interval = 20.0

    mins = []
    slots = []

    graph = MotionGraph ()
    adjacency_list = AdjacencyList (graph)
    vertex_map = VertexMap (graph)
    edge_map = EdgeMap (graph)

    # it's silly to have angle values outside of [0,360]
    # dunno if this will handle <0
    d = Numeric.fmod (d, 360.0)

    for degree in range(dof):
        data = d[:,degree]
        mins.append (MLab.min (data))
        max = MLab.max (data)
        size = max - mins[-1]
        if size == 0.0:
            # FIXME - should probably throw something
            return
        slots.append (int (math.ceil (size / interval)))

    # Create list of nodes
    nodes = []
    for i in range (slots[0]):
        min0 = mins[0] + i * interval
        if dof == 1:
            nodes.append (MotionGraphNode ([min0], [min0 + interval]))
        else:
            for j in range (slots[1]):
                min1 = mins[1] + j * interval
                if dof == 2:
                    nodes.append (MotionGraphNode ([min0, min1], [min0 + interval, min1 + interval]))
                else:
                    for k in range (slots[2]):
                        min2 = mins[2] + k * interval
                        nodes.append (MotionGraphNode ([min0, min1, min2], [min0 + interval, min1 + interval, min2 + interval]))

    graph.nodes = nodes

    # find edges
    data1 = d[0,:]
    for node in nodes:
        if node.inside (data1):
            node1 = node
    for frame in range (1, frames):
        node2 = None
        data2 = d[frame,:]
        for node in nodes:
            if node.inside (data2):
                node2 = node

        # check to see if it's a self-loop
        if node1 is node2:
            try:
                edge = edge_map.query (node1, node2)
                edge.visit ()
            except KeyError:
                try:
                    edge = edge_map.query (node2, node1)
                    edge.visit ()
                except KeyError:
                    edge = ProbabilityEdge (node1, node2)
                    edge.visit ()
                    graph.addList ([edge])
        else:
            try:
                edge = edge_map.query (node1, node2)
                edge.visit ()
            except KeyError:
                edge = ProbabilityEdge (node1, node2)
                edge.visit ()
                graph.addList ([edge])
        data1 = data2
        node1 = node2

    for vertex in vertex_map:
        # Normalize probabilities for all edges coming out of a vertex. The sum
        # of probabilities along edges coming out of any given vertex will be 1.
        total = 0
        edges = vertex_map.query (vertex)
        for edge in edges:
            if edge.u is vertex:
                total += edge.count
        for edge in edges:
            if edge.u is vertex:
                edge.normalize (total)

    return graph

def load (filename):
    amc = AMC.from_file (filename)
    bones = {}
    for (key, data) in amc.bones.iteritems ():
        print 'building graph for',key
        g = build_graph (key, data)
        if g is not None:
            f = file ('graphs/%s.dot' % key, 'w')
            DotPrint (g, f)
            f.close ()

if len (sys.argv) != 2:
    print 'Usage: %s [file.amc]' % sys.argv[0]
else:
    load (sys.argv[1])
