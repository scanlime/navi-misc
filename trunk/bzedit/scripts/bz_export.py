#!BPY
#
# bz_export.py - Exports all specially tagged Blender objects
#                to a BZFlag world file.
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
Group: 'Export'
Tip: 'Exports specially tagged Blender objects to a BZFlag world file'
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


class BzWorldExporter:
    def collectObjects(self):
        """Look through all Blender objects, finding those tagged with
           a 'bztype' property. This looks for exactly one 'world' object,
           storing it in self.world. All bzflag objects are added to
           self.objects.
           """
        self.world = None
        self.objects = []
        typeReg = bzflag.getTypeRegistry()

        for object in Blender.Object.Get():
            try:
                bztype = object.getProperty("bztype")
            except AttributeError:
                pass
            else:
                # Create a BZObject using the TypeRegistry
                bzo = typeReg.fromBlender(object)
                self.objects.append(bzo)

                if isinstance(bzo, bzflag.World):
                    if self.world:
                        bzflag.log.err("Multiple 'world' objects are not allowed")
                        return
                    else:
                        self.world = bzo

        if not self.world:
            bzflag.log.err("A 'world' object is required")

    def save(self, filename):
        """Write all collected bzflag objects to the given file"""
        writer = bzflag.WorldWriter(filename)
        for object in self.objects:
            writer.writeObject(object)

def main():
    global exporter
    exporter = BzWorldExporter()
    exporter.collectObjects()
    if bzflag.log.numErrors:
        bzflag.log.report("Errors in collecting objects")
        return
    Blender.Window.FileSelector(fileSelectedCallback, "Save BZFlag World")

def fileSelectedCallback(filename):
    global exporter
    exporter.save(filename)
    if bzflag.log.numErrors:
        bzflag.log.report("Errors in saving world file")

if bzflag:
    main()

### The End ###
