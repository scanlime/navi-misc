#!BPY
#
# asf-amc_import.py - Import Acclaim ASF/AMC motion capture data, creating
#       an armature and keyframes for each pose
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

""" Registration info for Blender menus
Name: 'ASF+AMC motion capture...'
Blender: 236
Group: 'Import'
Tip: 'Imports Acclaim ASF+AMC and builds an armature with the MC data'
"""

import Blender
import ASFReader
import AMCReader
import math
import string

# make it faster?
try:
    import psyco
except:
    pass

def loadASF (filename):
    reader = ASFReader.ASFReader ()
    try:
        reader.parse (filename)
    except IOError, s:
        ASFReader.log.err (s)

    if ASFReader.log.numErrors:
        ASFReader.log.report ('Errors in loading ASF')
    else:
        Blender.Window.FileSelector (loadAMC, 'Load AMC Motion Capture')

def loadAMC (filename):
    pass

Blender.Window.FileSelector (loadASF, 'Load ASF Skeleton File')
