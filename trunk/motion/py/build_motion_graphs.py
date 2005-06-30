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

from Graph.Data import Graph, AdjacencyList, Edge
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

class MotionGraph:
    __slots__ = ['graph', 'nodes']

    def __init__ (self):
        self.graph = Graph ()

class MotionGraphNode:
    def __init__ (self):
        pass

def build_graph (key, d):
    dof = d.shape[1]
    print dof,key

    mins = []
    slots = []

    graph = MotionGraph ()

    for degree in range(dof):
        data = d[:,degree]
        mins.append (MLab.min (data))
        max = MLab.max (data)
        size = max - mins[-1]
        slots.append (int(math.ceil (size / 5.0)))
        print '        [%f, %f] = %f * %d' % (mins[-1], max, size, slots[-1])

    print slots

    # Gross mess to create a 1, 2 or 3 dimensional array of nodes.  This will
    # need to be generalized, since the root is 6dof.  On second thought, 3
    # of those dof should be completely independent of the graph prediction,
    # and just feed back into the splicing.
    nodes = []
    if dof == 1:
        nodes = [0] * slots[0]
        for i in range (len (nodes)):
            nodes[i] = MotionGraphNode ()
    else:
        nodes = [[]] * slots[0]
        if dof == 2:
            for i in range (len (nodes)):
                nodes[i] = [0] * slots[1]
                for j in range (len (nodes[i])):
                    nodes[i][j] = MotionGraphNode ()
        else:
            for i in range (len (nodes)):
                nodes[i] = [[]] * slots[1]
                for j in range (len (nodes[i])):
                    nodes[i][j] = [0] * slots[2]
                    for k in range (len (nodes[i][j])):
                        nodes[i][j][k] = MotionGraphNode ()

    graph.nodes = nodes

    return graph

def load (filename):
    amc = AMC.from_file (filename)
    bones = {}
    for (key, data) in amc.bones.iteritems ():
        g = build_graph (key, data)

if len (sys.argv) != 2:
    print 'Usage: %s [file.amc]' % sys.argv[0]
else:
    load (sys.argv[1])
