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
import Numeric, sys

if len (sys.argv) != 3:
    print "usage: " + sys.argv[0] + " <input file> <output file>"
else:
    amc = AMC.from_file (sys.argv[1])
    lorenz = Systems.Lorenz (16.0, 45.0, 4.0)
    sequence = Sequence.Sequence (amc, lorenz, Numeric.array ([1, 2, 3]), n=1000)
    sequence.shuffle (Numeric.array ([1,2,3]), n=1000)

    sequence.save (sys.argv[2], amc.format)

# vim: ts=4:sw=4:et
