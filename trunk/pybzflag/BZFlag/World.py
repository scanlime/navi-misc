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

class World:
    """Abstraction for a BZFlag world. Currently this can only be created
       from binary worlds downloaded from the server, but eventually this
       will need to be able to read textual bzflag world files as well.
       """
    def loadBinary(self, bin):
        """Load a binary world from the supplied file-like object"""
        blockDict = Common.getMessageDict(BinaryWorld)
        blocks = []
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

            # Read the block body
            packedBody = bin.read(block.getSize() - len(packedHeader))
            if len(packedBody) < (block.getSize() - len(packedHeader)):
                raise Errors.ProtocolError("Incomplete block in binary world data")
            block.unmarshall(packedHeader + packedBody)
            blocks.append(block)
        print blocks

### The End ###
        
    
