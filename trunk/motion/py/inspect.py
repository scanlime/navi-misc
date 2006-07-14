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

for name, graph in graphs.iteritems ():
    nodes[name] = 0
    branching[name] = 0
    vMap = graph.representations[VertexMap]
    adj = graph.representations[AdjacencyList]

    for v in vMap:
        nodes[name] += 1
        branching[name] += len ([e for e in adj.query (v)])

    branching[name] /= nodes[name]

avgNodes = avgBranch = 0

for name, count in nodes.iteritems ():
    avgNodes += count
avgNodes /= len (nodes)

for name, count in branching.iteritems ():
    avgBranch += count
avgBranch /= len (nodes)

output = 'Graph data\n\nNodes\n'
for name, count in nodes.iteritems ():
    output += '  %s: %f\n' % (name, count)

output += '\nBranching factor\n'
for name, factor in branching.iteritems ():
    output += '  %s: %f\n' % (name, count)

output += '\nAverages\n'
output += '  Nodes: %f\n' % avgNodes
output += '  Branching Factor: %f\n' % avgBranch

print output

if opts.file:
    f = file (opts.file, 'w')
    f.write (output)

# vim: ts=4:sw=4:et
