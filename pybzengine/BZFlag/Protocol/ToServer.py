""" BZFlag.Protocol.FromServer

Low-level python abstractions for the BZFlag protocol messages
sent from the game server to the clients.
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


class MsgExit(Common.Message):
    messageId = 0x6578

class MsgEnter(Common.Message):
    messageId = 0x656E
    entries = [
        StructEntry(Common.PlayerType, 'playerType'),
        StructEntry(Common.TeamColor,  'team'),
        StructEntry(StringField(32),   'callSign'),
        StructEntry(StringField(128),  'emailAddress'),
        ]

class MsgAlive(Common.Message):
    messageId = 0x616C
    entries = [
        StructEntry(Common.Vector3, 'position'),
        StructEntry(Common.Vector3, 'forward'),
        ]

class MsgLagPing(Common.Message):
    messageId = 0x7069
    entries = [
        StructEntry(UInt16, 'sequence'),
        ]

class MsgNetworkRelay(Common.Message):
    messageId = 0x6e72

class MsgGetWorld(Common.Message):
    messageId = 0x6777
    entries = [
        StructEntry(UInt32, 'offset'),
        ]

class MsgWantWHash(Common.Message):
    messageId = 0x7768

class MsgMessage(Common.Message):
    messageId = 0x6D67
    entries = [
        StructEntry(Common.PlayerId,  'destination'),
        StructEntry(StringField(128), 'message'),
        ]

class MsgKilled(Common.Message):
    messageId = 0x6b6c
    entries = [
        StructEntry(Common.PlayerId, 'killer'),
        StructEntry(UInt16,          'reason'),
        StructEntry(UInt16,          'shotId'),
        ]

class MsgGrabFlag(Common.Message):
    messageId = 0x6766
    entries = [
        StructEntry(UInt16, 'flagNum'),
        ]

class MsgDropFlag(Common.Message):
    messageId = 0x6466
    entries = [
        StructEntry(Common.Vector3, 'position'),
        ]

class MsgUDPLinkEstablished(Common.Message):
    messageId = 0x6f67

class MsgTeleport(Common.Message):
    messageId = 0x7470
    entries = [
        StructEntry(UInt16, 'fromId'),
        StructEntry(UInt16, 'toId'),
        ]

class MsgPause(Common.Message):
    messageId = 0x7061
    entries = [
        StructEntry(UInt8, 'paused'),
        ]


MsgUDPLinkRequest = Common.MsgUDPLinkRequest
MsgNegotiateFlags = Common.MsgNegotiateFlags
MsgShotBegin = Common.MsgShotBegin
MsgShotEnd = Common.MsgShotEnd
MsgPlayerUpdate = Common.MsgPlayerUpdate
MsgSetTTL = Common.MsgSetTTL
MsgNull = Common.MsgNull

### The End ###
