#
# Blender to BZFlag World exporter
# Micah Dowty <micah@navi.cx>
#

import Blender
import math

class BzWorldExporter:
    def __init__(self, filename, scale=10):
        self.stream = open(filename, "w")
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

    def save(self):
        """Write all collected bzflag objects to the current stream"""
        assert len(self.bzTypeMap['world']) <= 1
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


exp = BzWorldExporter("test.bzw")
exp.collectObjects()
exp.save()