#!/usr/bin/env python

"""Utility for removing bones from an AMC file.

usage: amputate.py [options] [joint list] <input file> <output file>
"""

# Copyright (C) 2006-2007 W. Evan Sheehan
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
from optparse import OptionParser

usage = "%prog [options] [joint list] <input file> <output file>"
parser = OptionParser(usage)
parser.add_option("-k", "--keep", action="store_true", default=False)

opts, args = parser.parse_args()

if len(args) < 3:
    parser.error("please include an input AMC file, output file name, and at least one joint")

amc = AMC.from_file(args[-2])
keepers = args[0:-2]
amputee = AMC ()

if opts.keep:
    for bone in keepers:
        try:
            amputee.bones[bone] = amc.bones[bone]
        except KeyError:
            pass
else:
    for bone, v in amc.bones.iteritems():
        if bone not in keepers:
            amputee.bones[bone] = v

amputee.save(args[-1])

# vim: ts=4:sw=4:et
