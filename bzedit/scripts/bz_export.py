#!BPY
#
# bz_export.py - Exports all specially tagged Blender objects
#                to a BZFlag world file.
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
Group: 'Export'
Tip: 'Exports specially tagged Blender objects to a BZFlag world file'
"""

import Blender
import BZFlag
import math

def collectObjects():
    """Look through all Blender objects, finding those tagged with
       a 'bztype' property. This looks for exactly one 'world' object,
       and assigns that world to all objects. Returns a list of prepared
       BZObject instances.
       """
    world = None
    blendObjects = []

    scene = Blender.Scene.GetCurrent()
    for object in scene.getChildren():
        try:
            bztype = object.getProperty("bztype").getData()
        except AttributeError:
            pass
        else:
            if bztype == 'world':
                # Create the world object first, and save it
                if world:
                    BZFlag.log.err("Multiple 'world' objects are not allowed")
                    return
                else:
                    world = BZFlag.fromBlender(object)
            else:
                # Store the blender object for now
                blendObjects.append(object)

    if not world:
        BZFlag.log.err("A 'world' object is required")

    # Create the BZObjects only after we have established our world object
    objects = [world]
    for object in blendObjects:
        bzo = BZFlag.fromBlender(object, world)
        if bzo:
            objects.append(bzo)
    return objects

def saveObjects(objects, filename):
    """Write all collected bzflag objects to the given file"""
    writer = BZFlag.Writer(filename)
    for object in objects:
        writer.writeObject(object)

def main():
    global objects
    objects = collectObjects()
    if BZFlag.log.numErrors:
        BZFlag.log.report("Errors in collecting objects")
        return
    Blender.Window.FileSelector(fileSelectedCallback, "Save BZFlag World")

def fileSelectedCallback(filename):
    global objects
    saveObjects(objects, filename)
    if BZFlag.log.numErrors:
        BZFlag.log.report("Errors in saving world file")

if BZFlag:
    main()

### The End ###
