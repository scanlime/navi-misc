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

try:
    amc = AMC.from_file (sys.argv[1])
except IndexError:
    print "usage: " + sys.argv[0] + " <amc file>"
    raise

lorenz = Systems.Lorenz (1, 2, 3)
sequence = Sequence.Sequence (lorenz, Numeric.array ([4,5,6]), [0,1000,.01], amc.bones)
newSequence = sequence.shuffle (Numeric.array ([9,9,9]), [0,1000,.01])

bone = amc.bones.keys ()[0]

for frame in newSequence:
    assert (frame[bone] in amc.bones[bone])

# vim: ts=4:sw=4:et
