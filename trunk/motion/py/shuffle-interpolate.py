#!/usr/bin/env python
#
# shuffle-interpolate.py - Read in an AMC file, do the chaotic shuffle,
#                          interpolate the breaks, and write out a new
#                          AMC file.
#
# Copyright (C) 2005 David Trowbridge
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

from Motion import AMC
from Dance import Systems, Sequence, MotionGraph
from Graph.ExtraAlgorithms import ParallelBFS
import Numeric, sys

try:
    amc = AMC.from_file (sys.argv[1])
except IndexError:
    print 'usage: %s <amc file' % sys.argv[0]
    raise

print 'shuffling sequence'
lorenz = Systems.Lorenz (16.0, 45.0, 4.0)
sequence = Sequence.Sequence (amc, lorenz, Numeric.array ([1, 2, 3]), n=5000)
sequence.shuffle (Numeric.array ([17.0, 2.0, -1.0]), n=5000)

graphs = {}
for key in amc.bones.iterkeys ():
    print 'creating graph for %s' % key
    g = MotionGraph.build_graphs (key, [amc.bones[key]])
    if g is not None:
        graphs[key] = g

for frame in sequence:
    for bone in amc.bones.keys ():
        assert (frame[bone] in amc.bones[bone])

search = ParallelBFS ()

