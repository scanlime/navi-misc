""" BZFlag.World

Implementation of BZFlag's world database. This includes loading
and saving worlds in binary and text formats.
"""
# 
# Python BZFlag Protocol Package
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

from BZFlag.Protocol import BinaryWorld, Common
from BZFlag import Errors
import os


class Octree:
    def add(self, block):
        pass


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
       """
    def __init__(self, sceneClass=Octree):
        self.sceneClass = sceneClass
        self.erase()
    
    def loadBinary(self, bin):
        """Load a binary world from the supplied file-like object"""
        blockDict = Common.getMessageDict(BinaryWorld)
        self.erase()
        while 1:
            # Read the block header
            header = BinaryWorld.BlockHeader()
            packedHeader = bin.read(header.getSize())
            if len(packedHeader) < header.getSize():
                raise Errors.ProtocolError("Premature end of binary world data")
            header.unmarshall(packedHeader)
            
            # Look up the block type and instantiate it
            try:
                block = blockDict[header.id]()
            except KeyError:
                raise Errors.ProtocolError(
                    "Unknown block type 0x%04X in binary world data" % header.id)
            if isinstance(block, BinaryWorld.EndOfData):
                break

            # Read the block body
            packedBody = bin.read(block.getSize() - len(packedHeader))
            if len(packedBody) < (block.getSize() - len(packedHeader)):
                raise Errors.ProtocolError("Incomplete block in binary world data")
            block.unmarshall(packedHeader + packedBody)
            self.storeBlock(block)
        self.postprocess()


    def erase(self):
        """Reset all internal structures, prepare to load a world"""
        self.blocks = []
        self.teleporters = []
        self.teleporterLinks = []
        self.scene = self.sceneClass()
        self.gameStyle = None

    def storeBlock(self, block):
        """Store one block class. This will be called while loading a world,
           between erase() and postprocess().
           """
        # File this block in the appropriate lists
        self.blocks.append(block)
        if isinstance(block, BinaryWorld.Style):
            self.gameStyle = block
        if hasattr(block, 'center'):
            self.scene.add(block)
        if isinstance(block, BinaryWorld.TeleporterLink):
            self.teleporterLinks.append(block)
        if isinstance(block, BinaryWorld.Teleporter):
            self.teleporters.append(block)

    def postprocess(self):
        """Performs any checks or calculations necessary
           after a world has completed loading.
           """
        # Link teleporters to each other.
        # Note that this creates circular links, so the world will not
        # be automatically garbage collected! This isn't a problem yet,
        # since only one world should be loaded.
        for link in self.teleporterLinks:
            fromSide = TeleporterSide(self.teleporters[link.fromSide >> 1], link.fromSide & 1)
            toSide = TeleporterSide(self.teleporters[link.toSide >> 1], link.toSide & 1)
            fromSide.link(toSide)
        

class Cache:
    """Cache worlds on disk according to a server-generated hash,
       so we don't always have to send them over the wire.
       """
    def __init__(self, path="~/.bzflag-cache"):
        self.path = os.path.expanduser(path)
        try:
            os.makedirs(self.path)
        except OSError:
            # Path could already exist or it could be unreachable. We don't care.
            pass

    def getFilename(self, hash):
        return os.path.join(self.path, hash) + ".bwc"

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

### The End ###
        
    
