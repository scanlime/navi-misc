#!BPY
#
# amc_import.py - Import an Acclaim Motion Capture file and apply NLA
#                 transformations to a matching armature
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
Name: 'AMC Motion Capture (.amc)...'
Blender: 236
Group: 'Import'
Tip: 'Imports an Acclaim AMC file and applies motion capture data to an armature'
"""

import Blender
import AMCReader

def fileSelectedCallback(filename):
    reader = AMCReader.AMCReader()
    try:
        reader.parse(filename)
    except IOError, s:
        AMCReader.log.err(s)
    else
        importData(reader)

    if AMCReader.log.numErrors:
        AMCReader.log.report('Errors in loading AMC file')

if AMCReader:
    Blender.Window.FileSelector(fileSelectedCallback, 'Load AMC Motion Capture')
