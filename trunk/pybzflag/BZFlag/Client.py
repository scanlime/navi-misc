""" BZFlag.Client

Provides the BaseClient class, which implements basic communication
with the server and provides hooks for adding more functionality
in subclasses.
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

import BZFlag
from BZFlag import Network, Protocol, Errors, Player, Game, World
from BZFlag.Protocol import FromServer, ToServer, Common


class BaseClient:
    """Implements a very simple but extensible BZFlag client.
       This client can connect and disconnect, and it has a system
       for asynchronously processing messages. This class only processes
       messages related to upkeep on the server-client link, such as
       lag ping, disconnection, and UDP-related messages.

       The methods of this class and its subclasses use the following
       naming conventions:

         - Low-level socket handlers should be of the form handleFoo()
         - Event handlers for messages should be of the form onMsgFoo()
         - Event handlers for message replies should be of the form onMsgFooReply()
         - Event handlers for other events should be of the form onFoo()
       """
    def __init__(self, server=None):
        self.tcp = None
        self.udp = None
        self.connected = 0
        if server:
            self.connect(server)

    def connect(self, server):
        """This does the bare minimum necessary to connect to the
           BZFlag server. It does not negotiate flags, obtain the
           world database, or join the game. After this function
           returns, the client is connected to the server and can
           receive and transmit messages.
           """
        # Establish the TCP socket
        if self.tcp:
            self.disconnect()
        self.tcp = Network.Socket()
        self.tcp.connect(server, Common.defaultPort)
        self.tcp.setBlocking(0)

        # Until we establish a UDP connection, we'll need to send
        # normally-multicasted messages over TCP
        self.multicast = self.tcp

        # Now we have to wait for the server's Hello packet,
        # with the server version and client ID.
        self.tcp.handler = self.handleHelloPacket

    def disconnect(self):
        if self.tcp:
            self.tcp.close()
            self.tcp = None
        if self.udp:
            self.udp.close()
            self.udp = None
        self.multicast = None
        self.connected = 0

    def getSockets(self):
        """Returns a list of sockets the client expects
           incoming data on. This is meant to be used with the
           Network.EventLoop class or compatible.
           """
        sockets = []
        if self.tcp:
            sockets.append(self.tcp)
        if self.udp:
            sockets.append(self.udp)
        return sockets

    def run(self):
        """A simple built-in event loop, for those not wanting
           to integrate the BZFlag client into an existing event
           loop using the above getSockets() and the sockets'
           poll() method.
           """
        Network.EventLoop().run(self.getSockets())

    def handleHelloPacket(self, socket, eventLoop):
        """This is a callback used to handle incoming socket
           data when we're expecting a hello packet.
           """
        # We should have just received a Hello packet with
        # the server version and our client ID.
        hello = socket.readStruct(FromServer.HelloPacket)
        if hello.version != BZFlag.protocolVersion:
            raise Errors.ProtocolError(
                "Protocol version mismatch: The server is version " +
                "'%s', this client is version '%s'." % (
                hello.version, BZFlag.protocolVersion))
        self.id = hello.clientId
        
        # Now we're connected
        self.connected = 1
        socket.handler = self.handleMessage
        self.onConnect()

    def handleMessage(self, socket, eventLoop):
        """This is a callback used to handle incoming socket
           data when we're expecting a message.
           """
        # This can return None if part of the mesasge but not the whole
        # thing is available. The rest of the message will be rebuffered,
        # so we'll read the whole thing next time this is called.
        msg = socket.readMessage(FromServer)
        if msg:
            msg.socket = socket
            msg.eventLoop = eventLoop
            msgName = msg.__class__.__name__
            handler = getattr(self, "on%s" % msgName, None)
            if self.onAnyMessage(msg):
                return
            if handler:
                handler(msg)
            else:
                self.onUnhandledMessage(msg)

    def onConnect(self):
        """This is called after a connection has been established.
           By default it doesn't do anything, it's up to subclasses
           to define what this does next.
           """
        pass

    def onAnyMessage(self, msg):
        """This is a hook that subclasses can use to easily
           monitor and intercept messages. It is called before
           dispatching each message, and if it returns true that
           message is cancelled.
           """
        return None

    def onUnhandledMessage(self, msg):
        raise Errors.ProtocolWarning("Unhandled message %s" % msg.__class__.__name__)
    
    def onMsgSuperKill(self, msg):
        """The server wants us to die immediately"""
        self.disconnect()
        msg.eventLoop.stop()

    def onMsgLagPing(self, msg):
        """The server is measuring our lag, reply with the same message."""
        msg.socket.write(msg)

    def onMsgNetworkRelay(self, msg):
        """The server needs us to use TCP instead of UDP for messages
           that we'd normally multicast.
           """
        self.multicast = self.tcp

        
class StatefulClient(BaseClient):
    """Extends the BaseClient to keep track of the state of the game
       world, as reported by the server and the other clients.
       """
    def __init__(self, server=None):
        self.game = Game.Game()
        self.worldCache = None
        BaseClient.__init__(self, server)

    def onConnect(self):
        """Immediately after connecting, ask for a world hash so
           we can check our cache for a copy of that world
           """
        self.getWorldHash()

    def getWorldHash(self):
        """Ask for a hash of the binary world data, so we can
           check our cache for it. This will trigger onMsgWantHash()
           """
        self.tcp.write(ToServer.MsgWantWHash())
        
    def onMsgWantWHash(self, msg):
        """Receive the world hash. If this is a permanent
           (not automatically generated by bzfs) map, try
           to cache it.
           """
        if msg.lifetime == 'permanent':
            self.worldCache = World.Cache()
        else:
            self.worldCache = None
        self.worldHash = msg.hash
            
        if self.worldCache and self.worldCache.hasWorld(self.worldHash):
            # Yay, the world is in our cache
            f = self.worldCache.openWorld(self.worldHash)
            self.game.world.loadBinary(f)
            f.close()
            self.onLoadWorld()
        else:
            # We're not using the cache or it didn't have our world.
            # Start a download.
            self.downloadWorld()

    def downloadWorld(self):
        """Start downloading the game world from the server.
           This will trigger onMsgGetWorld(), which will send
           more MsgGetWorlds until the entire world has been downloaded.
           """
        self.binaryWorld = ''
        self.tcp.write(ToServer.MsgGetWorld(offset=0))

    def onMsgGetWorld(self, msg):
        """We've received one chunk of the binary world. If there's more,
           request it, otherwise load the world and move on.
           """
        self.binaryWorld += msg.data
        if msg.remaining:
            # We need more data!
            self.tcp.write(ToServer.MsgGetWorld(offset=len(self.binaryWorld)))
        else:
            # Download is complete. Convert the binary world
            # into a World object and discard the binary world.
            # If we're using a cache, save a copy of the map.
            if self.worldCache:
                self.worldCache.storeWorld(self.worldHash, self.binaryWorld)
            import StringIO
            self.game.world.loadBinary(StringIO.StringIO(self.binaryWorld))
            del self.binaryWorld
            self.worldDownloaded = 1
            self.onLoadWorld()

    def onLoadWorld(self):
        """The world has been loaded from the server. This is a hook for
           subclasses to take further action, like entering the game.
           """
        pass

    def onMsgFlagUpdate(self, msg):
        pass

    def onMsgTeamUpdate(self, msg):
        pass

    def onMsgAddPlayer(self, msg):
        pass

    def onMsgRemovePlayer(self, msg):
        pass

    def onMsgNewRabbit(self, msg):
        pass

    def onMsgAlive(self, msg):
        pass

    def onMsgPlayerUpdate(self, msg):
        pass

    def onMsgShotBegin(self, msg):
        pass


class PlayerClient(StatefulClient):
    """Extends the StatefulClient with functionality for implementing
       a player. This includes methods for entering and leaving the
       game, and a method of glueing the client with a frontend that
       provides actual player interaction, or a bot AI.
       """
    def __init__(self, server, playerIdentity):
        self.player = Player.Player(playerIdentity)
        self.inGame = 0
        StatefulClient.__init__(self, server)

    def onDownloadWorld(self):
        self.enterGame()

    def enterGame(self):
        msg = ToServer.MsgEnter()
        msg.playerType = self.player.identity.type
        msg.team = self.player.identity.team
        msg.callSign = self.player.identity.callSign
        msg.emailAddress = self.player.identity.emailAddress
        self.tcp.write(msg)

    def disconnect(self):
        self.exitGame()
        StatefulClient.disconnect(self)

    def exitGame(self):
        self.inGame = 0
        self.tcp.write(ToServer.MsgExit())

    def onMsgAccept(self, msg):
        """This is called after we try to enterGame, if it's successful."""
        self.inGame = 1
        self.onEnterGame()

    def onEnterGame(self):
        pass

    def onMsgReject(self, msg):
        """This is called after we try to enterGame, if we failed."""
        raise Errors.GameException("Unable to enter the game: %s" % msg.reason)


### The End ###
        
    
