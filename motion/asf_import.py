#!BPY
#
# asf_import.py - Import an Acclaim Skeleton File and create a set
#                 of armatures matching the data
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
Name: 'ASF Skeleton File...'
Blender: 236
Group: 'Import'
Tip: 'Imports an Acclaim ASF file and builds a set of armatures'
"""

import Blender
import math

try:
    import ASFReader
except ImportError:
    Blender.Draw.PupMenu('Error importing "ASFReader" module%t|'
                         'You may have to start the blender game engine|'
                         'momentarily then exit it to set the Python|'
                         'path properly.')
    ASFReader = None

def importObjects(reader):
    pass

def fileSelectedCallback(filename):
    reader = ASFReader.ASFReader()
    try:
        reader.parse(filename)
    except IOError, s:
        ASFReader.log.err(s)
    else:
        importObjects(reader)

    if ASFReader.log.numErrors:
        ASFReader.log.report('Errors in loading ASF file')

if ASFReader:
    Blender.Window.FileSelector(fileSelectedCallback, 'Load ASF Skeleton File')
