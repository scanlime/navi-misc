""" BZFlag.Server.Stateful

Provides the StatefulServer class, which extends BaseServer to
support updating a game state and transmitting changes.
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
from BZFlag import Event, Game, Flag, Meta, World, Player
from StringIO import StringIO
from BZFlag.Server.Base import BaseServer


class StatefulServer(BaseServer):
    """On top of the basic networking functionality given by BaseServer,
       this implements a server that knows about a game state and can
       communicate this to clients.
       """
    def init(self):
        BaseServer.init(self)
        self.game = Game.Game()
        self.binaryWorld = None
        self.publicityTimer = None
        self.worldBlockSize = 1000

        self.options.update({
            'world':              None,
            'welcomeMessage':     BZFlag.serverWelcomeMessage,
            'publicTitle':        None,
            'publicityInterval':  30 * 60,
            'gameStyle':          [],
            'shots':              1,
            })

        Event.attach(self, 'onAttemptEnter', 'onEnter', 'onFinishEnter',
                     'onWelcome')

        self.onListen.observe(self.publicize)

    def setOptions(self, **options):
        Network.Endpoint.setOptions(self, **options)
        if 'world' in options.keys() and options['world']:
            self.game.world = World.load(options['world'])

        if 'gameStyle' in options.keys():
            self.game.world.style.gameStyle = options['gameStyle']

        if 'shots' in options.keys():
            self.game.world.style.shots = int(options['shots'])

    def publicize(self, enable=None):
        """Periodically post information about this server to the metaserver.
           If enable is None, it is automatically set according to whether or
           not the publicTitle option is true.
           """
        if enable is None:
            enable = self.options['publicTitle']

        if self.publicityTimer:
            # Remove any old timers
            self.eventLoop.remove(self.publicityTimer)
            self.publicityTimer = None
        if enable:
            # Post our server info immediately, and set up a timer to repost
            # it periodically, in case the server loses it for some reason.
            def postInfo():
                Meta.MetaClient(self.eventLoop).add(self.getServerInfo())
            self.publicityTimer = Event.PeriodicTimer(
                self.options['publicityInterval'], postInfo)
            self.eventLoop.add(self.publicityTimer)
            postInfo()
        else:
            # Remove our posting from the list
            Meta.MetaClient(self.eventLoop).remove(self.getServerInfo().name)

    def getServerInfo(self):
        """Return a ServerInfo instance, used to advertise
           ourselves to list servers.
           """
        info = Meta.ServerInfo()

        # This should be our canonical hostname
        import socket
        info.name     = "%s:%s" % (
            socket.gethostbyname_ex(socket.gethostname())[0], self.tcp.port)

        info.version  = self.protocolVersion
        info.gameinfo = self.game.getGameInfo()
        info.title    = self.options['publicTitle']
        print info.info()
        return info

    def onMsgNegotiateFlags(self, msg):
        """Given a list of supported flags by the client and the
           flags in the game world, send back a list of flags the
           client is missing.
           """
        supportedFlags = Flag.splitAbbreviations(msg.data)
        missingFlags = []
        for flag in self.game.flags.values():
            if not flag.abbreviation in supportedFlags:
                missingFlags.append(flags)
        msg.client.write(self.outgoing.MsgNegotiateFlags(
            data = Flag.joinAbbreviations(missingFlags)))

    def onMsgWantWHash(self, msg):
        """Send the client an MD5 hash of the current world"""
        msg.client.write(self.outgoing.MsgWantWHash(
            lifetime = self.game.world.lifetime,
            hash = self.game.world.getHash()))

    def onMsgGetWorld(self, msg):
        """Send the client a section of our world"""
        # Convert the world to binary only on the first chunk we send
        if msg.offset == 0 or not self.binaryWorld:
            f = StringIO()
            self.game.world.saveBinary(f)
            self.binaryWorld = f.getvalue()

        block = self.binaryWorld[msg.offset:msg.offset + self.worldBlockSize]
        msg.client.write(self.outgoing.MsgGetWorld(
            remaining = len(self.binaryWorld) - msg.offset - len(block),
            data = block))

    def onMsgEnter(self, msg):
        """Sent by the client to request entry into the game.
           the onEnter event provides a hook for denying the
           requests. Default is to allow.

           Events:

             onAttemptEnter(msg) - Sent when a client is trying to enter.
                                   Return a Common.RejectionCode string to
                                   veto the entry.

             onEnter(msg)        - Perform tasks necessary to enter the
                                   player into the game, such as sending
                                   initial score updates.

             onFinishEnter(msg)  - Send a MsgAddPlayer for the new player,
                                   ending the entering process.

             onWelcome(msg)      - Send a message to the player indicating
                                   that they have entered the game.
           """
        reason = self.onAttemptEnter(msg)
        if reason:
            # An observer vetoed the request, deny it
            # using the given reason
            msg.client.write(self.outgoing.MsgReject(reason=reason))
        else:
            # Accept it. The player is now in the game.
            msg.client.write(self.outgoing.MsgAccept())
            self.onEnter(msg)
            self.onFinishEnter(msg)
            self.onWelcome(msg)

    def onFinishEnter(self, msg):
        """Finish the process of entering a client into the game by
           sending it a MsgAddPlayer for itself.
           """
        addmsg = self.outgoing.MsgAddPlayer(
            id           = msg.client.id,
            type         = msg.playerType,
            team         = msg.team,
            wins         = 0,
            losses       = 0,
            teamKills    = 0,
            callSign     = msg.callSign,
            emailAddress = msg.emailAddress)

        # Send it to the client and use it to update our game state
        msg.client.write(addmsg)
        self.game.addPlayer(Player.fromMessage(addmsg))

    def onWelcome(self, msg):
        """Event triggered after a player has completely entered the
           game, to welcome them in with a message.
           """
        msg.client.write(self.outgoing.MsgMessage(
            fromId = 'server',
            toId = msg.client.id,
            message = self.options['welcomeMessage']))

    def onMsgPlayerUpdate(self, msg):
        self.game.updatePlayerFromMessage(msg)

### The End ###
