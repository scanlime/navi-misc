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

# Default BZFlag port
defaultPort = 5155

# An IP address, as defined by struct in_addr in netinet/in.h
InAddr = VectorType("!BBBB")

# A vector in 3D space
Vector3 = VectorType("!fff")

# A player identifier is now a single unsigned byte, with several special meanings.
PlayerId = Enum(UInt8, {
    255: None,
    254: 'all',
    253: 'server',
    244: 'lastRealPlayer',
    })

TeamColor = Enum(UInt16, {
    -1: None,
    0: 'rogue',
    1: 'red',
    2: 'green',
    3: 'blue',
    4: 'purple',
    5: 'rabbit',
    })

PlayerType = Enum(UInt16, ['tank', 'JAFO', 'computer'])

PlayerStatus = Bitfield(UInt16, {
    'dead':         0x0000,
    'alive':        0x0001,
    'paused':       0x0002,
    'exploding':    0x0004,
    'teleporting':  0x0008,
    'flagActive':   0x0010,
    'crossingWall': 0x0020,
    'falling':      0x0040,
    })

RejectionCode = Enum(UInt16, [
    'badRequest', 'badTeam', 'badType', 'noRogues', 'teamFull', 'serverFull',
    ])

GameStyle = Bitfield(UInt16, {
    'teamFlag':    0x0001,
    'superFlag':   0x0002,
    'rogues':      0x0004,
    'jumping':     0x0008,
    'inertia':     0x0010,
    'ricochet':    0x0020,
    'shakable':    0x0040,
    'antidote':    0x0080,
    'timeSync':    0x0100,
    'rabbitChase': 0x0200,
    })

FlagId = Enum(UInt16, [
    # Team flags
    None, 'rogue', 'red', 'green', 'blue', 'purple',
    # Good flags
    'velocity', 'quickTurn', 'oscOverthruster', 'rapidFire', 'machineGun',
    'guidedMissle', 'laser', 'ricochet', 'superBullet', 'invisibleBullet',
    'stealth', 'tiny', 'narrow', 'shield', 'steamroller', 'shockWave',
    'phantomZone', 'genocide', 'jumping', 'identify', 'cloaking',
    # Bad flags
    'colorBlindness', 'obesity', 'leftTurnOnly', 'rightTurnOnly',
    'momentum', 'blindness', 'jamming', 'wideAngle',
    ])

FlagStatus = Enum(UInt16, [
    'noExist', 'onGround', 'onTank', 'inAir', 'coming', 'going',
    ])

FlagType = Enum(UInt16, [
    'normal', 'unstable', 'sticky',
    ])


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
        StructEntry(GameStyle, 'gameStyle'),
        StructEntry(UInt16,    'maxPlayers'),
        StructEntry(UInt16,    'maxShots'),
        StructEntry(UInt16,    'rogueCount'),
        StructEntry(UInt16,    'redCount'),
        StructEntry(UInt16,    'greenCount'),
        StructEntry(UInt16,    'blueCount'),
        StructEntry(UInt16,    'purpleCount'),
        StructEntry(UInt16,    'rogueMax'),
        StructEntry(UInt16,    'redMax'),
        StructEntry(UInt16,    'greenMax'),
        StructEntry(UInt16,    'blueMax'),
        StructEntry(UInt16,    'purpleMax'),
        StructEntry(UInt16,    'shakeWins'),
        StructEntry(UInt16,    'shakeTimeout'),   # 1/10ths of a second
        StructEntry(UInt16,    'maxPlayerScore'),
        StructEntry(UInt16,    'maxTeamScore'),
        StructEntry(UInt16,    'maxTime'),        # Seconds
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
        StructEntry(FlagId,     'id'),
        StructEntry(FlagStatus, 'status'),
        StructEntry(FlagType,   'type'),
        StructEntry(PlayerId,   'owner'),
        StructEntry(Vector3,    'position'),
        StructEntry(Vector3,    'launch'),
        StructEntry(Vector3,    'landing'),
        StructEntry(Float,      'flightTime'),
        StructEntry(Float,      'flightEndTime'),
        StructEntry(Float,      'initialVelocity'),
        ]


class MessageHeader(Struct):
    entries = [
        StructEntry(UInt16,   'length'),
        StructEntry(UInt16,   'id'),
        ]


class Message(Struct):
    """Subclass of Struct that includes a message Id and length in its marshalled form."""
    headerClass = MessageHeader

    def __init__(self, packed=None):
        self.header = self.headerClass()
        Struct.__init__(self, packed)

    def unmarshall(self, packed):
        packed = self.header.unmarshall(packed)
        return Struct.unmarshall(self, packed)

    def marshall(self):
        self.header.length = Struct.getSize(self)
        self.header.id = self.messageId
        return self.header.marshall() + Struct.marshall(self)

    def getSize(self, packed=None):
        return self.header.getSize() + Struct.getSize(self)


class MsgPlayerUpdate(Message):
    """This is referenced by ToServer and FromServer, since it
       is exactly the same going either direction.
       """
    messageId = 0x7075
    entries = [
        StructEntry(PlayerId,     'id'),
        StructEntry(PlayerStatus, 'status'),
        StructEntry(Vector3,      'position'),
        StructEntry(Vector3,      'velocity'),
        StructEntry(Float,        'azimuth'),
        StructEntry(Float,        'angularVelocity'),
        ]

    
def getMessageDict(module):
    """Return a dictionary mapping message IDs to message classes,
       given a module containing Message subclasses.
       """
    if not hasattr(module, 'messageDict'):
        module.messageDict = {}
        for key in module.__dict__:
            try:
                value = module.__dict__[key]
                if issubclass(value, Message):
                    module.messageDict[value.messageId] = value
            except TypeError:
                pass
    return module.messageDict

### The End ###
        
    
