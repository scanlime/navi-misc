#!BPY
#
# bz_import.py - Import an existing BZFlag world into a collection
#                of specially tagged Blender objects.
#
# Blender-based BZFlag World Editor
# Copyright (C) 2004 David Trowbridge and Micah Dowty
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

""" Registration info for Blender menus:
Name: 'BZFlag World...'
Blender: 234
Group: 'Import'
Tip: 'Imports a BZFlag world file into specially tagged Blender objects'
"""

import Blender
import math

try:
    import bzflag
except ImportError:
    Blender.Draw.PupMenu("Error importing 'bzflag' module%t|"
                         "You may have to start the Blender game engine|"
                         "momentarily then exit it to set the Python|"
                         "path properly.")
    bzflag = None

def fileSelectedCallback(filename):
    try:
        reader = bzflag.WorldReader(filename)
    except IOError, s:
        bzflag.log.err(s)
    else:
        for object in reader.readObjects():
            object.toBlender()
    if bzflag.log.numErrors:
        bzflag.log.report("Errors in loading world file")

if bzflag:
    Blender.Window.FileSelector(fileSelectedCallback, "Load BZFlag World")

### The End ###
