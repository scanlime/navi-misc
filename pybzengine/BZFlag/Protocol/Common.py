""" BZFlag.Protocol.Common

Using the marshalling utilities defined in BZFlag.Protocol,
define wrappers around common BZFlag structure that aren't
specific to packets to and from the BZFlag server.
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
from BZFlag import Util
import math


# Default BZFlag port
defaultPort = 5155

# An IP address, as defined by struct in_addr in netinet/in.h
InAddr = VectorType("!BBBB")

Vector2 = VectorType("!ff")
Vector3 = VectorType("!fff")

# This is a Vector3 packed into a Vector2 by assuming X is always zero
VectorYZ = MappedEntry(Vector2,
                       lambda v2: (0.0, v2[0], v2[1]),
                       lambda v3: (v3[1], v3[2]))

# This is a Vector3 packed into a Vector2 by assuming Z is always zero
VectorXY = MappedEntry(Vector2,
                       lambda v2: (v2[0], v2[1], 0.0),
                       lambda v3: (v3[0], v3[1]))

# The protocol stores values in radians, but we try to use degrees everywhere here
Radians = MappedEntry(Float,
                      lambda radians: radians * 180 / math.pi,
                      lambda degrees: degrees * math.pi / 180)


PlayerId = Enum(UInt8, {
    255: None,
    254: 'all',
    253: 'server',
    244: 'lastRealPlayer',
    # All other values are client IDs
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

PlayerType = Enum(UInt16, ['tank', 'observer', 'computer'])

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

# Flag IDs are simply the abbreviation, padded to 16 bits
FlagId = StringField(2)

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
        StructEntry(UInt16,    'players'),
        StructEntry(UInt16,    'shots'),
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
        StructEntry(UInt16,    'shakeTime'),      # 1/10ths of a second
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


class FlagUpdate(Struct):
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

    def __init__(self, packed=None, **kw):
        self.header = self.headerClass()
        Struct.__init__(self, packed)
        self.__dict__.update(kw)

    def unmarshall(self, packed):
        packed = self.header.unmarshall(packed)
        return Struct.unmarshall(self, packed)

    def marshall(self):
        self.header.length = self.getSize() - self.header.getSize()
        self.header.id = self.messageId
        return self.header.marshall() + Struct.marshall(self)

    def getSize(self, packed=None):
        return self.header.getSize() + Struct.getSize(self)

    def dump(self, f, showContents=True, keyColumnWidth=25):
        """Dump a message in human readable form to the stream 'f'."""
        from BZFlag import Protocol
        from StringIO import StringIO

        name = self.__class__.__name__

        # If the message was received with Network.Socket.readMessage,
        # we can determine where it came from by its fromModule attribute
        direction = ''
        try:
            from BZFlag.Protocol import FromServer, ToServer
            if self.fromModule == ToServer:
                direction = "-->"
            elif self.fromModule == FromServer:
                direction = "<--"
        except AttributeError:
            pass

        f.write("%s %s\n" % (direction, name))
        if not showContents:
            return

        # Attributes for us to ignore. These are generally things we annotate the message
        # with later. Note that this isn't a list of all such annotations- most notably,
        # 'protocol' is omitted since it's very useful to have in message dumps.
        ignoreList = ('eventLoop', 'socket', 'header', 'fromModule', 'fromAddress', 'client')

        # Recursively build a list of (key,value) tuples that will be displayed
        # to represent a message. This handles traversing into substructures
        # like FlagUpdate.
        def buildKeys(object, prefix=""):
            keys = object.__dict__.keys()
            keys.sort()
            lst = []
            for key in keys:
                if key[0] != '_' and not key in ignoreList:
                    value = object.__dict__[key]
                    if isinstance(value, Protocol.Struct):
                        lst.extend(buildKeys(value, prefix + key + "."))
                    else:
                        lst.append((prefix + key, value))
            return lst

        for (key, value) in buildKeys(self):
            if key == 'data':
                # Special decoding for 'data' members- do a hex dump
                d = StringIO()
                Util.hexDump(StringIO(value), d)
                value = ("%d bytes\n" % len(value)) + d.getvalue()
            else:
                # Let python decode everything else
                value = repr(value)

            # Handle printing multiline values properly
            lines = value.split("\n")
            print ("%%%ss: %%s" % keyColumnWidth) % (key, lines[0])
            for line in lines[1:]:
                if line:
                    f.write(" " * (keyColumnWidth + 2) + line + "\n")


class DataMessage(Message):
    """This is a type of message that includes variable-length data
       after all the defined entries.
       """
    def unmarshall(self, packed):
        self.data = Message.unmarshall(self, packed)

    def marshall(self):
        return Message.marshall(self) + self.data

    def getSize(self, packed=None):
        return Message.getSize(self) + len(self.data)


class MsgPlayerUpdate(Message):
    """This is referenced by ToServer and FromServer, since it
       is exactly the same going either direction.
       """
    messageId = 0x7075
    entries = [
        StructEntry(PlayerId,     'id'),
        StructEntry(UInt32,       'order'),
        StructEntry(PlayerStatus, 'status'),
        StructEntry(Vector3,      'position'),
        StructEntry(Vector3,      'velocity'),
        StructEntry(Radians,      'azimuth'),
        StructEntry(Radians,      'angularVelocity'),
        ]

class MsgNegotiateFlags(DataMessage):
    messageId = 0x6e66

class MsgShotBegin(Message):
    messageId = 0x7362
    entries = [
        StructEntry(PlayerId, 'playerId'),
        StructEntry(UInt16,   'shotId'),
        StructEntry(Vector3,  'position'),
        StructEntry(Vector3,  'velocity'),
        StructEntry(Float,    'time'),
        StructEntry(FlagId,   'flagId'),
        StructEntry(Float,    'lifetime'),
        ]

class MsgShotEnd(Message):
    messageId = 0x7365
    entries = [
        StructEntry(PlayerId, 'playerId'),
        StructEntry(UInt16,   'shotId'),
        StructEntry(UInt16,   'reason'),
        ]

class MsgSetTTL(Message):
    messageId = 0x7474
    entries = [
        StructEntry(UInt16, 'ttl'),
        ]

class MsgUDPLinkRequest(Message):
    messageId = 0x6f66
    entries = [
        StructEntry(UInt16, 'port'),
        ]


class FlagNegotiationID(Struct):
    entries = [
        StructEntry(FlagId,         'id'),
        StructEntry(StringField(2), 'abbreviation'),
        ]

class MsgNull(Message):
    messageId = 0x0000


def getMessageDict(module):
    """Return a dictionary mapping message IDs to message classes,
       given a module containing Message subclasses.
       """
    return Util.getSubclassDict(module, Message, 'messageId')

### The End ###
