#!/usr/bin/env python
#
# shuffle.py - Read in an AMC file, do the chaotic shuffle, write out an AMC
#              file.
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

"""Shuffle a motion sequence

usage: shuffle.py [options] <input file> <output file>
"""

from Motion import AMC
from Dance import Systems
from Dance.Sequence import Sequence
from optparse import OptionParser
import Numeric, string

def save(sequence, file):
    """Save a sequence to a file.
   
    'sequence' is a list of dictionaries. Each dictionary represents a single
    frame in the motion; it maps a bone name to a list of floats representing
    an angle for each degree of freedom. This function builds a single 2D
    Numeric array for each bone; the first dimension is the frame, the second
    dimension is the data for the frame. This dictionary is set to the 'bones'
    attribute of an AMC object and saved to a file specified by 'file'. 'file'
    is a string.
    """
    amc = AMC()
    bones = {}

    for bone in sequence[0].iterkeys():
        bones[bone] = [frame[bone] for frame in sequence]

    amc.bones = dict([(bone, Numeric.array(data)) for bone, data in bones.iteritems()])
    amc.save(file)


# Set up the options
parser = OptionParser("usage: %prog [options] <input file> <output file>")
parser.add_option("-i", "--initial", dest="ic", default="15,60,1", \
        help="A comma separated list of initial conditions for the shuffle")
parser.add_option("-n", dest="n", type="int", default=10000, \
        help="Number of iterations for the chaotic systems")

opts, args = parser.parse_args()

if len (args) != 2:
    parser.error("input and output file are required")

# Load the AMC
amc = AMC.from_file(args[0])

# Create the dance sequence
lorenz = Systems.Lorenz(16.0, 45.0, 4.0)
sequence = Sequence(amc, lorenz, Numeric.array ([60, 15, 1]), n=opts.n)

# Shuffle the sequence
initial = [float(x) for x in opts.ic.split(",")]
shuffled = sequence.shuffle(initial)

save(shuffled, args[1])

# vim: ts=4:sw=4:et
