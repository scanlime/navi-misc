""" BZFlag.Client.Player

Provides the PlayerClient class, which extends StatefulClient to
include functionality necessary for implementing a player.
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
import BZFlag
from BZFlag import Errors, Event, Player
from BZFlag.Client.Stateful import StatefulClient


class PlayerClient(StatefulClient):
    """Extends the StatefulClient with functionality for implementing
       a player. This includes methods for entering and leaving the
       game, and a method of glueing the client with a frontend that
       provides actual player interaction, or a bot AI.
       """
    def init(self):
        StatefulClient.init(self)
        self.options.update({
            'callSign':   None,
            'team':       'rogue',
            'email':      'PyBZFlag',
            'playerType': 'tank',
            })

        # We won't have a player instance until we get a MsgAddPlayer
        # back for ourselves and StatefulClient adds it to the Game.
        self.inGame = 0
        self.player = None
        Event.attach(self, 'onEnterGame')

        # Set up an observer for the game's onAddPlayer that will set
        # up our player member whenever that MsgAddPlayer is received.
        self.game.onAddPlayer.observe(self.addPlayer)
        self.game.world.onLoad.observe(self.enterGame)

    def addPlayer(self, game, player):
        if player.identity.playerId == self.id:
            self.player = player
            self.onEnterGame()

    def enterGame(self):
        msg = self.outgoing.MsgEnter()
        identity = Player.Identity(self.options['callSign'],
                                   self.options['team'],
                                   self.options['email'],
                                   self.options['playerType'])

        if not identity.callSign:
            raise Errors.ProtocolError("A call sign is required to enter the game")
        msg.playerType = identity.type
        msg.team = identity.team
        msg.callSign = identity.callSign
        msg.emailAddress = identity.emailAddress
        self.tcp.write(msg)

    def disconnect(self):
        self.exitGame()
        StatefulClient.disconnect(self)

    def exitGame(self):
        self.inGame = 0
        self.tcp.write(self.outgoing.MsgExit())

    def onMsgAccept(self, msg):
        """This is called after we try to enterGame, if it's successful.
           This probably shouldn't be used for anything important, since it
           also might be used in other undocumented ways.
           """
        pass

    def onMsgReject(self, msg):
        """This is called after we try to enterGame, if we failed."""
        raise Errors.GameException("Unable to enter the game: %s" % msg.reason)

    def sendMessage(self, message, destination='all'):
        """Send a message to other players"""
        self.tcp.write(self.outgoing.MsgMessage(destination = destination,
                                                message = message))

### The End ###
