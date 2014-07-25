#!/usr/bin/env python
#
# shuffle-interpolate.py - Read in an AMC file, do the chaotic shuffle,
#                          interpolate the breaks, and write out a new
#                          AMC file.
#
# Copyright (C) 2005-2007 David Trowbridge
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
shuffled = sequence.shuffle (Numeric.array ([17.0, 2.0, -1.0]))

# FIXME
# sequence.save (args[4], samc.format)

print 'loading asf'
asf = ASFReader()
asf.parse(args[0])

# parents = {}
# for group in asf.hierarchy:
    # if len(group) == 0:
        # continue
    # for child in group[1:]:
        # parents[child] = group[0]

print 'loading graphs'
graphs = pickle.load(open(args[2]))

print 'loading bayes net'
bayes_net = pickle.load(open(args[3]))

for boundary in sequence.boundaries:
    start = sequence[boundary - 1]
    end = sequence[boundary]

    print 'searching at boundary',boundary
    paths = Heuristic (graphs, CNode (starts), CNode (ends), f, successor).run ()
    print paths

    if paths is None:
        print 'no path found!'
        sys.exit ()

    for i in range(len(paths)):
        frame = {}
        for bone in paths[i].data.keys():
            node = paths[i].data[bone]
            center = node.center

            if bone == 'root':
                # Linearly interpolate root position, for now
                rootstart = pre[1].bones['root'][0:3]
                rootend   = post[1].bones['root'][0:3]
                position = linear_interp (rootstart, rootend, i, len(paths))
                center = position + center
            frame[bone] = center
        sequence.insert (frame, boundary + i)

    # FIXME
    # sequence.save(args[5], samc.format)

# vim: ts=4:sw=4:et
