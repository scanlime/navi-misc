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
from twisted.internet import protocol, reactor, defer
from twisted.python import log
import time


class Request:
    """The Request object specifies a server, optionally a channel, and
       a number of bots that need to inhabit that server/channel.

       When a request is created, it registers itself with the BotNetwork,
       which then tries its hardest to keep the request satisfied. The
       bots satisfying this request are available in its 'bots' member.
       """
    def __init__(self, botNet, server, channel=None, numBots=1):
        self.bots = []
        self._active = True

        self.botNet = botNet
        self.server = server
        self.channel = channel
        self.numBots = numBots

        log.msg("New %r" % self)
        botNet.addRequest(self)

    def __repr__(self):
        if self.numBots == 1:
            botInfo = "1 bot"
        else:
            botInfo = "%d bots" % self.numBots
        if self.channel:
            chanInfo = " in %s" % self.channel
        else:
            chanInfo = ''
        return "<Request for %s%s on %s>" % (botInfo, chanInfo, self.server)

    def active(self):
        """Return True if this request is still active, similar to DelayedCall's interface"""
        return self._active

    def cancel(self):
        """Indicate that this request is no longer needed. It is removed from the bot network."""
        self.botNet.removeRequest(self)
        self._active = False
        log.msg("Cancelled %r" % self)

    def findBots(self):
        """Find bots that match this request, storing them in self.bots"""
        # Look for our server and channel in the map from servers to bot lists
        matches = []
        if self.server in self.botNet.servers:
            # Look for our channel in the map from channel names to bot lists
            for bot in self.botNet.servers[self.server]:
                if self.channel in bot.channels:
                    matches.append(bot)

        # Ignore any bots we don't need
        self.bots = matches[:self.numBots]

    def isFulfilled(self):
        return len(self.bots) == self.numBots


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

    def __cmp__(self, other):
        if not isinstance(other, Server):
            return cmp(self.__class__, other.__class__)
        else:
            return cmp((self.host, self.port), (other.host, other.port))

    def __hash__(self):
        return hash((self.host, self.port))


class SequentialNickAllocator:
    """A nick allocator is responsible for determining what constitutes a valid nick,
       and generating a list of valid nicks. This particular implementation
       uses sequentially numbered bots starting with a given prefix.
       """
    def __init__(self, prefix, username='CIA', realname='CIA Bot (http://cia.navi.cx)'):
        self.prefix = prefix
        self.username = username
        self.realname = realname

    def isValid(self, nick):
        if not nick.startswith(self.prefix):
            return False
        numericPart = nick[len(self.prefix):]
        try:
            int(numericPart)
            return True
        except ValueError:
            return False

    def generate(self):
        """Generate a sequence of valid nicks, starting with the most preferable"""
        i = 1
        while True:
            yield self.prefix + str(i)
            i += 1


class BotNetwork:
    """A collection of IRC bots that work to satisfy a collection of Request objects.
       Users should interact with the BotNetwork via Request instances.
       """
    # In addition to checking bot status immediately after changes that
    # are likely to be important, we check bot status periodically, every
    # botCheckInterval seconds.
    botCheckInterval = 60

    # Timeout, in seconds, for creating new bots
    newBotTimeout = 60 * 5

    # Bots are given this many seconds after being marked inactive before they're
    # disconnected. This way if a request is deleted then immediately replaced
    # with another that has similar requirements, we don't end up replacing
    # a bot we just deleted. I'm sure it has other good qualities too.
    botInactivityTimeout = 60 * 5

    botCheckTimer = None

    def __init__(self, nickAllocator):
        self.nickAllocator = nickAllocator
        self.requests = []

        # A map from Server to list of Bots
        self.servers = {}

        # A list of all servers for which bots are being created currently.
        # Maps from Server instance to a DelayedCall signalling a timeout.
        self.newBotServers = {}

        # Lists all bots we're thinking about deleting due to inactivity.
        # Maps from Bot instance to a DelayedCall.
        self.inactiveBots = {}

        # Start the bot checking cycle
        self.checkBots()

    def findBot(self, host, nickname, port=None):
        """Find the bot currently on the given server with the given nick.
           This is mostly for use with the debug console. Note that for
           convenience, the server is specified as a host and port here.
           A Server instance will be created.
           """
        server = Server(host, port)
        try:
            bots = self.servers[server]
        except KeyError:
            return None
        for bot in bots:
            if bot.nickname == nickname:
                return bot

    def findRequest(self, host, channel=None, port=None):
        """Find a request matching the given host, port, and channel.
           This is mostly for use with the debug console, hence it taking
           a host and port for convenience rather than a Server instance.
           """
        server = Server(host, port)
        for req in self.requests:
            if req.server == server and req.channel == channel:
                return req

    def addRequest(self, request):
        """Add a request to be serviced by this bot network. This should
           only be called by the Request class, as it automatically registers
           itself with the botNet it was constructed for.
           """
        self.requests.append(request)
        self.checkBots()

    def removeRequest(self, request):
        """Indicates that a request is no longer needed"""
        self.requests.remove(request)
        self.checkBots()

    def checkBots(self):
        """Search for unfulfilled requests, trying to satisfy them, then search
           for unused bots and channels, deleting them or scheduling them for deletion.
           """
        # Scan through all requests, trying to satisfy those that aren't.
        # Make note of which bots and which channels are actually being used.
        # activeBots is a map from Bot instance to a map of channels that are being used.
        usedBots = {}
        for request in self.requests:
            request.findBots()

            if not request.isFulfilled():
                # This request isn't fulfilled, try to change that
                self.tryToFulfill(request)

            for reqBot in request.bots:
                # Make note of the bots and channels this request needs,
                # and if the bot is already marked as inactive, cancel that.
                usedBots.setdefault(reqBot, {})[request.channel] = True

                # Make sure that any referenced bots are no longer marked inactive
                if reqBot in self.inactiveBots:
                    timer = self.inactiveBots[reqBot]
                    if timer.active():
                        timer.cancel()
                    del self.inactiveBots[reqBot]

        # Now look for unused bots and/or channels
        for server in self.servers.itervalues():
            for bot in server:
                if bot in usedBots:
                    usedChannels = usedBots[bot]

                    # We need this bot.. but are all the channels necessary?
                    for channel in bot.channels:
                        if channel not in usedChannels:
                            bot.part(channel)

                else:
                    # We don't need this bot. Tell it to part all of its channels,
                    # and if it isn't already, schedule it for deletion.
                    for channel in bot.channels:
                        bot.part(channel)
                    if bot not in self.inactiveBots:
                        self.inactiveBots[bot] = reactor.callLater(
                            self.botInactivityTimeout, self.botInactivityCallback, bot)

        # Set up the next round of bot checking
        if self.botCheckTimer and self.botCheckTimer.active():
            self.botCheckTimer.cancel()
        self.botCheckTimer = reactor.callLater(self.botCheckInterval, self.checkBots)

    def tryToFulfill(self, request):
        """Given an unfulfilled request, try to take actions to fulfill it"""
        # How many more bots do we need?
        neededBots = request.numBots - len(request.bots)

        # Do we have any existing bots that can join a channel to fulfill the request?
        if request.channel and request.server in self.servers:
            for bot in self.servers[request.server]:
                if bot not in request.bots:
                    # If the bot's already trying to connect to our channel,
                    # decrease the needed bots count so we don't end up asking
                    # all our bots to join this channel before the first one succeeds
                    if request.channel in bot.requestedChannels:
                        neededBots -= 1
                    elif not bot.isFull():
                        bot.join(request.channel)
                        neededBots -= 1
                    if neededBots <= 0:
                        return

        # Nope... how about asking more bots to join the request's server?
        if not request.server in self.newBotServers:
            self.createBot(request.server)

    def createBot(self, server):
        """Create a new bot for the given server, retrying if necessary"""
        # We're not already trying to connect a bot, or our previous attempt failed.
        # Start trying to connect a bot, and set a timeout.
        log.msg("Creating a new IRC bot for %s" % server)
        BotFactory(self, server)
        self.newBotServers[server] = reactor.callLater(self.newBotTimeout, self.newBotTimedOut, server)

    def newBotTimedOut(self, server):
        """We just timed out waiting for a new bot connection. Try again."""
        log.msg("Timed out waiting for an IRC bot to connect to %s" % server)
        del self.newBotServers[server]
        # Don't immediately assume that we need to try again, but give us a chance to check
        self.checkBots()

    def botInactivityCallback(self, bot):
        """Bots that have been unused for a while eventually end up here, and are disconnected"""
        log.msg("Disconnecting inactive bot %r" % bot)
        bot.quit()

    def botConnected(self, bot):
        """Called by a bot when it has been successfully connected."""
        self.servers.setdefault(bot.server, []).append(bot)

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
           this might have been a disconnection we didn't intend, do another bot
           check right away.
           """
        try:
            self.servers[bot.server].remove(bot)
            if not self.servers[bot.server]:
                del self.servers[bot.server]
        except:
            # The bot might have not been in our server list in the first
            # place, if it got disconnected before becoming fully connected.
            pass

        self.checkBots()

    def botJoined(self, bot, channel):
        self.checkBots()

    def botLeft(self, bot, channel):
        self.checkBots()

    def botKicked(self, bot, channel, kicker, message):
        # FIXME: remove the ruleset
        pass


class Bot(irc.IRCClient):
    """An IRC bot connected to one server any any number of channels,
       sending messages on behalf of the BotController.

       The Bot class is responsible for keeping track of the timers and
       limits associated with joining channels, but it doesn't map itself
       onto Requests, nor does it manage bot connection and disconnection.
       """
    maxChannels = 18

    # Timeout, in seconds, for joining channels
    joinTimeout = 60

    def __init__(self):
        self.channels = []

        # a map from channel name to a DelayedCall instance representing its timeout
        self.requestedChannels = {}

    def __repr__(self):
        return "<Bot %r on server %s>" % (self.nickname, self.server)

    def isFull(self):
        return len(self.channels) + len(self.requestedChannels) >= self.maxChannels

    def connectionMade(self):
        """Called by IRCClient when we have a socket connection to the server."""
        self.emptyChannels()
        self.server = self.factory.server
        self.botNet = self.factory.botNet

        # Start picking an initial nickname. If this one is in use, we get
        # an ERR_NICKNAMEINUSE which we handle by picking the next name
        # from this generator and re-registering.
        self.initialNickGenerator = self.botNet.nickAllocator.generate()
        self.nickname = self.initialNickGenerator.next()
        irc.IRCClient.connectionMade(self)

    def irc_ERR_NICKNAMEINUSE(self, prefix, params):
        """An alternate nickname-in-use error handler that picks the next
           nick from our allocator instead of using those blasted underscores
           """
        self.register(self.initialNickGenerator.next())

    def nickChanged(self, newname):
        irc.IRCClient.nickChanged(self, newname)
        if not self.botNet.nickAllocator.isValid(newname):
            # We got a bad nick, try to find a better one
            log.msg("%r got an unsuitable nickname, trying to find a better one..." % self)
            self.findNick().addCallback(self.foundBetterNick)

    def register(self, nickname, hostname='foo', servername='bar'):
        """Twisted's default register() is silly in that it doesn't let us
           specify a new username. We want all the usernames to be the
           same, so filters can be written and things are just in general
           better when bots are renaming themselves dynamically.
           """
        if self.password is not None:
            self.sendLine("PASS %s" % self.password)
        self.setNick(nickname)
        self.sendLine("USER %s %s %s :%s" % (self.botNet.nickAllocator.username,
                                             hostname,
                                             servername,
                                             self.botNet.nickAllocator.realname))

    def foundBetterNick(self, nick):
        log.msg("%r found a better nick, renaming to %r" % (self, nick))
        self.setNick(nick)

    def findNickQuickly(self):
        """This is used to get an initial nick during registration, before
           we're allowed to make WHOIS queries. It only checks whether a nick
           is already in use by one of our bots. If we happened to grab a nick
           that's already in use, the server will rename us and our nickChanged()
           handler will try to find a better nick.
           """
        for nick in self.botNet.nickAllocator.generate():
            if not nick in self.botNet.servers.get(self.server, []):
                return nick

    def findNick(self):
        """Find a new unused nickname for this bot. As this requires
           testing whether each desired nickname is in use, it returns a Deferred.
           """
        result = defer.Deferred()
        self._findNick(True, None, self.botNet.nickAllocator.generate(), result)
        return result

    def _findNick(self, isUsed, testedNick, generator, result):
        """Callback implementing the guts of findNick.
           On the first call, isUsed is True and testedNick is None.
           We grab the next nick from the provided generator and
           start testing it, providing this function as the deferred's
           callback. If the nick isn't used, we send the nick to
           our result callback. Otherwise, the cycle continues.
           """
        if isUsed:
            nextNick = generator.next()
            self.isNickUsed(nextNick).addCallback(self._findNick, nextNick, generator, result)
        else:
            result.callback(testedNick)

    def isNickUsed(self, nick):
        """Determine if the given nick is in use, using WHOIS.
           Returns a Deferred that eventually resolves to a boolean.

           If the server doesn't respond to the WHOIS, we assume the nick
           isn't in use. This way if we're on a server that somehow has a broken
           WHOIS, we end up with an ugly nick rather than sitting in an infinite loop.
           """
        result = defer.Deferred()

        # First check whether any of our own bots are using this nick
        for bot in self.botNet.servers.get(self.server, []):
            if nick == bot.nickname:
                result.callback(True)
                return result

        # It's not that easy- try a WHOIS query
        result.setTimeout(30, result.callback, False)
        self.sendLine("WHOIS %s" % nick)
        self.currentWhoisDeferred = result
        return result

    def irc_RPL_WHOISUSER(self, prefix, params):
        """Reply to the WHOIS command we use to evaluate if a nick is used or not.
           This one would indicate that the nick is indeed used.
           """
        self.currentWhoisDeferred.callback(True)
        del self.currentWhoisDeferred

    def irc_ERR_NOSUCHNICK(self, prefix, params):
        """Reply to the WHOIS command we use to evaluate if a nick is used or not.
           This indicates that the nick is available.
           """
        self.currentWhoisDeferred.callback(False)
        del self.currentWhoisDeferred

    def emptyChannels(self):
        """Called when we know we're not in any channels and we shouldn't
           be trying to join any yet.
           """
        self.channels = []

        for timer in self.requestedChannels.itervalues():
            if timer.active():
                timer.cancel()
        self.requestedChannels = {}

    def signedOn(self):
        """IRCClient is notifying us that we've finished connecting to
           the IRC server and can finally start joining channels.
           """
        self.emptyChannels()

        # Important to check whether the nick we got was any good
        self.nickChanged(self.nickname)

        log.msg("%r connected" % self)
        self.botNet.botConnected(self)

    def connectionLost(self, reason):
        self.emptyChannels()
        log.msg("%r disconnected" % self)
        self.botNet.botDisconnected(self)
        irc.IRCClient.connectionLost(self)

    def join(self, channel):
        """Called by the bot's owner to request that a channel be joined.
           If this channel isn't already on our requests list, we send a join
           command and set up a timeout.
           """
        if channel not in self.requestedChannels and channel not in self.channels:
            self.requestedChannels[channel] = reactor.callLater(self.joinTimeout, self.joinTimedOut, channel)
            irc.IRCClient.join(self, channel)

    def cancelRequest(self, channel):
        """Cancels a request to join the given channel if we have one"""
        if channel in self.requestedChannels:
            timer = self.requestedChannels[channel]
            if timer.active():
                timer.cancel()
            del self.requestedChannels[channel]

    def joinTimedOut(self, channel):
        """Our join() timed out, remove the channel from our request list"""
        self.cancelRequest(channel)

    def part(self, channel):
        """Called to request that a bot leave the given channel.
           This removes the channel from our requests list if necessary
           before sending the part command.
           """
        self.cancelRequest(channel)
        irc.IRCClient.part(self, channel)

    def joined(self, channel):
        """A channel has successfully been joined"""
        log.msg("%r joined %r" % (self, channel))
        self.cancelRequest(channel)
        self.channels.append(channel)
        self.factory.botNet.botJoined(self, channel)

    def left(self, channel):
        """Called when part() is successful and we've left a channel.
           Implicitly also called when we're kicked via kickedFrom().
           """
        log.msg("%r left %r" % (self, channel))
        self.channels.remove(channel)
        self.factory.botNet.botLeft(self, channel)

    def kickedFrom(self, channel, kicker, message):
        log.msg("%r was kicked from %r by %r: %r" % (self, channel, kicker, message))
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