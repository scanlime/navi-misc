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

from Dance import MotionGraph
from Graph import algorithms_c
from Graph.Data import VertexMap, AdjacencyList
from Graph.ExtraAlgorithms import ParallelBFS, Heuristic
from Motion import AMC, ASFReader, Interpolate
from optparse import OptionParser
import Numeric, pickle

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


options = OptionParser(usage="%prog <asf file> <graph> <bayes net> <input amc> <output amc> <start frame> <end frame>")

opts, args = options.parse_args()

if len(args) < 7:
    options.error("Missing %d arguments" % (7 - len(args)))

samc = AMC.from_file(args[3])

print 'loading asf'
asf = ASFReader()
asf.parse(args[0])

print 'loading graphs'
graphs = pickle.load(open(args[1]))

print 'loading bayes net'
bayes_net = pickle.load(open(args[2]))

search = Interpolate.GraphSearch(graphs, bayes_net, asf)

start = {}
end = {}

for bone, data in samc.bones.iteritems():
    start[bone] = data[int(args[-2]) - 1]
    end[bone] = data[int(args[-1]) - 1]

print "searching..."
path = search(start, end)

save (path, args[-3])

# vim: ts=4:sw=4:et
