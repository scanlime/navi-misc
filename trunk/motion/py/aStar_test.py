#!/usr/bin/env python
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

from Graph import algorithms_c
from Graph.Data import AdjacencyList, EdgeList, VertexMap
import math, pickle, random, sys

def f (path, goal):
    """Calculate the f-cost of a path"""
    # cost of current path
    g = len (path)
    h = 0

    # cost to goal
    h = len (algorithms_c.dijkstraSearch (adjacency, path[-1], goal))

    return g + h


if len (sys.argv) != 2:
    print "usage: %s <graph pickle>" % (sys.argv[0])
    sys.exit (1)
  
graph = pickle.load (open(sys.argv[1]))
vMap = graph["root"].representations[VertexMap]
nodes = [v for v in vMap]
actualPath = [random.choice (nodes)]
adjacency = graph["root"].representations[AdjacencyList]
for i in range (3):
    actualPath.append (random.choice (
                       [edge for edge in adjacency.query (actualPath[-1])]).v)

print "Searching for path from %s to %s..." % (actualPath[0], actualPath[-1])
print "    Estimated cost: %d" % (f([actualPath[0]], actualPath[-1]))
print

path = algorithms_c.aStarSearch (adjacency,
                                 actualPath[0],
                                 actualPath[-1],
                                 f)

if path:
    print "    Path of length %d found" % (len(path))
    for element in path:
        print element
else:
    print "    No path found"

print
print "Original path:"
for n in actualPath:
    print "    %s" % (n)

# vim: ts=4:sw=4:et
