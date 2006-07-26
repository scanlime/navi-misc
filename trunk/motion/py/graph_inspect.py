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
options.add_option ('-b', '--bayes', action='store_true', default=False,
        help='Display Bayes net data, if available')
options.add_option ('-o', dest='file', help='Save results to a file')

opts, args = options.parse_args ()

if len (args) < 1:
    options.error ('Graph file required')

graphs = pickle.load (file (args[0], 'r'))

nodes = {}
branching = {}
avgNodes = avgBranch = 0.0

adjacency = graphs.representations[AdjacencyList]
vertices = graphs.representations[VertexMap]

for name in adjacency.data.keys ():
    nodes[name] = 0
    factor = 0.0
    vMap = vertices.data[name]
    adj = adjacency.data[name]

    for v in vMap:
        nodes[name] += 1
        factor += len ([e for e in adj.query (v)])

    avgNodes += nodes[name]
    avgBranch += factor
    branching[name] = (factor / nodes[name])

avgBranch /= avgNodes
avgNodes /= len (nodes)

output = 'Graph data\n  %-10s %10s %20s\n' % ('Graph', 'Nodes', 'Branching Factor')
output += '  ========== ========== ====================\n'
for name in nodes.iterkeys ():
    output += '  %-10s %10d %20f\n' % (name, nodes[name], branching[name])

output += '\nGraphs %5d\n' % len (nodes)
output += '\nAverages\n'
output += '  %-15s %5f\n' % ('Nodes:', avgNodes)
output += '  %-15s %5f\n' % ('Branching Factor:', avgBranch)

if opts.bayes and hasattr (adjacency, 'bayes'):
    probs = {}
    entries = {}
    avgProb = avgEntries = 0.0

    for name, net in adjacency.bayes.iteritems ():
        entries[name] = 0
        probs[name] = 0

        for p in net.itervalues ():
            entries[name] += 1
            probs[name] += p

        avgProb += probs[name]
        avgEntries += entries[name]
        probs[name] /= entries[name]

    avgProb /= avgEntries
    avgEntries /= len (entries)
        
    output += '\n\nBayes data\n  %-10s %10s %20s\n' % ('Graph', 'Entries', 'Avg. Probability')
    output += '  ========== ========== ====================\n'

    for name in probs.iterkeys ():
        output += '  %-10s %10d %20f\n' % (name, entries[name], probs[name])

    output += '\nBayes Nets %5d\n' % len (adjacency.bayes)
    output += '\nAverages\n'
    output += '  %-15s %5f\n' % ('Entries:', avgEntries)
    output += '  %-15s %5f\n' % ('Probability:', avgProb)

print output

if opts.file:
    f = file (opts.file, 'w')
    f.write (output)

# vim: ts=4:sw=4:et
