""" BZFlag.World.World

Abstraction for BZFlag's world format. This class can load and
save worlds to text and binary formats, storing them in a Scene class.
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

from BZFlag.Protocol import WorldObjects
from BZFlag import Event, Errors
import os, math, md5, sys, BZFlag
from StringIO import StringIO
from BZFlag.World import Scale


class TeleporterSide:
    """Represent one side of a teleporter. This is used to link teleporters together"""
    def __init__(self, teleporter, side):
        self.teleporter = teleporter
        self.side = side

    def link(self, to):
        if not hasattr(self.teleporter, 'links'):
            self.teleporter.links = [None, None]
        self.teleporter.links[self.side] = to


class World:
    """Abstraction for a BZFlag world. Currently this can only be created
       from binary worlds downloaded from the server, but eventually this
       will need to be able to read textual bzflag world files as well.

       The data structure used to store the world's contents in a form efficient
       for in-game use is separated into a Scene class. You can tell this class
       which Scene implementation to use in the constructor. By default it uses
       a flat list, but if this is being used for a 3D client it might be better
       to implement an Octree.
       """
    def __init__(self):
        Event.attach(self, 'onLoad', 'onAddObject', 'onRemoveObject', 'onChangeObject')
        self.erase()

    def saveBinary(self, f):
        """Save a binary world to the supplied file-like object"""
        for block in self.blocks:
            try:
                f.write(str(block))
            except:
                raise Errors.ProtocolError("Error saving block %r to binary: %s" % (block, sys.exc_info()[1]))

    def saveText(self, f):
        """Save a text world to the supplied file-like object"""
        for block in self.blocks:
            if isinstance(block, WorldObjects.WorldObject):
                block.textWrite(f)

    def erase(self):
        """Reset all internal structures, prepare to load a world"""
        self.blocks = []
        self.teleporters = []
        self.teleporterLinks = []
        self.style = None
        self.lifetime = 'permanent'     # For client-side world caching

    def clone(self, src):
        """Shallow-copy the contents from another Scene, leaving event handlers and such alone.
           Triggers the onLoad event.
           """
        self.blocks = src.blocks
        self.teleporters = src.teleporters
        self.teleporterLinks = src.teleporterLinks
        self.style = src.style
        self.lifetime = src.lifetime
        self.onLoad()

    def storeSkeletonHeader(self, size=Scale.WorldSize, wallHeight=Scale.WallHeight):
        """Adds required objects to the world that are only present in the binary format"""
        self.storeBlock(WorldObjects.Style(
            worldSize = size
            ))
        self.storeBlock(WorldObjects.Wall(
            center = [0, size/2, 0],
            angle  = 270,
            size   = [0, size/2, wallHeight]
            ))
        self.storeBlock(WorldObjects.Wall(
            center = [size/2, 0, 0],
            angle  = 180,
            size   = [0, size/2, wallHeight]
            ))
        self.storeBlock(WorldObjects.Wall(
            center = [0, -size/2, 0],
            angle  = 90,
            size   = [0, size/2, wallHeight]
            ))
        self.storeBlock(WorldObjects.Wall(
            center = [-size/2, 0, 0],
            angle  = 0,
            size   = [0, size/2, wallHeight]
            ))

    def storeSkeletonFooter(self):
        self.storeBlock(WorldObjects.EndOfData())

    def storeBlock(self, block):
        """Store one block class. This will be called while loading a world,
           between erase() and postprocess().
           """
        # File this block in the appropriate lists
        self.blocks.append(block)
        if isinstance(block, WorldObjects.Style):
            self.style = block
            self.size  = (block.worldSize, block.worldSize)
        if isinstance(block, WorldObjects.TeleporterLink):
            self.teleporterLinks.append(block)
        if isinstance(block, WorldObjects.Teleporter):
            self.teleporters.append(block)

    def postprocess(self):
        """Performs any checks or calculations necessary
           after a world has completed loading.
           Triggers the onLoad event.
           """
        # Link teleporters to each other.
        # Note that this creates circular links, so the world will not
        # be automatically garbage collected! This isn't a problem yet,
        # since only one world should be loaded.
        for link in self.teleporterLinks:
            fromSide = TeleporterSide(self.teleporters[link.fromSide >> 1], link.fromSide & 1)
            toSide = TeleporterSide(self.teleporters[link.toSide >> 1], link.toSide & 1)
            fromSide.link(toSide)
        self.onLoad()

    def getHash(self):
        """Return an MD5 hash of the binary world"""
        f = StringIO()
        self.saveBinary(f)
        m = md5.new()
        m.update(f.getvalue())
        return m.hexdigest()


class Cache:
    """Cache worlds on disk according to a server-generated hash,
       so we don't always have to send them over the wire.
       """
    def __init__(self, path=BZFlag.cachePath):
        self.path = os.path.expanduser(path)
        try:
            os.makedirs(self.path)
        except OSError:
            # Path could already exist or it could be unreachable. We don't care.
            pass

    def getFilename(self, hash):
        return os.path.join(self.path, "p" + hash + ".bwc")

    def hasWorld(self, hash):
        try:
            return os.path.isfile(self.getFilename(hash))
        except OSError:
            return 0

    def storeWorld(self, hash, data):
        try:
            f = open(self.getFilename(hash), "wb")
            f.write(data)
            f.close()
        except OSError:
            # In case the file was corrupted (if the disk was full, for example)
            # try to remove it before giving up completely.
            try:
                os.remove(self.getFilename(hash))
            except OSError:
                pass

    def openWorld(self, hash):
        # Don't catch OSError here. If the cache wasn't valid we
        # should have stopped after hasWorld() returned zero.
        # At this point, we're committed to using the cache.
        return open(self.getFilename(hash), "rb")


def load(name):
    """Load a world 'ingelligently' from the given name.
       See the Help() world generator or the '-w help'
       command line option for more information about this format.
       """
    from BZFlag.World import Generator
    import re

    # If it's not a string, we can't handle it here- assume it's a
    # file-like object or something else that Text() can handle.
    if type(name) != str:
        return Generator.Text(name)

    # See if it's a generator
    name = name.strip()
    nameLower = name.lower()
    nameBase = re.sub(r"[\(:].*", "", name).strip().lower()
    for gen in Generator.__all__:
        if nameBase == gen.lower():
            # Found a generator. If it's an exact (case insensitive) match,
            # run the generator with defaults and return that.
            if nameLower == gen.lower():
                return getattr(Generator, gen)()

            # If there is a semicolon after the generator name, this is the alternate format
            if nameLower.startswith(nameBase + ":"):
                argsIn = name[len(nameBase)+1:].split(",")
                argsOut = []
                # Automatically quote any args that don't parse as-is
                for arg in argsIn:
                    try:
                        if arg.find("=") > 0:
                            # This looks like a keyword argument,
                            # use only the argument part
                            eval(arg.split("=")[1], Generator.__dict__)
                        else:
                            eval(arg.split("=")[1], Generator.__dict__)
                    except:
                        arg = '"' + arg + '"'
                    argsOut.append(arg)
                return eval("%s(%s)" % (gen, ",".join(argsOut)),
                            Generator.__dict__)

            # Nope, we'll need to evaluate this as a python expression
            return eval(name, Generator.__dict__)

    # No? Try loading it as a text file
    return Generator.Text(name)

### The End ###
