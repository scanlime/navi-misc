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

from Motion import AMC
from Dance import Systems, Sequence
from optparse import OptionParser
import Numeric, string

parser = OptionParser ("usage: %prog <input file> <output file>")
parser.add_option ("-i", "--initial", dest="ic", default="60,15,1", \
        help="A comma separated list of initial conditions for the shuffle")
parser.add_option ("-n", dest="n", type="int", default=10000, \
        help="Number of iterations for the chaotic systems")

opts, args = parser.parse_args ()

if len (args) != 2: parser.error ("input and output file are required")

amc = AMC.from_file (args[0])
lorenz = Systems.Lorenz (16.0, 45.0, 4.0)
sequence = Sequence.Sequence (amc, lorenz, Numeric.array ([60, 15, 1]), n=opts.n)
sequence.shuffle (Numeric.array ([float(x) for x in string.split (opts.ic, ",")]), n=opts.n)

sequence.save (args[1], amc.format)

# vim: ts=4:sw=4:et
