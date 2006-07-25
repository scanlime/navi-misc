#!/usr/bin/env python
#
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

from Graph import MotionGraph
from Graph.Data import VertexMap, AdjacencyList
from optparse import OptionParser
import pickle

options = OptionParser (usage='%prog [options] <graph>')
options.add_option ('-o', dest='file', help='Save results to a file')

opts, args = options.parse_args ()

if len (args) < 1:
    options.error ('Graph file required')

graphs = pickle.load (file (args[0], 'r'))

nodes = {}
branching = {}

names = graphs.representations[AdjacencyList].data.keys ()
for name in names:
    nodes[name] = 0
    factor = 0.0
    vMap = graphs.representations[VertexMap].data[name]
    adj = graphs.representations[AdjacencyList].data[name]

    for v in vMap:
        nodes[name] += 1
        factor += len ([e for e in adj.query (v)])

    branching[name] = (factor / nodes[name])

avgNodes = 0.0
avgBranch = 0.0

for name, count in nodes.iteritems ():
    avgNodes += count
avgNodes = avgNodes / len (nodes)

for name, count in branching.iteritems ():
    avgBranch += count
avgBranch = avgBranch / len (branching)

output = 'Graph data\n  %-10s %10s %20s\n' % ('Graph', 'Nodes', 'Branching Factor')
for name in nodes.iterkeys ():
    output += '  %-10s %10d %20f\n' % (name + ':', nodes[name], branching[name])

output += '\nGraphs %5d\n' % len (nodes)
output += '\nAverages\n'
output += '  %-15s %5f\n' % ('Nodes:', avgNodes)
output += '  %-15s %5f\n' % ('Branching Factor:', avgBranch)

print output

if opts.file:
    f = file (opts.file, 'w')
    f.write (output)

# vim: ts=4:sw=4:et
