#!BPY
#
# bz_import.py - Import an existing BZFlag world into a collection
#                of specially tagged Blender objects.
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
Name: 'BZFlag World (.bzw)...'
Blender: 234
Group: 'Import'
Tip: 'Imports a BZFlag world file into specially tagged Blender objects'
"""

import Blender
import math
import BZFlag
import psyco

def importObjects(reader):
    # First determine if we have a world. If not, we'll need to create a default one.
    world = None
    objects = []
    for object in reader.objects:
        if isinstance(object, BZFlag.World):
            world = object
        else:
            objects.append(object)
    if not world:
        world = BZFlag.World()

    # Create the Blender object for the world first
    world.toBlender()

    # Create Blender objects for each BZFlag object
    for object in objects:
        object.world = world
        object.toBlender()

    # Set all layers except 20 visible. Layer 20 is reserved for defined groups
    Blender.Window.ViewLayer([(i + 1) for i in range(19)])

def loadNormal(filename):
    try:
        reader = BZFlag.Reader()
        reader.parse(filename)
    except StandardError, e:
        print 'error!',e
        BZFlag.log.err(e)
    else:
        importObjects(reader)

        # Remove that pesky default cube if it's still around
        try:
            Blender.Scene.GetCurrent().unlink(Blender.Object.Get("Cube"))
        except AttributeError:
            pass

    if BZFlag.log.numErrors:
        BZFlag.log.report("Errors in loading world file")

def loadDebug(filename):
    reader = BZFlag.Reader()
    reader.parse(filename)
    importObjects(reader)
    try:
        Blender.Scene.GetCurrent().unlink(Blender.Object.Get("Cube"))
    except AttributeError:
        pass

def fileSelectedCallback(filename):
    loadDebug(filename)


if BZFlag:
    psyco.full()
    Blender.Window.FileSelector(fileSelectedCallback, "Load BZFlag World")

### The End ###
