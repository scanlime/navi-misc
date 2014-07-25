#!BPY
#
# bz_add.py - A simple script that makes new BZFlag objects
#             accessable via the "Add" menu.
#
# Blender-based BZFlag World Editor
# Copyright (C) 2004-2005 David Trowbridge and Micah Dowty
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
Name: 'BZFlag Object...'
Blender: 234
Group: 'Add'
Submenu: 'World' world
Submenu: 'Box' box
Submenu: 'Pyramid' pyramid
Submenu: 'Teleporter' teleporter
Tip: 'Add a new BZFlag object'
"""

import Blender

try:
    import bzflag
except ImportError:
    Blender.Draw.PupMenu("Error importing 'bzflag' module%t|"
                         "You may have to start the Blender game engine|"
                         "momentarily then exit it to set the Python|"
                         "path properly.")
    bzflag = None

if bzflag:
    # Create a new BZObject and convert that to a new Blender object
    bzo = bzflag.getTypeRegistry().dict[__script__.arg]()
    object = bzo.toBlender()

    # Put the new object at the cursor
    object.setLocation(*Blender.Window.GetCursorPos())

### The End ###
