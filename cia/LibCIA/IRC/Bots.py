""" LibCIA.IRC.Bots

A small library for managing multiple IRC bots on multiple IRC networks
"""
#
# CIA open source notification system
# Copyright (C) 2003-2004 Micah Dowty <micahjd@users.sourceforge.net>
#
#  This program is free software; you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation; either version 2 of the License, or
#  (at your option) any later version.
#
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with this program; if not, write to the Free Software
#  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
#

from twisted.protocols import irc
from twisted.internet import protocol, reactor
from twisted.python import log
import time


class ChannelMessageQueue:
    """A way to deliver buffered messages to a particular IRC server and
       channel, handling flood protection and load balancing when necessary.
       """
    def __init__(self, botNet, server, channel):
        self.request = ChannelRequest(botNet, server, channel)
        self.queuedLines = []

    def send(self, message):
        """Split up a message into lines and queue it for transmission"""
        self.queuedLines.extend(message.split('\n'))
        self.checkQueue()

    def cancel(self):
        """Cancel the request associated with this message queue"""
        self.request.cancel()

    def checkQueue(self):
        # FIXME: for now, send the whole queue contents to the first bot.
        #        This is where rate limiting and load balancing goes.
        while self.queuedLines:
            if not self.request.bots:
                return
            self.request.bots[0].msg(self.request.channel, self.queuedLines[0])
            del self.queuedLines[0]


class Request:
    """The Request object is an abstract base class for needs that can
       be fulfilled by a set of IRC bots.

       This class has a 'bot' member that holds a list of Bot instances
       fulfilling this request. It will be an empty list if this request
       is not fulfilled.

       The recheck() method decides whether the request is fulfilled
       or not by setting 'bot'. If the request isn't fulfilled, and
       it has been longer than fulfillmentTimeout since the last
       action was taken, takeAction will be called.
       """
    # Timers for this request, as mentioned above. All timers in seconds.
    fulfillmentTimeout = 60*3

    # Time at which the last action was taken
    lastActionTime = None

    fulfillmentTimer = None
    bots = None
    _active = True

    def __init__(self, botNet):
        self.botNet = botNet
        log.msg("New request %r" % self)
        botNet.addRequest(self)
        self.recheck()

    def cancelFulfillmentTimer(self):
        if self.fulfillmentTimer and self.fulfillmentTimer.active():
            self.fulfillmentTimer.cancel()
        self.fulfillmentTimer = None

    def active(self):
        """Return True if this request is still active, similar to DelayedCall's interface"""
        return self._active

    def cancel(self):
        """Indicate that this request is no longer needed. It is
           removed from the bot network, and all our current timers are disabled.
           """
        self.botNet.removeRequest(self)
        self.cancelFulfillmentTimer()
        self._active = False
        log.msg("Cancelled %r" % self)

    def recheck(self):
        """Decide whether the request is fulfilled or not, and take actions if necessary"""
        self.bots = self.findBots()
        if self.isFulfilled():
            # Yay, we're done. Clean up a bit.
            self.lastActionTime = None
            self.cancelFulfillmentTimer()
        else:
            # Nope, still not fulfilled.
            # If it's been a while since the last action we took, try it again
            if self.lastActionTime is None or time.time() > self.lastActionTime + self.fulfillmentTimeout:
                self.lastActionTime = time.time()
                self.takeAction()

                # Set a timer to recheck in a while so that if that action failed we can try again
                self.cancelFulfillmentTimer()
                self.fulfillmentTimer = reactor.callLater(self.fulfillmentTimeout + 2, self.recheck)

    def findBots(self):
        """To be implemented by subclasses: return a list of Bot instances that satisfy this Request"""
        return []

    def takeAction(self):
        """To be implemented by subclasses: none of the current bots satisfy this
           need, take some action to try to correct this.
           """
        pass

    def isFulfilled(self):
        """By default, a request is fulfilled if we have any bots attached.
           If the subclass knows better, this can be overridden.
           """
        return len(self.bots) > 0


class Server:
    """Represents an IRC server, by host and optional port number"""
    defaultPort = 6667

    def __init__(self, host, port=None):
        if port is None:
            port = self.defaultPort
        self.host = host
        self.port = port

    def __str__(self):
        if self.port == self.defaultPort:
            return self.host
        else:
            return "%s:%d" % (self.host, self.port)

    def __repr__(self):
        return "<Server at %s>" % self

    def __eq__(self, other):
        return self.host == other.host and self.port == other.port

    def __hash__(self):
        return hash((self.host, self.port))


class ChannelRequest(Request):
    """A Request implementation that requires a specified number of bots to be
       in a particular IRC channel on any server.
       """
    def __init__(self, botNet, server, channel, numBots=1):
        self.server = server
        self.channel = channel
        self.numBots = numBots
        Request.__init__(self, botNet)

    def __repr__(self):
        return "<ChannelRequest for %s on %s>" % (self.channel, self.server)

    def findBots(self):
        # Look for our server and channel in the map from servers to bot lists
        matches = []
        if self.server in self.botNet.servers:
            # Look for our channel in the map from channel names to bot lists
            for bot in self.botNet.servers[self.server]:
                if self.channel in bot.channels:
                    matches.append(bot)

        # Ignore any bots we don't need
        return matches[:self.numBots]

    def isFulfilled(self):
        return len(self.bots) == self.numBots

    def takeAction(self):
        # First, see if there are any existing bots already on our server that aren't full
        for bot in self.botNet.servers.get(self.server, []):
            if not bot.isFull():
                bot.join(self.channel)
                return

        # Nope, ask for a new bot
        self.botNet.requestNewBot(self, self.server)

        # FIXME: HACK!
        self.lastActionTime = None


class BotNetwork:
    """A collection of IRC bots that work to satisfy a collection of Request objects.
       Users should interact with the BotNetwork via Request instances.
       """
    # Every so often we check whether bots are really needed. This is the bot
    # garbage collection interval, in seconds. This also implicitly gives
    # each request a chance to recheck itself.
    botCheckInterval = 60

    # Timeout, in seconds, for creating new bots
    newBotTimeout = 120

    # Bots are given this many seconds after being marked inactive before they're
    # disconnected. This way if a request is deleted then immediately replaced
    # with another that has similar requirements, we don't end up replacing
    # a bot we just deleted. I'm sure it has other good qualities too.
    botInactivityTimeout = 60 * 5

    botCheckTimer = None

    def __init__(self, nickFormat):
        self.requests = []

        # A map from Server to list of Bots
        self.servers = {}

        # A list of all servers for which bots are being created currently.
        # Maps from Server instance to a DelayedCall signalling a timeout.
        self.newBotServers = {}

        # Lists all bots we're thinking about deleting due to inactivity.
        # Maps from Bot instance to a DelayedCall.
        self.inactiveBots = {}

        # Map (server, channel) tuples to message queues
        self.messageQueueMap = {}

        # Start the bot checking cycle
        self.checkBots()

    def addRequest(self, request):
        """Add a request to be serviced by this bot network. This should
           only be called by the Request class, as it automatically registers
           itself with the botNet it was constructed for.
           """
        self.requests.append(request)

    def removeRequest(self, request):
        """Indicates that a request is no longer needed"""
        self.requests.remove(request)
        self.checkBots()

    def requestNewBot(self, request, server):
        """Called by a Request to indicate that it needs a new bot created for a given server.
           Since the Request isn't aware that another Request may have already expressed an
           interest in the same server, this needs to ensure that only one new bot at a time
           is created for each server.
           """
        if not server in self.newBotServers:
            self.createBot(server)

    def createBot(self, server):
        """Create a new bot for the given server, retrying if necessary"""
        # We're not already trying to connect a bot, or our previous attempt failed.
        # Start trying to connect a bot, and set a timeout.
        log.msg("Creating a new IRC bot for %s" % server)
        BotFactory(self, server)
        self.newBotServers[server] = reactor.callLater(self.newBotTimeout, self.newBotTimedOut, server)

    def newBotTimedOut(self, server):
        """We just timed out waiting for a new bot connection. Try again."""
        log.msg("Timed out waiting for an IRC bot to connect to %s, trying again" % server)
        self.createBot(server)

    def botConnected(self, bot):
        """Called by a bot when it has been successfully connected."""
        self.servers.setdefault(bot.server, []).append(bot)
        log.msg("Bot %r connected" % bot)

        try:
            timer = self.newBotServers[bot.server]
            if timer.active():
                timer.cancel()
            del self.newBotServers[bot.server]
        except KeyError:
            # Hmm, we weren't waiting for this bot to connect?
            # Oh well, this bot will be garbage collected soon if that's really the case.
            pass
        self.checkBots()

    def botDisconnected(self, bot):
        """Called by a bot when it has been disconnected for any reason. Since
           this might have been a disconnection we didn't intend, do another bot CHECK
           run right away to recheck all requests.
           """
        self.servers[bot.server].remove(bot)
        log.msg("Bot %r disconnected" % bot)
        if not self.servers[bot.server]:
            del self.servers[bot.server]

        self.checkBots()

    def checkBots(self):
        """This gives all requests a chance to recheck themselves, and searches
           for bots that are no longer being used, eventually disconnecting them.
           """
        referencedBots = {}
        for request in self.requests:
            request.recheck()
            for refBot in request.bots:
                referencedBots[refBot] = True

                # Make sure that any referenced bots are no longer marked inactive
                if refBot in self.inactiveBots:
                    timer = self.inactiveBots[refBot]
                    if timer.active():
                        timer.cancel()
                    del self.inactiveBots[refBot]

        # Now find any bots that aren't in referencedBots or inactiveBots already,
        # and mark them as inactive (setting a fresh inactivity timer)
        for server in self.servers.itervalues():
            for bot in server:
                if bot not in referencedBots and bot not in self.inactiveBots:
                    self.inactiveBots[bot] = reactor.callLater(
                        self.botInactivityTimeout, self.botInactivityCallback, bot)

        # Set up the next round of bot checking
        if self.botCheckTimer and self.botCheckTimer.active():
            self.botCheckTimer.cancel()
        self.botCheckTimer = reactor.callLater(self.botCheckInterval, self.checkBots)

    def botInactivityCallback(self, bot):
        """Bots that have been unused for a while eventually end up here, and are disconnected"""
        log.msg("Disconnecting inactive bot %r" % bot)
        bot.quit()

    def botJoined(self, bot, channel):
        log.msg("%r joined %r" % (bot, channel))
        self.checkBots()

    def botLeft(self, bot, channel):
        log.msg("%r left %r" % (bot, channel))
        self.checkBots()

    def botKicked(self, bot, channel, kicker, message):
        # FIXME: remove the ruleset
        pass


class Bot(irc.IRCClient):
    """An IRC bot connected to one server any any number of channels,
       sending messages on behalf of the BotController.
       """
    maxChannels = 18

    def __repr__(self):
        return "<Bot %r on server %s>" % (self.nickname, self.server)

    def isFull(self):
        return len(self.channels) + len(self.requestedChannels) >= self.maxChannels

    def connectionMade(self):
        """Called by IRCClient when we have a socket connection to the server.
           This allocates the nick we'd like to use.
           """
        self.emptyChannels()
        self.server = self.factory.server
        self.botNet = self.factory.botNet
        self.nickname = "squiggly"
        irc.IRCClient.connectionMade(self)

    def emptyChannels(self):
        """Called when we know we're not in any channels and we shouldn't
           be trying to join any yet.
           """
        self.channels = []
        self.requestedChannels = []

    def signedOn(self):
        """IRCClient is notifying us that we've finished connecting to
           the IRC server and can finally start joining channels.
           """
        self.emptyChannels()
        self.botNet.botConnected(self)

    def connectionLost(self, reason):
        self.botNet.botDisconnected(self)
        irc.IRCClient.connectionLost(self)

    def join(self, channel):
        """Called by the bot's owner to request that a channel be joined.
           It's added to our list of pending requests. Eventually we expect
           to get a call to joined() because of this.
           """
        self.requestedChannels.append(channel)
        irc.IRCClient.join(self, channel)

    def joined(self, channel):
        """A channel has successfully been joined"""
        # Note that if we get a join message for a channel we haven't
        # requested, this first line will raise an exception and the
        # message will be effectively ignored.
        self.requestedChannels.remove(channel)
        self.channels.append(channel)
        self.factory.botNet.botJoined(self, channel)

    def left(self, channel):
        """Called when part() is successful and we've left a channel.
           Implicitly also called when we're kicked via kickedFrom().
           """
        self.channels.remove(channel)
        self.factory.botNet.botLeft(self, channel)

    def kickedFrom(self, channel, kicker, message):
        self.left(channel)
        self.factory.botNet.botKicked(self, channel, kicker, message)


class BotFactory(protocol.ClientFactory):
    """Twisted ClientFactory for creating Bot instances"""
    protocol = Bot

    def __init__(self, botNet, server):
        self.botNet = botNet
        self.server = server
        reactor.connectTCP(server.host, server.port, self)

    def clientConnectionLost(self, connector, reason):
        log.msg("IRC Connection to %r lost: %r" % (self.server, reason))

    def clientConnectionFailed(self, connector, reason):
        log.msg("IRC Connection to %r failed: %r" % (self.server, reason))

### The End ###
