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

# Load the graph
graph = pickle.load (file (args[0], 'r'))

# Initialize the variables for the graph data
nodes = {}
branching = {}
avgNodes = avgBranch = 0.0

# Require the adjacency list and vertex map representations for inspecting the
# graphs.
adjacency = graph.representations[AdjacencyList]
vertices = graph.representations[VertexMap]

# For each graph in the combined graph...
for name in adjacency.data.keys ():
    # Initialize the number of nodes in the graph and the average branching
    # factor of the graph.
    nodes[name] = 0
    factor = 0.0
    # Get the adjacency list and vertex map for the single graph
    vMap = vertices.data[name]
    adj = adjacency.data[name]

    for v in vMap:
        # Count the number of nodes
        nodes[name] += 1
        # Add the branching factor of each node to the average
        factor += len ([e for e in adj.query (v)])

    # Add the number of nodes in each graph to the average number of nodes in
    # all graphs.
    avgNodes += nodes[name]
    # Add the sum of each nodes branching factor to the average branching
    # factor for all graphs
    avgBranch += factor
    # Calculate the average branching factor of this graph by dividing the
    # sum of every node's branching factor by the number of nodes in the graph.
    branching[name] = (factor / nodes[name])

# Calculate the average branching factor and average number of nodes for all
# graphs.
avgBranch /= avgNodes
avgNodes /= len (nodes)
combNodes = 1
for n in nodes.itervalues ():
    combNodes *= n

# Assemble the output of the graph data
output = 'Graph data'.center (44) + '\n\n  %s %s %s\n' % ('Graph'.center (10),
        'Nodes'.center (10), 'Branching Factor'.center (20))
output += '  ========== ========== ====================\n'
for name in nodes.iterkeys ():
    output += '  %-10s %10d %20f\n' % (name, nodes[name], branching[name])

output += '\nGraphs %5d\n' % len (nodes)
output += '\nTotal Combined Nodes %10d\n' % combNodes
output += '\nAverages\n'
output += '  %-15s %5f\n' % ('Nodes:', avgNodes)
output += '  %-15s %5f\n' % ('Branch Factor:', avgBranch)

if opts.bayes and hasattr (adjacency, 'bayes'):
    # Initialize the variables for the Bayes net data
    probs = {}
    entries = {}
    avgProb = avgEntries = 0.0

    # For each Bayes net...
    for name, net in adjacency.bayes.iteritems ():
        # Initialize the number of entries and avg. probability to 0
        entries[name] = 0
        probs[name] = 0.0

        # Sum the number of entries and the probabilities
        for p in net.itervalues ():
            entries[name] += 1
            probs[name] += p

        # Add the sum of the probabilities to the average probability, and the
        # total entries to the average number of entries.
        avgProb += probs[name]
        avgEntries += entries[name]
        # Calculate the average probability of this net
        probs[name] /= entries[name]

    # Calculate the average probability and average number of entries for all
    # Bayes nets.
    avgProb /= avgEntries
    avgEntries /= len (entries)
       
    # Assemble the output of the Bayes net data
    output += '\n\n' + 'Bayes data'.center (44) + '\n  %s %s %s\n' % \
            ('Graph'.center (10), 'Entries'.center (10),
                    'Avg.  Probability'.center (20))
    output += '  ========== ========== ====================\n'

    for name in probs.iterkeys ():
        output += '  %-10s %10d %20f\n' % (name, entries[name], probs[name])

    output += '\nBayes Nets %5d\n' % len (adjacency.bayes)
    output += '\nAverages\n'
    output += '  %-15s %5f\n' % ('Entries:', avgEntries)
    output += '  %-15s %5f\n' % ('Probability:', avgProb)

# Print to stdout
print output

# Print to a file if specified on the command line
if opts.file:
    f = file (opts.file, 'w')
    f.write (output)

# vim: ts=4:sw=4:et
