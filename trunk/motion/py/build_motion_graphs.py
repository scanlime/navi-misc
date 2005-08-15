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

from Graph.Data import Graph, Edge, AdjacencyList, VertexMap, EdgeList
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
        self.dot_label = '%.2f' % self.weight

class MotionGraph (Graph):
    __slots__ = ['nodes']

    def __init__ (self):
        Graph.__init__ (self, [DotPrint])

class MotionGraphNode:
    __slots__ = ['mins', 'maxs']

    def __init__ (self, mins, maxs):
        self.mins = mins
        self.maxs = maxs

        extents = []
        for i in range (len (mins)):
            extents.append ('[%.2f, %.2f]' % (mins[i], maxs[i]))
        self.dot_label = '\\n'.join (extents)

    def inside (self, point):
        if len(point) != len(self.mins):
            raise AttributeError ("dimension doesn't match!")
        for i in range (len (point)):
            if point[i] < self.mins[i] or point[i] > self.maxs[i]:
                return False
        return True

def cp_range (dof, angle):
    """Return a list of tuples which form a cartesian product of the
       range [0,360] in steps of angle.  This is used to build our sea
       of nodes for building the graph.
       """
    if dof == 1:
        return [(x,) for x in (range (0, 360, angle))]
    if dof == 2:
        nodes = []
        for x in range (0, 360, angle):
            for y in range (0, 360, angle):
                nodes.append ((x, y))
        return nodes
    if dof == 3:
        nodes = []
        for x in range (0, 360, angle):
            for y in range (0, 360, angle):
                for z in range (0, 360, angle):
                    nodes.append ((x, y, z))
        return nodes

def fixnegative (x):
    if x < 0:
        return x + 360
    return x

def build_graph (key, d):
    # if this is the root, we only want the first 3 dof, for now
    # FIXME - we really should do some stuff to track root orientation,
    #   but that's a much more complicated problem.
    if (key == 'root'):
        d = d[:,0:3]

    dof = d.shape[1]
    frames = d.shape[0]

    # degrees covered (angle-wise) within a single node.  Note that for some
    # bones, the number of nodes we have will be (360 / interval)^3, so be
    # sparing when decreasing this!
    interval = 5

    mins = []
    slots = []

    graph = MotionGraph ()
    adjacency_list = AdjacencyList (graph)
    vertex_map = VertexMap (graph)
    edge_list = EdgeList (graph)

    # it's silly to have angle values outside of [0,360]
    # dunno if this will handle <0
    d = Numeric.remainder (d, 360.0)

    # Create list of nodes
    nodes = {}
    for angle in cp_range (dof, interval):
        node = MotionGraphNode ([n for n in angle], [n + interval for n in angle])
        nodes[angle] = node

    graph.nodes = nodes

    # find edges
    data1 = d[0,:]
    bases = tuple (map (fixnegative, (int (n / interval) * interval for n in data1)))
    node1 = nodes[bases]

    for frame in range (1, frames):
        node2 = None
        data2 = d[frame,:]
        bases = tuple (map (fixnegative, (int (n / interval) * interval for n in data1)))
        node2 = nodes[bases]

        # check to see if it's a self-loop
        if node1 is node2:
            try:
                edge = edge_list.query (node1, node2)
                edge.visit ()
            except KeyError:
                try:
                    edge = edge_list.query (node2, node1)
                    edge.visit ()
                except KeyError:
                    edge = ProbabilityEdge (node1, node2)
                    edge.visit ()
                    graph.addList ([edge])
        else:
            try:
                edge = edge_list.query (node1, node2)
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
