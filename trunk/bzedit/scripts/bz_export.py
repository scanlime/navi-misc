#!BPY
#
# Blender to BZFlag World exporter
# Micah Dowty <micah@navi.cx>
#

""" Registration info for Blender menus:
Name: 'BZFlag World'
Blender: 234
Group: 'Export'
Tip: 'Exports specially tagged Blender objects to a BZFlag world file'
"""

import Blender
import math

class BzWorldExporter:
    def __init__(self, scale=10):
        self.scale = scale

    def collectObjects(self):
        """Look through all Blender objects, finding those tagged with
           a 'bztype' property. This builds self.bzTypeMap, mapping bztypes
           to lists of objects.
           """
        self.bzTypeMap = {}
        for object in Blender.Object.Get():
            props = object.getAllProperties()
            for prop in props:
                if prop.getName() == "bztype":
                    self.bzTypeMap.setdefault(prop.getData(), []).append(object)

    def writeLine(self, *args):
        self.stream.write(" ".join([str(arg) for arg in args]) + "\n")

    def check(self):
        """Check the world for correctness. Returns True if correct,
           or False if incorrect. A popup reports errors to the user if
           they are found.
           """
        errors = []

        if 'world' not in self.bzTypeMap:
            errors.append("A 'world' object is required")
        elif len(self.bzTypeMap['world']) > 1:
            errors.append("Multiple 'world' objects are not allowed")

        if errors:
            Blender.Draw.PupMenu("Errors exporting BZFlag world:%t|" + "|".join(errors))
            return False
        else:
            return True

    def save(self, filename):
        """Write all collected bzflag objects to the given file"""
        self.stream = open(filename, "w")
        for type, objects in self.bzTypeMap.iteritems():
            handler = getattr(self, "write_" + type)
            for object in objects:
                handler(object)
                self.stream.write("\n")

    def writeCommon(self, object):
        self.writeLine("position",
                       object.LocX * self.scale,
                       object.LocY * self.scale,
                       object.LocZ * self.scale)
        self.writeLine("rotation", object.RotZ * 180 / math.pi)
        self.writeLine("size",
                       object.SizeX * self.scale,
                       object.SizeZ * self.scale,
                       object.SizeY * self.scale)

    def write_box(self, object):
        self.writeLine("box")
        self.writeCommon(object)
        self.writeLine("end")

    def write_world(self, object):
        self.writeLine("world")
        self.writeLine("size", object.SizeX * self.scale)
        self.writeLine("end")

    def write_pyramid(self, object):
        self.writeLine("pyramid")
        self.writeCommon(object)
        self.writeLine("end")

exp = BzWorldExporter()
exp.collectObjects()
if exp.check():
    Blender.Window.FileSelector(exp.save, "Save BZFlag World")
