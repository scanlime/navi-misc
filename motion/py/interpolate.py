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

from Graph.Data import AdjacencyList
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
            for x in data.center:
                line += " %f" % (x)
            line += "\n"
            f.write(line)

    f.close()

arguments = ['graph', 'input amc', 'start frame', 'end frame']
options = OptionParser(usage='%prog [options] ' + \
        string.join (['<%s>' % x for x in arguments]))
options.add_option('-e', type=float, dest='epsilon', \
        help='Set the threshold for the Bayes filter') 
options.add_option('-b', '--benchmark', action='store_true', default=False, \
        help='Run with a Timer and display benchmarks of the interpolation')
options.add_option('-o', dest='file', \
        help='Save the interpolated AMC data to a file')
opts, args = options.parse_args ()

if len (args) < len (arguments):
    options.error ('Missing %d arguments' % (len (arguments) - len(args)))
elif len (args) > len (arguments):
    options.error ('%d extra arguments' % (len (args) - len (arguments)))

samc = AMC.from_file (args[1])

print 'loading graphs'
graph = pickle.load (open(args[0]))

if opts.epsilon is not None:
    graph.representations[AdjacencyList].epsilon = opts.epsilon

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
    search = Interpolate.GraphSearch (graph, start, end)
    endTime = time.clock ()
    print '\nTime:', (endTime - startTime)
else:
    search = Interpolate.GraphSearch (graph, start, end)

if opts.file:
    # Save the interpolated data to a file
    save (search.path, opts.file)
else:
    # Print the interpolated data to stdout
    print
    print len(search.path)
    print
    print search.path

# vim: ts=4:sw=4:et
