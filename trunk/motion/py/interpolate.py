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

from Graph import algorithms_c, Combinatoric, MotionGraph
from Graph.Combinatoric import BayesAdjacency, AdjacencyList, VertexMap
from Graph.Data import Graph
from Graph.Algorithms import Heuristic
from Motion import AMC, ASFReader, Interpolate
from optparse import OptionParser
import Numeric, pickle, string, time

def save(sequence, file):
    """Save a sequence to a file.

    For now, this function writes sequence out to a file manually, instead of
    using the save() function built in to AMC object. The AMC.save() function
    is only writing out 0's.
    """
    f = open(file, "w")
    # Default format for all of our AMC files
    f.write(":FULLY-SPECIFIED\n")
    f.write(":DEGREES\n")

    # For each frame...
    for i in range(len(sequence)):
        # Write the frame number
        f.write("%d\n" % (i + 1))

        # Write the data for each bone in the frame
        for bone, data in sequence[i].iteritems():
            # Line starts with the bone name
            line = bone
            # Add all the position data
            for x in data:
                line += " %f" % (x)
            line += "\n"
            f.write(line)

    f.close()

arguments = ['asf file', 'graph', 'input amc', 'start frame', 'end frame']
options = OptionParser(usage='%prog [options] ' + \
        string.join (['<%s>' % x for x in arguments]))

options.add_option('-e', type=float, dest='epsilon', default=0.3**29, \
        help='Set the threshold for the Bayes filter') 
options.add_option('-b', '--benchmark', action='store_true', default=False, \
        help='Run with a Timer and display benchmarks of the interpolation')
options.add_option('-B', '--bayes', help='Specify a bayes net to load')
options.add_option('-o', dest='file', \
        help='Save the interpolated AMC data to a file')

opts, args = options.parse_args ()

if len (args) < len (arguments):
    options.error ('Missing %d arguments' % (len (arguments) - len(args)))
elif len (args) > len (arguments):
    options.error ('%d extra arguments' % (len (args) - len (arguments)))

samc = AMC.from_file (args[2])

print 'loading asf'
asf = ASFReader ()
asf.parse (args[0])


print 'loading graphs'
graphs = pickle.load (open(args[1]))

# Build the combinatoric graph
cgraph = Graph ()

if opts.bayes:
    print 'loading bayes net'
    bayes_net = pickle.load (open(opts.bayes))
    cAdj = BayesAdjacency (cgraph, bayes_net, asf, opts.epsilon)
else:
    cAdj = AdjacencyList (cgraph)

cVMap = VertexMap (cgraph)
cgraph.addList (graphs.items())

# Build the dictionaries for the starting and ending frames of the search
start = {}
end = {}
for bone, data in samc.bones.iteritems():
    start[bone] = data[int(args[-2]) - 1]
    end[bone] = data[int(args[-1]) - 1]

print "searching..."
if opts.benchmark:
    # Time the search
    startTime = time.clock ()
    search = Interpolate.GraphSearch (cgraph, start, end)
    endTime = time.clock ()
    print 'Time:', (endTime - startTime)
else:
    search = Interpolate.GraphSearch (cgraph, start, end)

if opts.file:
    # Save the interpolated data to a file
    save (search.path, opts.file)
else:
    # Print the interpolated data to stdout
    print
    print search.path

# vim: ts=4:sw=4:et
