""" BZFlag.Protocol.BinaryWorld

Specifies the binary encoding for BZFlag worlds, as sent via
MsgGetWorld from client to server. The Game.World class uses
these structures to implement its loadBinary() member.
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

from BZFlag.Protocol import *
from BZFlag.Protocol import Common
from BZFlag import Errors


class BlockHeader(Struct):
    entries = [
        StructEntry(UInt16,   'id'),
        ]


class Block(Common.Message):
    """The world binary is made up of a header followed by a number of blocks.
       This is like a Message, but without the length field.
       """
    headerClass = BlockHeader
    messageId = None


class Style(Block):
    messageId = 0x7374
    entries = [
        # This first field is the size, but since this is the only
        # block that has it, we'll just treat it as a magical constant.
        ConstStructEntry(UInt16, 28,  Errors.ProtocolError("Bad size in world style block")),
        StructEntry(Float,            'worldSize'),
        StructEntry(Common.GameStyle, 'gameStyle'),
        StructEntry(UInt16,           'players'),
        StructEntry(UInt16,           'shots'),
        StructEntry(UInt16,           'flags'),
        StructEntry(Float,            'linearAcceleration'),
        StructEntry(Float,            'angularAcceleration'),
        StructEntry(UInt16,           'shakeTime'),
        StructEntry(UInt16,           'shakeWins'),
        StructEntry(UInt32,           'serverTime'),
        ]




### The End ###
        
    
