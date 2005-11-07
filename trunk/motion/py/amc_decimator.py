#!/usr/bin/env python
"""amc_decimator.py

Read in an AMC file and every nth frame out to another AMC file.
"""

# Copyright (C) 2005 W. Evan Sheehan
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

from optparse import OptionParser
from Motion import AMC
import Numeric

parser = OptionParser ("usage: %prog [options] -n N <input file> <output file>")
parser.add_option ("-n", type="int", dest="step", help="Choose every Nth frame")

opts, args = parser.parse_args ()

if len (args) != 2: parser.error ("input and output fields required")
if opts.step == None: parser.error ("missing -n option")

# The original AMC file...
amc = AMC.from_file (args[0])
length = len (amc.bones.values()[0])

# The decimated AMC file...
decimated = AMC ()
decimated.format = amc.format

for bone,frames in amc.bones.iteritems ():
    slowed = []

    # Take every nth frame and duplicate it n times. This should create a
    # sequence from every nth frame that is the same length as the original
    # sequence.
    for i in range (0, length, opts.step):
        slowed.extend ([frames[i] for j in range (opts.step)])

    # Create the Numeric array
    decimated.bones[bone] = Numeric.array (slowed)

decimated.save (args[1])


# vim: ts=4:sw=4:et
