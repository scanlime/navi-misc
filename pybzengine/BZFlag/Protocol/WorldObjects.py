""" BZFlag.Protocol.WorldObjects

Specification of behaviour and storage for all BZFlag world
objects. These are Structs that can be packed into binary
and back easily, plus they have methods for serializing to
ASCII map files, and methods for converting to 2D and 3D
representations.
"""
#
# Python BZFlag Package
# Copyright (C) 2003 Micah Dowty <micahjd@users.sourceforge.net>
#
#  This library is free software; you can redistribute it and/or
#  modify it under the terms of the GNU Lesser General Public
#  License as published by the Free Software Foundation; either
#  version 2.1 of the License, or (at your option) any later version.
#
#  This library is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
#  Lesser General Public License for more details.
#
#  You should have received a copy of the GNU Lesser General Public
#  License along with this library; if not, write to the Free Software
#  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
#

from BZFlag.Protocol import *
from BZFlag.Protocol import Common
from BZFlag import Errors
import re, math


class BlockHeader(Struct):
    entries = [
        StructEntry(UInt16, 'id'),
        ]


class Block(Common.Message):
    """The world binary is made up of a header followed by a number of blocks.
       This is like a Message, but without the length field.
       """
    headerClass = BlockHeader
    messageId = None


class WorldObject(Block):
    """Building on the Struct and Message functionality, this base
       class adds methods for serializing to text.
       """
    # The section name used in text mode. None indicates that
    # the object should not be saved in text mode.
    textName = None

    def __init__(self, packed=None, **kw):
        self.unknownMapKeywords = {}
        self.options = []
        Block.__init__(self, packed, **kw)
        self.drawables = None

    def textRead(self, lines):
        """This accepts a list of text lines, with comments and
           leading/trailing whitespace already stripped out. Note that
           lines starting with '#!' should have the '#!' stripped out and
           be treated like normal lines, since that prefix is used
           to hide new map editor properties from older bzfs code.
           """
        for line in lines:
            tokens = re.split("\s+", line)
            keyword = tokens[0].lower()
            args = tokens[1:]
            self.unknownMapKeywords = {}
            if keyword == 'position':
                self.center = map(float, args[:3])
            elif keyword == 'rotation':
                self.angle = float(args[0])
            elif keyword == 'size':
                self.size = map(float, args[:3])
            elif keyword == 'border':
                self.border = float(args[0])
            elif keyword == 'name':
                self.name = " ".join(args)
            elif keyword == 'from':
                self.fromSide = int(args[0])
            elif keyword == 'to':
                self.toSide = int(args[0])
            elif keyword == 'color':
                self.team = Common.TeamColor.dict[int(args[0])]
            elif keyword == 'drivethrough':
                self.options.append('driveThrough')
            elif keyword == 'shootthrough':
                self.options.append('shootThrough')
            elif keyword == 'flipz':
                self.options.append('flipZ')
            elif keyword == 'end':
                break
            else:
                self.unknownMapKeywords[keyword] = args

    def textWrite(self, f):
        """Write a text representation of this object to
           the given file-like object.
           """
        if not self.textName:
            return
        f.write("%s\n" % self.textName)
        if hasattr(self, 'name'):
            f.write("#!name %s\n" % self.name)
        if hasattr(self, 'center'):
            f.write("position %s\n" % " ".join(map(str, self.center)))
        if hasattr(self, 'size'):
            f.write("size %s\n" % " ".join(map(str, self.size)))
        if hasattr(self, 'border'):
            f.write("border %s\n" % self.border)
        if hasattr(self, 'angle'):
            f.write("rotation %s\n" % self.angle)
        if hasattr(self, 'fromSide'):
            f.write("from %s\n" % self.fromSide)
        if hasattr(self, 'toSide'):
            f.write("to %s\n" % self.toSide)
        if hasattr(self, 'team'):
            f.write("color %s\n" % Common.TeamColor.reverseDict(self.team))
        if 'driveThrough' in self.options:
            f.write("drivethrough\n")
        if 'shootThrough' in self.options:
            f.write("shootthrough\n")
        if 'flipZ' in self.options:
            f.write("flipz\n")
        f.write("end\n\n")

    def toUntransformedPolygon(self):
        # Create four vertices from our extents
        return ((-self.size[0],
                 -self.size[1]),
                ( self.size[0],
                  -self.size[1]),
                ( self.size[0],
                  self.size[1]),
                (-self.size[0],
                  self.size[1]))

    def toPolygon(self):
        """Return a 2D polygon representation of this object, as a tuple of tuples."""
        poly = self.toUntransformedPolygon()

        # Rotate the polygon by our angle
        radians = -self.angle * math.pi / 180
        cos = math.cos(radians)
        sin = math.sin(radians)
        def rotate(point):
            return ( cos*point[0] + sin*point[1],
                    -sin*point[0] + cos*point[1])
        poly = map(rotate, poly)

        # Translate it to our center location
        def translate(point):
            return (point[0] + self.center[0],
                    point[1] + self.center[1])
        return map(translate, poly)

    def getDrawables(self):
        if self.drawables is None:
            return []
        else:
            return self.drawables


class Style(WorldObject):
    messageId = 0x7374
    entries = [
        # This first field is the size, but since this is the only
        # block that has it, we'll just treat it as a magical constant.
        ConstStructEntry(UInt16, 30,  Errors.ProtocolError("Bad size in world style block")),
        ConstStructEntry(UInt16, 1,   Errors.ProtocolError("Bad version in world size block")),
        StructEntry(Float,            'worldSize',            800.0),
        StructEntry(Common.GameStyle, 'gameStyle',            []),
        StructEntry(UInt16,           'players',              20),
        StructEntry(UInt16,           'shots',                1),
        StructEntry(UInt16,           'flags',                0),
        StructEntry(Float,            'linearAcceleration',   0.0),
        StructEntry(Float,            'angularAcceleration',  0.0),
        StructEntry(UInt16,           'shakeTime',            16138),
        StructEntry(UInt16,           'shakeWins',            22155),
        StructEntry(UInt32,           'serverTime',           0),
        ]

    def getDrawables(self):
        from BZFlag.UI import Drawable
        if self.drawables == None:
            self.drawables = [Drawable.Ground(self.worldSize)]
        return self.drawables


ObjectOptions = Bitfield(UInt8, {
    'driveThrough':  0x01,
    'shootThrough':  0x02,
    'flipZ':         0x04,
    })


class TeamBase(WorldObject):
    textName = 'base'
    messageId = 0x6261
    entries = [
        StructEntry(Common.TeamColor, 'team'),
        StructEntry(Common.Vector3,   'center', [0,0,0]),
        StructEntry(Common.Radians,   'angle',  0),
        StructEntry(Common.VectorXY,  'size',   [1,1,0]),
        StructEntry(Common.Vector3,   'safety', [0,0,0]),
        ]

    def getDrawables(self):
        from BZFlag.UI import Drawable
        if self.drawables == None:
            self.drawables = Drawable.detectBaseDrawables(self)
        return self.drawables


class Wall(WorldObject):
    messageId = 0x776C
    entries = [
        StructEntry(Common.Vector3,   'center', [0,0,0]),
        StructEntry(Common.Radians,   'angle',  0),
        StructEntry(Common.VectorYZ,  'size',   [0,1,1]),
        ]

    def getDrawables(self):
        from BZFlag.UI import Drawable
	if self.drawables == None:
	    self.drawables = [Drawable.WallSides(self),
                              Drawable.WallTop(self)]
        return self.drawables


class Box(WorldObject):
    textName = 'box'
    messageId = 0x6278
    entries = [
        StructEntry(Common.Vector3,   'center',       [0,0,0]),
        StructEntry(Common.Radians,   'angle',        0),
        StructEntry(Common.Vector3,   'size',         [1,1,1]),
        StructEntry(ObjectOptions,    'options',      []),
        ]

    def getDrawables(self):
        from BZFlag.UI import Drawable
        if self.drawables == None:
            self.drawables = Drawable.detectBoxDrawables(self)
        return self.drawables


class Pyramid(WorldObject):
    textName = 'pyramid'
    messageId = 0x7079
    entries = [
        StructEntry(Common.Vector3,   'center',       [0,0,0]),
        StructEntry(Common.Radians,   'angle',        0),
        StructEntry(Common.Vector3,   'size',         [1,1,1]),
        StructEntry(ObjectOptions,    'options',      []),
        ]

    def getDrawables(self):
        from BZFlag.UI import Drawable
        if self.drawables == None:
            self.drawables = Drawable.detectPyramidDrawables(self)
        return self.drawables


class Teleporter(WorldObject):
    textName = 'teleporter'
    messageId = 0x7465
    entries = [
        StructEntry(Common.Vector3,   'center',       [0,0,0]),
        StructEntry(Common.Radians,   'angle',        0),
        StructEntry(Common.Vector3,   'size',         [1,1,1]),
        StructEntry(Float,            'border',       1.0),
        StructEntry(ObjectOptions,    'options',      []),
        ]

    def getDrawables(self):
        from BZFlag.UI import Drawable
        if self.drawables == None:
            self.drawables = [
                Drawable.TeleporterField(self.center, self.angle, self.size),
                Drawable.TeleporterBorder(self.center, self.angle, self.size, self.border),
                ]
        return self.drawables


class TeleporterLink(WorldObject):
    textName = 'link'
    messageId = 0x6C6E
    entries = [
        StructEntry(UInt16, 'fromSide'),
        StructEntry(UInt16, 'toSide'),
        ]


class EndOfData(Block):
    messageId = 0x6564

### The End ###
