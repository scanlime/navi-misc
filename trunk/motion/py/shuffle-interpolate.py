#!/usr/bin/env python
#
# shuffle-interpolate.py - Read in an AMC file, do the chaotic shuffle,
#                          interpolate the breaks, and write out a new
#                          AMC file.
#
# Copyright (C) 2005-2006 David Trowbridge
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

from Motion import AMC
from Dance import Systems, Sequence, MotionGraph
from Graph import algorithms_c
from Graph.Data import VertexMap, AdjacencyList
from Graph.ExtraAlgorithms import ParallelBFS
from optparse import OptionParser
import Numeric, sys, pickle

def comb(items):
    if len (items) == 1:
        return [[x,] for x in items[0]]
    a = comb(items[1:])
    ret = []
    for x in items[0]:
        for y in a:
            ret.append([x,] + y)
    return ret

def find_node (graph, pos):
    vertex_map = graph.representations[VertexMap]
    for vertex in vertex_map:
        if vertex.inside (pos):
            return vertex

def fixnegative (x):
    while x < 0:
        x = x + 360
    return x

def fix360 (x):
    if x == 360:
        return 0
    return x

def linear_interp (start, end, pos, len):
    result = []
    for i in range (len (start)):
        compstart = start[i]
        compend   = end[i]

        pos = compstart + ((compend - compstart) * (float(pos) / float(len)))
        result.append (pos)
    return result

def f (path, goal):
    end = path[-a]
    g = len (path)
    h = 0

    for bone in end.iterkeys():
        h += algorithms_c.dijkstraSearch (adjacency[bone], end[bone], goal[bone])

    return (g + h)


parser = OptionParser ("usage: %prog <input amc> <graph pickle> <shuffled output amc> <interpolated output amc>")
parser.add_option ("-i", "--initial", dest="ic", default="60,15,1", \
        help="A comma separated list of initial conditions for the shuffle")
parser.add_option ("-n", dest="n", type="int", default=10000, \
        help="Number of iterations for the chaotic systems")
parser.add_option ("-d", dest="depth", type="int", default=6, \
        help="Maximum depth for the graph search");

opts, args = parser.parse_args ()

if len (args) != 4: parser.error ("input, graph and 2 output files are required")

samc = AMC.from_file (args[0])

print 'shuffling sequence'
lorenz = Systems.Lorenz (16.0, 45.0, 4.0)
sequence = Sequence.Sequence (samc, lorenz, Numeric.array ([60, 15, 1]), n=opts.n)
sequence.shuffle (Numeric.array ([17.0, 2.0, -1.0]), n=30)

sequence.save (args[2], samc.format)

graphs = pickle.load (open (args[1]))

# Need the adjacency lists in the f-cost function
adjacency = {}
for bone, graph in graphs.iteritems():
    adjacency[bone] = graph.representations[AdjacencyList]

for boundary in sequence.boundaries:
    pre  = sequence[boundary - 1]
    post = sequence[boundary]

    starts = {}
    ends = {}

    for bone in samc.bones.keys ():
        start = pre[1].bones[bone]
        end   = post[1].bones[bone]

        if bone == 'root':
            start = start[3:6]
            end   = end[3:6]

        start = [Numeric.remainder (d, 360.0) for d in start]
        end   = [Numeric.remainder (d, 360.0) for d in end]

        start = tuple (map (fix360, map (fixnegative, start)))
        end   = tuple (map (fix360, map (fixnegative, end)))

        startNode = find_node (graphs[bone], start)
        endNode   = find_node (graphs[bone], end)

        starts[bone] = startNode
        ends[bone] = endNode


    print 'searching at boundary',boundary
    paths = algorithms_c.aStarSearch (adjacency, starts, ends, f)


    if paths is None:
        print 'no path found!'
        sys.exit ()

    for i in range (len (paths['root'])):
        frame = {}
        for bone in paths.keys():
            node = paths[bone][i]
            center = node.center ()

            if bone == 'root':
                # Linearly interpolate root position, for now
                rootstart = pre[1].bones['root'][0:3]
                rootend   = post[1].bones['root'][0:3]
                position = linear_interp (rootstart, rootend, i, len (paths['root']))
                center = position + center
            frame[bone] = center
        sequence.insert (frame, index)

    sequence.save (args[3], samc.format)

# vim: ts=4:sw=4:et
