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

from Motion import AMC, ASFReader
from Dance import Systems, Sequence, MotionGraph
from Graph import algorithms_c
from Graph.Data import VertexMap, AdjacencyList, CNode
from Graph.ExtraAlgorithms import ParallelBFS, Heuristic
from optparse import OptionParser
import Numeric, sys, pickle

def comb (bones, items):
    """Returns a list of combinatoric nodes created by combining nodes from
       items.
       """
    if len (items) == 1:
        return [{bone: x} for x in items[0]]

    a = comb(bones[1:], items[1:])
    ret = []
    for x in items[0]:
        for y in a:
            y[bones[0]] = x
            ret.append(y)
    return ret

def successor (graphs, node):
    """Generate successors of a combinatoric node."""
    immediate_successors = {}
    # Create a dictionary mapping bone name to the list of successors for that
    # bone in its current position.
    for bone, n in node.iteritems ():
        adj = graphs[bone].representations[AdjacencyList]
        immediate_successors[bone] = [edge.v for edge in adj.query (n)]

    # Return a list of the combinatoric nodes
    return [CNode (x) for x in comb (immediate_successors.keys (),
            immediate_successors.values ())]

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

def build_order(asf):
    order = ['root']
    pos = 0
    while pos < len(order):
        for group in asf.hierarchy:
            if len(group) and group[0] == order[pos]:
                order.extend(group[1:])
        pos += 1

    print order
    return order


parser = OptionParser ("usage: %prog <asf file> <input amc> <graph pickle> <bayes net pickle> <shuffled output amc> <interpolated output amc>")
parser.add_option ("-i", "--initial", dest="ic", default="60,15,1", \
        help="A comma separated list of initial conditions for the shuffle")
parser.add_option ("-n", dest="n", type="int", default=10000, \
        help="Number of iterations for the chaotic systems")

opts, args = parser.parse_args()

if len(args) != 6: parser.error ("input, graph, bayes net and 2 output files are required")

samc = AMC.from_file(args[1])

print 'shuffling sequence'
lorenz = Systems.Lorenz (16.0, 45.0, 4.0)
sequence = Sequence.Sequence (samc, lorenz, Numeric.array ([60, 15, 1]), n=opts.n)
sequence.shuffle (Numeric.array ([17.0, 2.0, -1.0]), n=30)

sequence.save (args[4], samc.format)

print 'loading asf'
asf = ASFReader()
asf.parse(args[0])

comb_order = build_order(asf)

print 'loading graphs'
graphs = pickle.load(open(args[2]))

print 'loading bayes net'
bayes_net = pickle.load(open(args[3]))

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
    paths = Heuristic (graphs, CNode (starts), CNode (ends), f, successor).run ()


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

    sequence.save(args[5], samc.format)

# vim: ts=4:sw=4:et
