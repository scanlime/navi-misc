#!/usr/bin/env python
#
# interpolate.py
#
# Copyright (C) 2005-2006 W. Evan Sheehan
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

"""Interpolate between two frames in an AMC file.

usage: interpolate.py <asf file> <graph> <bayes net> <input amc> <output amc> <start frame> <end frame>
"""

from Motion import AMC, ASFReader, Interpolate
from Dance import MotionGraph
from Graph import algorithms_c
from Graph.Data import VertexMap, AdjacencyList, CNode
from Graph.ExtraAlgorithms import ParallelBFS, Heuristic
import Numeric, sys, pickle

def save(sequence, format, file):
    amc = AMC()
    amc.format = format
    bones = {}

    for bone in sequence[0].iterkeys():
        bones[bone] = [frame[bone] for frame in sequence]

    amc.bones = dict([(bone, Numeric.array(data)) for bone, data in bones.iteritems()])
    amc.save(file)


if len(sys.argv) < 8:
    print "usage: %s <asf file> <graph> <bayes net> <input amc> <output amc> <start frame> <end frame>" % (sys.argv[0])
    sys.exit(1)

samc = AMC.from_file(sys.argv[4])

print 'loading asf'
asf = ASFReader()
asf.parse(sys.argv[1])

print 'loading graphs'
graphs = pickle.load(open(sys.argv[2]))

print 'loading bayes net'
bayes_net = pickle.load(open(sys.argv[3]))

search = Interpolate.GraphSearch(graphs, bayes_net, asf)

start = {}
end = {}

for bone, data in samc.bones.iteritems():
    start[bone] = data[int(sys.argv[-2])]
    end[bone] = data[int(sys.argv[-1])]

print "searching..."
path = search(start, end)

save (path, samc.format, sys.argv[-3])

print path

# vim: ts=4:sw=4:et
