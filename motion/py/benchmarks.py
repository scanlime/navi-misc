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

from Graph import algorithms_c
from Graph.Algorithms import Heuristic
from Graph.Data import AdjacencyList, VertexMap
from optparse import OptionParser
import random, re, time

def cost (path, goal):
    """Return the cost of ``path``.
    
    Cost of a given path is the length of the path, plus the sum of each
    shortest path from the end of it to the goal.
    """
    # Cost is g + h
    g = len (path)
    h = 0
    # List of all paths that are at their goal
    done = []
    # List of all paths not at their goal
    undone = []

    for name, x in input.graphs.iteritems ():
        a = x.representations[AdjacencyList]
        l = len (algorithms_c.dijkstraSearch (a, path[-1][name], goal[name]))
        if path[-1][name] == goal[name]:
            done.append (l)
        else:
            undone.append (l)
        h += l

    for i in done:
        for j in undone:
            h += j - i

    return (g + h)


def build_path (graph, length):
    nodes = [n for n in graph.representations[VertexMap]]
    start = random.choice (nodes)
    if not length:
        end = random.choice (nodes)
    else:
        adj = graph.representations[AdjacencyList]
        end = start
        for i in range (length):
            end = random.choice ([e for e in adj.query (end)]).v

    return (start, end)


# Set up and parse the command-line options for this script
options = OptionParser (usage="%prog <graphs>")
options.add_option ('-o', '--output',
        help='Store benchmark data in a comma separated value file')
options.add_option ('-l', '--length', type=int,
        help='Specify a path length for the searches')
options.add_option ('-i', '--iterations', type=int, default=5,
        help='Specify the number of searches to run (default 5)')
opts, args = options.parse_args ()

# Check the arguments
if len (args) < 1:
    options.error ("At least one graph file required")

output = 'Nodes'.center (15) + 'Branch'.center (15) + 'Run Time'.center (15) + '\n'

# Parse the input file
for inputFile in args:
    input = Input (inputFile)

    avgNodes = avgBranch = 0.0

    for v in graph.representations[VertexMap]:
        avgNodes += 1
        for e in graph.representations[AdjacencyList].query (v):
            avgBranch += 1
    avgBranch /= avgNodes

    results = 0.0
    for i in range (opts.iterations):
        s, e = build_path (graph, opts.length)
        # Go!
        startTime = time.clock ()
        if opts.searchPrint:
            HeuristicPrint (graph, cost, s, e)
        else:
            Heuristic (graph, cost, s, e)
        endTime = time.clock ()

        results += (endTime - startTime)

    results /= opts.iterations

    output += '%15d %15f %15f\n' % (avgNodes, avgBranch, results)

    if opts.output:
        f = file (opts.output, 'w')
        f.write ('#' + output)
    else:
        print
        print output
    
# vim: ts=4:sw=4:et
