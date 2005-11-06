#!/usr/bin/env python
#
# shuffle.py - Read in an AMC file, do the chaotic shuffle, write out an AMC
#              file.
#
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

from Motion import AMC
from Dance import Systems, Sequence
from optparse import OptionParser
import Numeric, string

parser = OptionParser ("usage: %prog <comma separated ics> <input file> <output file>")

opts, args = parser.parse_args ()

if len (args) != 3: parser.error ("input and output files, and initial conditions are required")

amc = AMC.from_file (args[1])
lorenz = Systems.Lorenz (16.0, 45.0, 4.0)
sequence = Sequence.Sequence (amc, lorenz, Numeric.array ([60, 15, 1]), n=10000)
sequence.shuffle (Numeric.array ([float(x) for x in string.split (args[0])]), n=10000)

sequence.save (args[2], amc.format)

# vim: ts=4:sw=4:et
