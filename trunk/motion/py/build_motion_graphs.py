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
import sys
import MLab

class ProbabilityEdge (Edge):
    def __init__ (self, u, v):
        Edge.__init__ (self, u, v)
        self.count = 0
        self.weight = None

    def visit (self):
        self.count += 1

    def normalize (self, total):
        self.weight = float (self.count) / total

def build_graph (key, d):
    dof = d.shape[1]
    print dof,key

    g = Graph ()
    for degree in range(dof):
        data = d[:,degree]
        print '        [%f, %f]' % (MLab.min (data), MLab.max (data))

def load (filename):
    amc = AMC.from_file (filename)
    bones = {}
    for (key, data) in amc.bones.iteritems ():
        g = build_graph (key, data)

if len (sys.argv) != 2:
    print 'Usage: %s [file.amc]' % sys.argv[0]
else:
    load (sys.argv[1])
