#!/usr/bin/env python
"""Utility for removing bones from an AMC file. Takes a list of bones to keep,
   and input file, and an output file.  The AMC file is written to the output
   file with only the bones listed on the command line.
   """
# Copyright (C) 2006 W. Evan Sheehan
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
import sys

if len(sys.argv) < 4:
    print "usage: %s [joint list] <input file> <output file>" % (sys.argv[0])
    print
    print "please include an input AMC file, output file name, and at least one joint"
    sys.exit (1)

amc = AMC.from_file (sys.argv[-2])
keepers = sys.argv[1:-2]
amputee = AMC ()

for bone in keepers:
    try:
        amputee.bones[bone] = amc.bones[bone]
    except KeyError:
        pass

amputee.save (sys.argv[-1])

# vim: ts=4:sw=4:et
