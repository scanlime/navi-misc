""" BZFlag.Protocol.FromServer

Low-level python abstractions for the BZFlag protocol messages
sent from the game server to the clients.
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


class HelloPacket(Struct):
    """This packet is sent from the server immediately upon connection, identifying
       the server version and providing the client with an Id number.
       """
    entries = [
        StructEntry(StringField(8),  'version'),
        StructEntry(Common.PlayerId, 'clientId'),
        ]

class MsgNull(Common.Message):
    messageId = 0x0000

class MsgAccept(Common.Message):
    messageId = 0x6163

class MsgSuperKill(Common.Message):
    messageId = 0x736B

class MsgReject(Common.Message):
    messageId = 0x726A
    entries = [
        StructEntry(Common.RejectionCode, 'reason'),
        ]

class MsgTeamUpdate(Common.Message):
    messageId = 0x7475
    entries = [
        StructEntry(Common.TeamColor, 'team'),
        StructEntry(UInt16,           'size'),
        StructEntry(UInt16,           'active'),
        StructEntry(UInt16,           'wins'),
        StructEntry(UInt16,           'losses'),
        ]

class MsgFlagUpdate(Common.Message):
    messageId = 0x6675
    entries = [
        StructEntry(UInt16,                       'flagNum'),
        StructEntry(SubStruct(Common.FlagStatus), 'status'),
        ]

class MsgAddPlayer(Common.Message):
    messageId = 0x6170
    entries = [
        StructEntry(Common.PlayerId,   'id'),
        StructEntry(Common.PlayerType, 'type'),
        StructEntry(Common.TeamColor,  'team'),
        StructEntry(UInt16,            'wins'),
        StructEntry(UInt16,            'losses'),
        StructEntry(UInt16,            'teamKills'),
        StructEntry(StringField(32),   'callSign'),
        StructEntry(StringField(128),  'emailAddress'),
        ]

class MsgMessage(Common.Message):
    messageId = 0x6D67
    entries = [
        StructEntry(Common.PlayerId,  'fromId'),
        StructEntry(Common.PlayerId,  'toId'),
        StructEntry(StringField(128), 'message'),
        ]

class MsgLagPing(Common.Message):
    messageId = 0x7069
    entries = [
        StructEntry(UInt16, 'sequence'),
        ]

class MsgNewRabbit(Common.Message):
    messageId = 0x6e52
    entries = [
        StructEntry(Common.PlayerId, 'id'),
        ]

class MsgRemovePlayer(Common.Message):
    messageId = 0x7270
    entries = [
        StructEntry(Common.PlayerId, 'id'),
        ]

class MsgAlive(Common.Message):
    messageId = 0x616C
    entries = [
        StructEntry(Common.PlayerId, 'id'),
        StructEntry(Common.Vector3, 'position'),
        StructEntry(Common.Vector3, 'forward'),
        ]

class MsgNetworkRelay(Common.Message):
    messageId = 0x6e72

class MsgShotBegin(Common.Message):
    messageId = 0x7362
    entries = [
        StructEntry(Common.PlayerId, 'playerId'),
        StructEntry(UInt16,          'shotId'),
        StructEntry(Common.Vector3,  'position'),
        StructEntry(Common.Vector3,  'velocity'),
        StructEntry(Float,           'time'),
        StructEntry(Common.FlagType, 'flagType'),
        StructEntry(Float,           'lifetime'),
        ]

class MsgDropFlag(Common.Message):
    messageId = 0x6466
    entries = [
        StructEntry(Common.PlayerId,              'playerId'),
        StructEntry(UInt16,                       'flagNum'),
        StructEntry(SubStruct(Common.FlagStatus), 'status'),
        ]

class MsgGetWorld(Common.DataMessage):
    messageId = 0x6777
    entries = [
        StructEntry(UInt32, 'remaining'),
        ]

class MsgWantWHash(Common.Message):
    messageId = 0x7768
    entries = [
        StructEntry(Enum(Int8, {
        ord('p'): 'permanent',
        ord('t'): 'temporary',
        }), 'lifetime'),
        StructEntry(StringField(33), 'hash'),
        ]

MsgPlayerUpdate = Common.MsgPlayerUpdate

### The End ###
        
    
