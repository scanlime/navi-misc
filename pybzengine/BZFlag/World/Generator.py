""" BZFlag.World.Generator

Implements factory functions for automatically generating various worlds.
All generator objects in this model should be in __all__, and have help
strings properly formatted for the Help() generator.
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

from BZFlag.World.World import World
from BZFlag.World import Scale
from BZFlag.Protocol import WorldObjects
from BZFlag import Util, Errors

__all__ = ['Empty', 'Random', 'Text', 'Binary', 'Heightmap', 'Help']


def Empty(size=Scale.WorldSize,
          wallHeight=Scale.WallHeight,
          ):
    """
    Generate a world with no pyramids or blocks, only walls.
    
    Keyword arguments:
        size       : Distance along each side of the generated square world
        wallHeight : Height of the world's walls
    """
    w = World()
    w.erase()
    w.storeSkeletonHeader(size=size, wallHeight=wallHeight)
    w.storeSkeletonFooter()    
    w.postprocess()
    return w


def Random(size          = Scale.WorldSize,
           wallHeight    = Scale.WallHeight,
           randomHeights = False,
           pyrBase       = 4.0 * Scale.TankHeight,
           pyrHeight     = 5.0 * Scale.TankHeight,
           boxBase       = 5.0 * Scale.TankLength,
           boxHeight     = 6.0 * Scale.MuzzleHeight,
           citySize      = 5,
           ):
    """
    Generate a traditional random world. The defaults use the
    same random world algorithm as bzfs proper, but it can
    be tweaked to a much greater extent.

    Keyword arguments:
        size          : Distance along each side of the generated square world
        wallHeight    : Height of the world's walls
        randomHeights : Boolean, enable randomly varying object heights
        pyrBase       : Base size for pyramids
        pyrHeight     : Height for pyramids
        boxBase       : Base size for boxes
        boxHeight     : Height for boxes
        citySize      : Determines the number of total pyramids and blocks
    """
    from random import random
    import math

    w = World()
    w.erase()
    w.lifetime = 'temporary'
    w.storeSkeletonHeader(size=size, wallHeight=wallHeight)

    def addRandom(cls, height, base):
        h = height
        num = int((0.5 + 0.7 * random()) * citySize * citySize)
        for i in xrange(num):
            if randomHeights:
                h = height * (2.0 * (random() + 0.5))
            w.storeBlock(cls(
                center = [size * (random() - 0.5),
                          size * (random() - 0.5),
                          0],
                angle = 2.0 * math.pi * random(),
                size = [base, base, h]))
    
    addRandom(WorldObjects.Box, boxHeight, boxBase)
    addRandom(WorldObjects.Pyramid, pyrHeight, pyrBase)
    
    w.storeSkeletonFooter()    
    w.postprocess()
    return w


def Text(name,
         size       = Scale.WorldSize,
         wallHeight = Scale.WallHeight,
         ):
    """
    Load a world from a text file. This is the default world format.

    Positional arguments:
        name : File or URI containing the world

    Keyword arguments:
        size       : Default distance along each side of the generated square world
        wallHeight : Height of the world's walls
    """
    from xreadlines import xreadlines
    import re
    
    f = Util.autoFile(name)
    w = World()
    section = None
    sectionDict = Util.getSubclassDict(WorldObjects, WorldObjects.WorldObject,
                                       'textName', 'textSectionDict')

    # We won't actually add the objects to the world until later,
    # since we need to start the world out with walls and a game style
    # block, but we might get the information needed for those at any
    # point in the file.
    w.erase()
    blocks = []
        
    for line in xreadlines(f):
        # If this is a kludge used by map editors to store extra
        # attributes. Don't process any comments on the line.
        if not line.startswith("#!"):
            line = re.sub("#.*", "", line)
        line = line.strip()
        if line:
            if section:
                # We're inside a section. Accumulate lines until 'end'
                sectionLines.append(line)
                if line == 'end':
                    # Done with this section, process it.

                    if section == 'world':
                        # World information
                        for line in sectionLines:
                            tokens = re.split("\s+", line)
                            keyword = tokens[0].lower()
                            args = tokens[1:]
                            if keyword == 'size':
                                size = int(args[0])

                    else:
                        # Assume all other sections are world objects
                        try:
                            cls = sectionDict[section]
                        except KeyError:
                            raise Errors.ProtocolError(
                                "World file contains unknown section type '%s'" % section)
                        inst = cls()
                        inst.textRead(sectionLines)
                        blocks.append(inst)
                        
                    section = None
            elif not line.startswith("#!"):
                # We're beginning a section
                section = line.lower()
                if section.find(" ") >= 0:
                    raise Errors.ProtocolError("Unexpected whitespace within section name")
                sectionLines = []

    if section:
        raise Errors.ProtocolError("World file has unterminated '%s' section" % section)

    # Now store all the blocks in our world
    w.storeSkeletonHeader(size, wallHeight)
    for block in blocks:
        w.storeBlock(block)
    w.storeSkeletonFooter()
    w.postprocess()
    return w


def Binary(name):
    """
    Load a world from a binary file. This is the world format used in
    transit from server to client, and used in the client's world cache.

    Positional arguments:
        name : File or URI containing the binary world
    """
    from BZFlag.Protocol import Common
    
    blockDict = Common.getMessageDict(WorldObjects)
    f = Util.autoFile(name)
    w = World()
    w.erase()
    while 1:
        # Read the block header
        header = WorldObjects.BlockHeader()
        packedHeader = f.read(header.getSize())
        if len(packedHeader) < header.getSize():
            raise Errors.ProtocolError("Premature end of binary world data")
        header.unmarshall(packedHeader)

        # Look up the block type and instantiate it
        try:
            block = blockDict[header.id]()
        except KeyError:
            raise Errors.ProtocolError(
                "Unknown block type 0x%04X in binary world data" % header.id)

        # Read the block body
        packedBody = f.read(block.getSize() - len(packedHeader))
        if len(packedBody) < (block.getSize() - len(packedHeader)):
            raise Errors.ProtocolError("Incomplete block in binary world data")
        block.unmarshall(packedHeader + packedBody)
        w.storeBlock(block)

        # We're done if this was the EndOfData block
        if isinstance(block, WorldObjects.EndOfData):
            break
    w.postprocess()
    return w


def Heightmap(name,
              size          = Scale.WorldSize,
              wallHeight    = Scale.WallHeight,
              boxHeight     = 20.0 * Scale.MuzzleHeight,
              ):
    """
    Uses the given image file to make a heightmapped world.
    This is very silly and nobody should use this except
    as an example and a proof-of-concept.
    Requires PIL.

    Positional arguments:
        name : File or URI containing the heightmap, in any format PIL supports

    Keyword arguments:
        size       : Distance along each side of the generated square world
        wallHeight : Height of the world's walls
        boxHeight  : Maximum box height, corresponding to a fully white pixel 
    """
    import Image
    f = Util.autoFile(name, "rb")
    img = Image.open(f).convert("L")   # Convert to grayscale
    w = World()
    w.erase()
    w.storeSkeletonHeader(size=size, wallHeight=wallHeight)

    # For now just output a box per nonzero pixel. If I expected
    # anyone to actually use this I'd make it combine rectangular
    # regions of the same pixel value into larger boxes.
    for y in xrange(img.size[1]):
        for x in xrange(img.size[0]):
            p = img.getpixel((x,y))
            if p:
                w.storeBlock(WorldObjects.Box(
                    center = ( (float(x) / img.size[0] - 0.5) * size,
                              -(float(y) / img.size[1] - 0.5) * size,
                               0),
                    size   = (size / img.size[0],
                              size / img.size[1],
                              p / 255.0 * boxHeight),
                    angle  = 0))
    
    w.storeSkeletonFooter()    
    w.postprocess()
    f.close()
    return w


def Help():
    """
    Displays information about the world name format and
    usage for all world generators, then exits.
    """
    import sys
    print """-- World names

World names can take one of four forms:

  - A local filename
    Example:
       foo.bzw

  - A URI (http, ftp, file, or anything supported by urllib2)
    Example:
       http://example.com/bz/world.bzw

  - A python expression for a world generator.
    Examples:
       Random()
       Text("foo.bzw", size=1000)

  - A modified syntax for python expressions that doesn't
    require quoting on the command line. It consists of
    a non-case-sensitive function name, a colon, then a
    comma-separated list of arguments. Arguments that can't
    be parsed as-is will automatically be quoted.
    Examples:
       text:foo.bzw,size=1000
       random
       Random:citySize=10

Supported world generators:
"""
    for genName in __all__:
        gen = globals()[genName]
        print "  - %s" % genName
        print gen.__doc__[1:]
    sys.exit(0)

### The End ###
