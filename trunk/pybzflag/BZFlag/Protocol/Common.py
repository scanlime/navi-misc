""" BZFlag.Protocol.Common

Using the marshalling utilities defined in BZFlag.Protocol,
define wrappers around common BZFlag structure that aren't
specific to packets to and from the BZFlag server.
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


# An IP address, as defined by struct in_addr in netinet/in.h
InAddr = VectorType("!BBBB")

# A vector in 3D space
Vector3 = VectorType("fff")

# A player identifier is now a single unsigned byte (changed from 1.7)
PlayerId = UInt8


class ServerId(Struct):
    """class ServerId from Address.h"""
    entries = [
        StructEntry(InAddr, 'serverHost'),      # Server host
        StructEntry(Int16,  'port'),            # Server port
        StructEntry(Int16,  'number'),          # Local player number
        ]

class Address(Struct):
    """class Address from Address.h"""
    entries = [
        StructEntry(InAddr, 'addr'),
        ]

class GameInfo(Struct):
    """The game information contained within a PingPacket"""
    entries = [
        StructEntry(UInt16, 'gameStyle'),
        StructEntry(UInt16, 'maxPlayers'),
        StructEntry(UInt16, 'maxShots'),
        StructEntry(UInt16, 'rogueCount'),
        StructEntry(UInt16, 'redCount'),
        StructEntry(UInt16, 'greenCount'),
        StructEntry(UInt16, 'blueCount'),
        StructEntry(UInt16, 'purpleCount'),
        StructEntry(UInt16, 'rogueMax'),
        StructEntry(UInt16, 'redMax'),
        StructEntry(UInt16, 'greenMax'),
        StructEntry(UInt16, 'blueMax'),
        StructEntry(UInt16, 'purpleMax'),
        StructEntry(UInt16, 'shakeWins'),
        StructEntry(UInt16, 'shakeTimeout'),   # 1/10ths of a second
        StructEntry(UInt16, 'maxPlayerScore'),
        StructEntry(UInt16, 'maxTeamScore'),
        StructEntry(UInt16, 'maxTime'),        # Seconds
        ]

class PingPacket(Struct):
    """class PingPacket from Ping.h"""
    entries = [
        StructEntry(SubStruct(ServerId), 'serverId'),
        StructEntry(SubStruct(Address),  'sourceAddr'),
        StructEntry(SubStruct(GameInfo), 'gameInfo'),
        ]

class FlagStatus(Struct):
    entries = [
        StructEntry(UInt16,   'flagId'),
        StructEntry(UInt16,   'status'),
        StructEntry(UInt16,   'type'),
        StructEntry(PlayerId, 'owner'),
        StructEntry(Vector3,  'position'),
        StructEntry(Vector3,  'launch'),
        StructEntry(Vector3,  'landing'),
        StructEntry(Float,    'flightTime'),
        StructEntry(Float,    'flightEndTime'),
        StructEntry(Float,    'initialVelocity'),
        ]

    
### The End ###
        
    
