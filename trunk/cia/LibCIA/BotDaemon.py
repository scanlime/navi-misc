""" LibCIA.BotDaemon

A simple interface for controlling multiple IRC bots via twisted's
IRC client. This module should be as simple as practical, since
restarting it causes all the IRC bots to go offline then back on.
Normally a process running the code in this module is forked off
into the background, and BotFrontend will search for that existing
process before trying to start a new one. This way most of CIA's
code can be upgraded without users on IRC even noticing :)
"""
#
# CIA open source notification system
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

from twisted.protocols import irc
from twisted.internet import protocol, reactor
from twisted.web import server, xmlrpc
import sys, time

socketName = "/tmp/cia.BotDaemon"


class Bot(irc.IRCClient):
    """An IRC bot connected to one server any any number of channels,
       sending messages on behalf of the BotController.
       """
    def connectionMade(self):
        self.nickname = self.factory.allocator.allocateNick()
        self.channels = []
        irc.IRCClient.connectionMade(self)

    def signedOn(self):
        self.factory.allocator.botConnected(self)

    def connectionLost(self, reason):
        self.factory.allocator.botDisconnected(self)
        irc.IRCClient.connectionLost(self)

    def joined(self, channel):
        self.channels.append(channel)
        self.factory.allocator.botJoined(self, channel)


class BotFactory(protocol.ClientFactory):
    """Twisted ClientFactory for creating Bot instances"""
    protocol = Bot

    def __init__(self, allocator):
        self.allocator = allocator
        reactor.connectTCP(allocator.host, allocator.port, self)

    def clientConnectionLost(self, connector, reason):
        # Automatically reconnect
        connector.connect()


class BotAllocator:
    """Manages bot allocation for one IRC server"""
    def __init__(self, server, nickFormat="CIA-%d", channelsPerBot=2):
        self.nickFormat = nickFormat
        self.channelsPerBot = channelsPerBot
        self.host, self.port = server

        # Map nicknames to bot instances
        self.bots = {}

        # Map channel names to bots
        self.channels = {}

        # Channels we're waiting on existing bots to join
        self.existingBotRequests = []

        # List of channels we want a new bot to join.
        # If this is non-empty, we should already have
        # a new bot in the process of connecting.
        self.newBotRequests = []

    def botConnected(self, bot):
        """Called by one of our bots when it's connected successfully and ready to join channels"""
        print "%r connected" % bot.nickname
        self.bots[bot.nickname] = bot

        # Join as many channels as this bot is allowed to,
        # transferring those channels from newBotRequests to existingBotRequests.
        for channel in self.newBotRequests[:self.channelsPerBot]:
            bot.join(channel)
            self.existingBotRequests.append(channel)
            self.newBotRequests.remove(channel)

        # If we still have channels that need joining that
        # go beyond this new bot's capabilities, start another new bot.
        if self.newBotRequests:
            BotFactory(self)

    def botJoined(self, bot, channel):
        """Called by one of our bots when it's successfully joined to a channel"""
        print "%r joined %r" % (bot.nickname, channel)
        self.existingBotRequests.remove(channel)
        self.channels[channel] = bot

    def botDisconnected(self, bot):
        """Called when one of our bots has been disconnected"""
        print "%r disconnected" % bot.nickname
        del self.bots[bot.nickname]
        for channel in self.channels.iterkeys():
            if self.channels[channel] == bot:
                del self.channels[channel]

    def nickInUse(self, nick):
        """Determine if a nickname is in use. Currently this only checks those
           used by our own bots.
           """
        return self.bots.has_key(nick)

    def allocateNick(self):
        """Return an unused nickname on this server for a newly created bot to use"""
        # This nickname will use self.nickFormat, with an increasing number
        # substituted in until the nick becomes available.
        sequence = 1
        while self.nickInUse(self.nickFormat % sequence):
            sequence += 1
        return self.nickFormat % sequence

    def addChannel(self, channel):
        """Add a channel to the list of those supported by the bots on this
           server, if it's not there already. The channel may not be available
           right away if a new bot has to be connected for it. Returns a Bot
           instance if one is already available to talk on this channel, or
           None if we're in the process of making a connection to the channel.
           """
        # Do we already have this channel?
        try:
            return self.channels[channel]
        except KeyError:
            pass

        # Are we already requesting this channel from an existing or new bot?
        if channel in self.existingBotRequests:
            return None
        if channel in self.newBotRequests:
            return None

        # Check for room in our existing bots
        for bot in self.bots.itervalues():
            if len(bot.channels) < self.channelsPerBot:
                print "%r joining %r" % (bot.nickname, channel)
                bot.join(channel)
                self.existingBotRequests.append(channel)
                return None

        # We'll have to get a new bot to join this channel.
        # Add it to the list of channels for a new bot to join, and
        # create that new bot if we're the first
        self.newBotRequests.append(channel)
        if len(self.newBotRequests) == 1:
            BotFactory(self)


class BotController(xmlrpc.XMLRPC):
    def __init__(self):
        xmlrpc.XMLRPC.__init__(self)

        # A map from (host, port) to BotAllocator instances
        self.servers = {}

    def xmlrpc_getServers(self):
        """Return a list of (host, port) tuples specifying all servers we know about"""
        return self.servers.keys()

    def xmlrpc_getChannels(self, server):
        """Return the list of channels we're in on the given server"""
        return self.servers[server].channels.keys()

    def xmlrpc_getBots(self, server):
        """Return a list of all bot nicknames on the given server"""
        return [bot.nickname for bot in self.servers[server].bots]

    def xmlrpc_getBotChannels(self, server, bot):
        """Return a list of all the channels a particular bot is in, given its server and nickname"""
        return self.servers[server].bots[bot].channels

    def xmlrpc_addChannel(self, server, channel):
        """Add a new server/channel to the list supported by our bots.
           New bots are automatically started as needed.
           """
        if not self.servers.has_key(server):
            self.servers[server] = BotAllocator(server)
        self.servers[server].addChannel(channel)


def main():
    r = BotController()

    freenode = ("irc.freenode.net", 6667)
    for i in xrange(5):
        r.xmlrpc_addChannel(freenode, "#botpark_%d" % i)

    # Listen on a UNIX socket with fairly restrictive permissions,
    # since this server is quite insecure and only needs to be accessed
    # by the BotFrontend.
    reactor.listenUNIX(socketName, server.Site(r), mode=0600)
    reactor.run()

if __name__ == "__main__":
    # Helpful for testing- just run the BotController server.
    # Normally the main() would be invoked automatically by BotFrontend
    main()

### The End ###
