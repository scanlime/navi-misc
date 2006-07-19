#!/usr/bin/env python
#
# build_motion_graphs.py - builds a set of weighted, directed graphs which
#       captures the probability that a particular joint will move from
#       one position in a certain direction.
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

from Graph import MotionGraph
from Graph.Data import Graph
from Graph.Combinatoric import AdjacencyList, BayesAdjacency, VertexMap, EdgeList
from Motion import AMC, ASFReader
from optparse import OptionParser
import pickle

def load (files, interval):
    amcs = []
    graphs = {}

    # Open all the AMC files at once, so we can build entire graphs at once.
    for filename in files:
        print 'loading',filename
        amcs.append (AMC.from_file (filename))

    # Build the actual graphs.  We iterate over bones, building graphs for each.
    # This assumes that we have the same bones with the same dof in each file!
    # In general, for the data we're working with, this is the case, but I'm going
    # to mark this as FIXME because it's possible we'd want to combine multiple
    # motion sequences.  The other option is to write a tool that can convert
    # one AMC file to use the bones listed in another.
    for key in amcs[0].bones.iterkeys ():
        print 'building graph for',key
        g = MotionGraph.build_graphs (key, [amc.bones[key] for amc in amcs], interval)
        graphs[key] = g

    return graphs

options = OptionParser ('%prog [options] -b <asf file> [output file] [AMC FILE]...')
options.add_option ('-d', dest='degrees', type=int, default=5,
		help='Set the discretization size of graph nodes')
options.add_option ('-b', '--bayes', dest='asf',
		help='Store the graph with a Bayesian filter')

opts, args = options.parse_args ()
if len (args) < 2:
    options.error ('incorrect number of arguments')
else:
    graphs = load (args[1:], int(opts.degrees))

    cgraph = Graph ()
    vMap = VertexMap (cgraph)
    eList = EdgeList (cgraph)

    if opts.asf:
        asf = ASFReader ()
        asf.parse (opts.asf)
        nets = MotionGraph.build_bayes (asf, args[1:], opts.degrees)

        adj = BayesAdjacency (cgraph, nets, asf)
    else:
        adj = AdjacencyList (cgraph)

    cgraph.addList (graphs.items ())

    print 'writing pickle'
    file = open (args[0], 'w')
    pickle.dump (cgraph, file)
    print 'flushing'
    file.flush ()
    file.close ()
    print 'done'
