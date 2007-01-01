""" LibCIA.IRC.Bots

A small library for managing multiple IRC bots on multiple IRC networks.
The code in this module runs in a separate daemon, so other CIA components
can be restarted without effecting the bots.

"""
#
# CIA open source notification system
# Copyright (C) 2003-2007 Micah Dowty <micah@navi.cx>
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
from twisted.python import log, util
from twisted.spread import pb
import time, random, Queue
from LibCIA import TimeUtil
from LibCIA.IRC import Network


class Request(pb.Referenceable):
    """The Request object specifies a network, optionally a channel, and
       a number of bots that need to inhabit that network/channel.

       When a request is created, it registers itself with the BotNetwork,
       which then tries its hardest to keep the request satisfied. The
       bots satisfying this request are available in its 'bots' member.
       """
    def __init__(self, botNet, network, channel=None, numBots=1):
        self.bots = []
        self._active = True

        self.botNet = botNet
        self.network = network
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
        return "<Request for %s%s on %s>" % (botInfo, chanInfo, self.network)

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
        # Look for our network and channel in the map from networks to bot lists
        matches = []
        if self.network in self.botNet.networks:
            # Look for our channel in the map from channel names to bot lists
            for bot in self.botNet.networks[self.network]:
                if (not self.channel) or (self.channel in bot.channels):
                    matches.append(bot)

        # Ignore any bots we don't need
        self.bots = matches[:self.numBots]

    def isFulfilled(self):
        return len(self.bots) == self.numBots

    def getUserCount(self):
        """Return the number of users this request services directly.
           This only works with requests for a channel, as we can't really
           tell for channel requests. The returned number is the number of
           users in the channel minus the number of bots we're using.
           If the number of users can't be determined, this returns None.
           """
        if self.channel and self.bots:
            nicks = self.bots[0].channels[self.channel].nicks
            if nicks:
                return len(nicks) - len(self.bots)

    def remote_active(self):
        return self.active()

    def remote_cancel(self):
        self.cancel()

    def remote_getBots(self):
        return self.bots

    def remote_getNumBots(self):
        return self.numBots

    def remote_getBotNicks(self):
        return [bot.nickname for bot in self.bots]

    def remote_isFulfilled(self):
        return self.isFulfilled()

    def remote_getChannel(self):
        return self.channel

    def remote_getNetworkName(self):
        return str(self.network)

    def remote_getUserCount(self):
        return self.getUserCount()

    def remote_msg(self, target, line):
        """Use an arbitrary bot in this request to send a message to a
           supplied target (channel or nickname). Currently this always
           uses the first bot if possible and ignores the request if no
           bots are available. This may be a good place to implement
           smarter queueing and load balancing later. Currently bots
           implement their own anti-flood queues.
           """
        if self.bots:
            self.bots[0].queueMessage(target, line)

    def remote_msgList(self, target, lines):
        """Send multiple msg()es"""
        for line in lines:
            self.remote_msg(target, line)


class NickAllocator:
    """A nick allocator is responsible for determining what constitutes a valid nick,
       and generating a list of valid nicks. This is an abstract base class.
       """
    username = 'CIA'
    realname = 'CIA Bot (http://cia.navi.cx)'

    def isValid(self, nick):
        """Returns True if the given nickname would be a valid output for our generator"""
        return False

    def generate(self):
        """Generate a sequence of valid nicks, starting with the most preferable.
           This must be an infinite (or nearly infinite) generator.
           """
        pass


class SequentialNickAllocator(NickAllocator):
    """Generate sequentially numbered nicks starting with a given prefix"""
    def __init__(self, prefix):
        self.prefix = prefix

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
        i = 1
        while True:
            yield self.prefix + str(i)
            i += 1


class RandomAcronymNickAllocator(NickAllocator):
    """A nick allocator that generates random acronyms of a given length.
       No, it doesn't know what they mean yet :)
       """
    def __init__(self, length=3, alphabet="ABCDEFGHIJKLMNOPQRSTUVWXYZ"):
        self.length = length
        self.alphabet = alphabet

    def isValid(self, nick):
        if len(nick) != self.length:
            return False
        for letter in nick:
            if letter not in self.alphabet:
                return False
        return True

    def generate(self):
        while True:
            yield "".join([random.choice(self.alphabet) for i in xrange(self.length)])



class MessageLog:
    """A log that stores a fixed number of messages"""
    numMessages = 20

    def __init__(self):
        self.buffer = []

    def log(self, message):
        self.buffer = self.buffer[-self.numMessages:] + [message]


class BotNetwork(pb.Root):
    """A collection of IRC bots that work to satisfy a collection of Request objects.
       Users should interact with the BotNetwork via Request instances.
       """
    # In addition to checking bot status immediately after changes that
    # are likely to be important, we check bot status periodically, every
    # botCheckInterval seconds.
    botCheckInterval = 60

    # Bots are given this many seconds after being marked inactive before they're
    # disconnected. This way if a request is deleted then immediately replaced
    # with another that has similar requirements, we don't end up replacing
    # a bot we just deleted. I'm sure it has other good qualities too.
    botInactivityTimeout = 60 * 5

    # Maximum acceptable lag, in seconds. After this much the bot is disconnected
    # This should be a fairly large number, since the bots may experience a minute
    # or more of lag routinely when trying to join channels on connection.
    maximumLag = 60 * 3

    botCheckTimer = None

    def __init__(self, nickAllocator):
        self.nickAllocator = nickAllocator
        self.requests = []
        self.unknownMessageLog = MessageLog()

        # A map from Network to list of Bots
        self.networks = {}

        # A list of all networks for which bots are being created currently.
        # Maps from BaseNetwork instance to a DelayedCall signalling a timeout.
        self.newBotNetworks = {}

        # Lists all bots we're thinking about deleting due to inactivity.
        # Maps from Bot instance to a DelayedCall.
        self.inactiveBots = {}

        # Start the bot checking cycle
        self.checkBots()

    def findBot(self, host, nickname, port=None):
        """Find the bot currently on the given network with the given nick.
           This is mostly for use with the debug console. Note that for
           convenience, the network is specified as a host and port here.
           A BaseNetwork instance will be created.
           """
        network = Network.find(host, port)
        try:
            bots = self.networks[network]
        except KeyError:
            return None
        for bot in bots:
            if bot.nickname == nickname:
                return bot

    def findRequest(self, network, channel):
        """Find a request matching the given network, and channel"""
        for req in self.requests:
            if req.network == network and req.channel == channel:
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
                usedBots.setdefault(reqBot, util.InsensitiveDict())[request.channel] = True

                # Make sure that any referenced bots are no longer marked inactive
                if reqBot in self.inactiveBots:
                    timer = self.inactiveBots[reqBot]
                    if timer.active():
                        timer.cancel()
                    del self.inactiveBots[reqBot]

        # Now look for unused bots and/or channels
        for network in self.networks.itervalues():
            for bot in network:
                if bot in usedBots:
                    usedChannels = usedBots[bot]

                    # We need this bot.. but are all the channels necessary?
                    for channel in bot.channels.iterkeys():
                        if channel not in usedChannels:
                            bot.part(channel)

                    # Since we need this bot, make sure it's still responsive. If its lag
                    # is too high, force it to give up. IF we have to disconnect the bot,
                    # give up this checkBots() and start over when botDisconnected() calls
                    # us again.
                    if bot.getLag() > self.maximumLag:
                        bot.quit()
                        self.botDisconnected(bot)
                        return

                else:
                    # We don't need this bot. Tell it to part all of its channels,
                    # and if it isn't already, schedule it for deletion.
                    for channel in bot.channels.iterkeys():
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
        if request.channel and request.network in self.networks:
            for bot in self.networks[request.network]:
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

        # Nope... how about asking more bots to join the request's network?
        if not request.network in self.newBotNetworks:
            self.createBot(request.network)

    def createBot(self, network):
        """Create a new bot for the given network, retrying if necessary"""
        # We're not already trying to connect a bot, or our previous attempt failed.
        # Start trying to connect a bot, and set a timeout.
        log.msg("Creating a new IRC bot for %s" % network)
        BotFactory(self, network)
        self.newBotNetworks[network] = reactor.callLater(network.newBotTimeout, self.newBotTimedOut, network)

    def newBotTimedOut(self, network):
        """We just timed out waiting for a new bot connection. Try again."""
        log.msg("Timed out waiting for an IRC bot to connect to %s" % network)
        del self.newBotNetworks[network]
        # Don't immediately assume that we need to try again, but give us a chance to check
        self.checkBots()

    def botInactivityCallback(self, bot):
        """Bots that have been unused for a while eventually end up here, and are disconnected"""
        log.msg("Disconnecting inactive bot %r" % bot)
        bot.quit()

    def botConnected(self, bot):
        """Called by a bot when it has been successfully connected."""
        self.networks.setdefault(bot.network, []).append(bot)

        try:
            timer = self.newBotNetworks[bot.network]
            if timer.active():
                timer.cancel()
            del self.newBotNetworks[bot.network]
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
            self.networks[bot.network].remove(bot)
            if not self.networks[bot.network]:
                del self.networks[bot.network]
        except:
            # The bot might have not been in our network list in the first
            # place, if it got disconnected before becoming fully connected
            # or if its disconnection gets detected in multiple ways (socket
            # closed, ping timeout, etc)
            pass

        # The maximum lag autodisconnect code in checkBots relies on this being
        # called whether or not we actually removed the bot- the above 'pass'
        # can not be safely replaced with 'return'.
        self.checkBots()

    def botJoined(self, bot, channel):
        self.checkBots()

    def botLeft(self, bot, channel):
        self.checkBots()

    def botKicked(self, bot, channel, kicker, message):
        # FIXME: remove the ruleset
        pass

    def remote_findRequest(self, host, port, channel, create=True):
        """Find and return a request, optionally creating it if necessary"""
        network = Network.find(host, port)
        r = self.findRequest(network, channel)
        if r:
            return r
        elif create:
            return Request(self, network, channel)
        else:
            return None

    def remote_getRequests(self):
        """Return a dictionary mapping (host, port, channel) to request instances"""
        d = {}
        for request in self.requests:
            d[ request.network.getIdentity() + (request.channel,) ] = request
        return d

    def remote_getTotals(self):
        """Return a dictionary of impressive-looking totals related to the bots"""
        totals = dict(
            networks = 0,
            bots = 0,
            channels = 0,
            users = 0,
            requests = 0,
            unfulfilled = 0,
            )

        for network in self.networks.iterkeys():
            totals['networks'] += 1
            for bot in self.networks[network]:
                totals['bots'] += 1
                totals['channels'] += len(bot.channels)

        for request in self.requests:
            totals['requests'] += 1
            totals['users'] += request.getUserCount() or 0
            if not request.isFulfilled():
                totals['unfulfilled'] += 1

        return totals

    def remote_getBots(self):
        bots = []
        for networkBots in self.networks.itervalues():
            bots.extend(networkBots)
        return bots

    def remote_getMessageLog(self):
        return self.unknownMessageLog.buffer

    def remote_getNewBots(self):
        """Return a list of bots that are currently trying to
           connect, represented as (network, deadline) tuples.
           """
        newBots = []
        for network, timer in self.newBotNetworks.iteritems():
            newBots.append((str(network), timer.getTime()))
        return newBots


class ChannelInfo:
    """A container for information about an IRC channel. Currently
       this just holds things we will be told without asking, like the
       channel's occupants and its topic.
       """
    def __init__(self, name):
        self.name = name
        self.nicks = []
        self.topic = None

        # This list collects names mentioned in an RPL_NAMREPLY.
        # Upon receiving an RPL_ENDOFNAMES this is transferred to
        # 'nicks' and replaced with a new empty list.
        self.nickCollector = []


class MessageQueue:
    """A single message FIFO with a fixed maximum size. If messages
       are dropped, this emits a warning at the proper time.

       Normally, this acts just like a bare Queue object, except that
       it never blocks. Once it fills up, each new message increments
       a 'dropped' counter.

       Logically, the dropped messages will always be at the back of
       the queue. After the queue empties, a new message is added
       indicating how many lines were dropped. Normal queueing resumes
       after this.
       """
    def __init__(self, maxSize):
        self._fifo = Queue.Queue(maxSize)
        self._numDropped = 0
        self._next = None

    def put(self, message):
        """Push a new message onto the Queue. Never blocks, but it
           may drop messages if the queue is full. The message
           should never be None.
           """
        if self._numDropped:
            self._numDropped += 1
        else:
            try:
                self._fifo.put(message, False)
            except Queue.Full:
                self._numDropped = 1

    def get(self):
        """Return the next message, discarding it from the queue."""
        try:
            return self._fifo.get(False)
        except Queue.Empty:
            if self._numDropped:
                n = self._numDropped
                self._numDropped = 0
                return "(%d lines omitted)" % n


class FairQueue:
    """A FairQueue tracks pending messages to any number of targets,
       each of which own their own independent queue. This prevents
       floods to one target from affecting timely delivery to other
       targets. All queues are checked in round-robin order.
       """
    def __init__(self, queueSize):
        self._queueSize = queueSize
        self._targetDict = {}
        self._pendingQueue = Queue.Queue()
        self._next = None

    def put(self, target, message):
        """Queue up a new message to the supplied target"""
        if target not in self._targetDict:
            queue = MessageQueue(self._queueSize)
            self._targetDict[target] = queue
            self._pendingQueue.put(target, False)
        else:
            queue = self._targetDict[target]
        queue.put(message)

    def peek(self):
        """Return the next message to be processed, without discarding
           it. A subsequent call to peek() will return the same message.
           Returns a (target, message) tuple if a message is available,
           or None if all queues are empty.
           """
        if self._next is None:
            while 1:
                # Get the next queue target in round-robin order
                try:
                    target = self._pendingQueue.get(False)
                except Queue.Empty:
                    break

                queue = self._targetDict[target]
                message = queue.get()

                if message:
                    # We got a message. Reschedule this queue for later.
                    self._next = (target, message)
                    self._pendingQueue.put(target, False)
                    break

                else:
                    # This queue is empty. Discard it.
                    del self._targetDict[target]

        return self._next

    def flush(self):
        """Discard the last message returned by peek()"""
        self._next = None


class Bot(irc.IRCClient, pb.Referenceable):
    """An IRC bot connected to one network any any number of channels,
       sending messages on behalf of the BotController.

       The Bot class is responsible for keeping track of the timers and
       limits associated with joining channels, but it doesn't map itself
       onto Requests, nor does it manage bot connection and disconnection.
       """
    # Timeout, in seconds, for joining channels
    joinTimeout = 60

    # Important timestamps
    lastPingTimestamp = None
    lastPongTimestamp = None
    signonTimestamp = None
    lastPingTransmitTimestamp = None

    # Byte counters. We maintain a perpetually incrementing byte counter,
    # tracking the amount of data sent over the life of the connection.
    # This byte counter is sent with PING, and we record the value returned
    # with the last PONG. Taking (txByteCount - txConfirmedBytes) gives us
    # the number of bytes transmitted but not confirmed: the maximum amount
    # of data the server may be buffering on our behalf. This buffer
    # level must be controlled, to avoid being kicked for flooding.
    txByteCount = 0
    txConfirmedBytes = 0

    # Maximum number of lines to queue per target (channel/user)
    maxQueueSize = 20

    # Unhandled commands to ignore, rather than log
    ignoredCommands = [
        "ERR_NOCHANMODES",     # Freenode spamming us to register
        506,                   # PLD spamming us to register
        333,                   # Freenode sends these with channel registration info
        ]

    def __init__(self):
        self.emptyChannels()
        self._messageQueue = FairQueue(self.maxQueueSize)
        self.pendingWhoisTests = {}
        self.connectTimestamp = None

    def emptyChannels(self):
        """Called when we know we're not in any channels and we shouldn't
           be trying to join any yet.
           """
        # Map from channel name to ChannelInfo instance. This only holds
        # channels we're actually in, not those we've been asked to join
        # but aren't in yet.
        self.channels = util.InsensitiveDict()

        # clear stale timers
        if hasattr(self, 'requestedChannels'):
            for timer in self.requestedChannels.itervalues():
                if timer.active():
                    timer.cancel()

        # A map from channel name to a DelayedCall instance representing its timeout
        self.requestedChannels = util.InsensitiveDict()

    def __repr__(self):
        return "<Bot %r on network %s>" % (self.nickname, self.network)

    def isFull(self):
        return len(self.channels) + len(self.requestedChannels) >= self.network.maxChannels

    def connectionMade(self):
        """Called by IRCClient when we have a socket connection to the server."""
        self.emptyChannels()
        self.connectTimestamp = time.time()
        self.network = self.factory.network
        self.botNet = self.factory.botNet

        # Start picking an initial nickname. This is really only expected to work
        # on servers where this is the only CIA bot. If this one is in use, we get
        # an ERR_NICKNAMEINUSE which we handle by picking a temporary nick we can
        # use to search for a better one.
        self.nickname = self.findNickQuickly()
        self.nicknames = [self.nickname]
        irc.IRCClient.connectionMade(self)

    def irc_ERR_NICKNAMEINUSE(self, prefix, params):
        """An alternate nickname-in-use error handler that generates a random
           temporary nick. This will let us at least connect to the server and
           issue WHOIS queries to efficiently find a better nick.

           As soon as we get the nickChanged back from this operation, we will
           realize this nick doesn't match those allowed by nickAllocator and
           start looking for a better one.
           """
        tempNick = "CIA-temp%03d" % random.randint(0, 999)
        self.nicknames.append(tempNick)
        self.setNick(tempNick)

    def sendLine(self, line):
        # Override sendLine() to update txByteCount.
        # Note that the text of 'line' doesn't count a CRLF,
        # but we should include that in our buffer estimates.
        self.txByteCount += len(line) + 2
        irc.IRCClient.sendLine(self, line)

    def nickChanged(self, newname):
        irc.IRCClient.nickChanged(self, newname)
        if self.botNet.nickAllocator.isValid(newname):
            # The nick was valid. If we aren't completely connected yet, fix that
            if self.signonTimestamp is None:
                self.finishConnection()
        else:
            # We got a bad nick, try to find a better one. If it doesn't
            # work within 1 minute, self-destruct.
            log.msg("%r starting nick negotiation" % self)
            reactor.callLater(60, self.enforceNickDeadline)
            self.findNick().addCallback(self.foundBetterNick)

    def enforceNickDeadline(self):
        """If this bot doesn't have a valid nick yet, kill it."""
        if not self.botNet.nickAllocator.isValid(self.nickname):
            self.sendLine("QUIT")

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
        self.nicknames.append(nick)
        self.setNick(nick)

    def findNickQuickly(self):
        """This is used to get an initial nick during registration, before
           we're allowed to make WHOIS queries. It only checks whether a nick
           is already in use by one of our bots. If we happened to grab a nick
           that's already in use, the server will rename us and our nickChanged()
           handler will try to find a better nick.
           """
        for nick in self.botNet.nickAllocator.generate():
            if not nick in self.botNet.networks.get(self.network, []):
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
        for bot in self.botNet.networks.get(self.network, []):
            if nick == bot.nickname:
                result.callback(True)
                return result

        # It's not that easy- try a WHOIS query
        if nick in self.pendingWhoisTests:
            # We already have a request on the wire, tack another deferred onto it
            self.pendingWhoisTests[nick].append(result)

        else:
            # Send a new request
            self.pendingWhoisTests[nick] = [result]
            self.sendLine("WHOIS %s" % nick)

        return result

    def irc_RPL_WHOISUSER(self, prefix, params):
        """Reply to the WHOIS command we use to evaluate if a nick is used or not.
           This one would indicate that the nick is indeed used.
           """
        nick = params[1]
        if nick in self.pendingWhoisTests:
            for result in self.pendingWhoisTests[nick]:
                result.callback(True)
            del self.pendingWhoisTests[nick]

    def irc_ERR_NOSUCHNICK(self, prefix, params):
        """Reply to the WHOIS command we use to evaluate if a nick is used or not.
           This indicates that the nick is available.
           """
        nick = params[1]
        if nick in self.pendingWhoisTests:
            for result in self.pendingWhoisTests[nick]:
                result.callback(False)
            del self.pendingWhoisTests[nick]

    # These are several other WHOIS replies that we want to ignore
    def irc_RPL_WHOISSERVER(self, prefix, params):
        pass
    def irc_RPL_WHOISIDLE(self, prefix, params):
        pass
    def irc_RPL_WHOISCHANNELS(self, prefix, params):
        pass
    def irc_RPL_ENDOFWHOIS(self, prefix, params):
        pass

    def signedOn(self):
        """IRCClient is notifying us that we've finished connecting to
           the IRC server and can finally start joining channels.
           """
        self.emptyChannels()

        # Check our initial nick, finish our connection if it was good
        self.nickChanged(self.nickname)

    def finishConnection(self):
        log.msg("%r connected" % self)
        self.botNet.botConnected(self)
        self.signonTimestamp = time.time()

        # Start the cycle of pinging the server to ensure our connection
        # is still up and measure lag. IRC servers seem to often fail in
        # ways that leave clients' sockets connected but ignore all data
        # from them, and this lets us measure lag for free.
        self._lagPingLoop()

    def sendServerPing(self):
        """Send a ping stamped with the current time and byte count"""
        self.lastPingTransmitTimestamp = time.time()
        self.sendLine("PING %s-%f" % (self.txByteCount, self.lastPingTransmitTimestamp))

    def _lagPingLoop(self):
        self.sendServerPing()
        reactor.callLater(self.network.pingInterval, self._lagPingLoop)

    def irc_PONG(self, prefix, params):
        """Handle the responses to pings sent with sendServerPing. This compares
           the timestamp in the pong (from when the ping was sent) and the current
           time, storing the lag and the current time.
           """
        try:
            # Most IRC servers send back a server name as params[0] then the
            # ping argument as params[1].. but some (broken?) ones send back
            # only a single argument, with the ping parameter.
            if len(params) >= 2:
                pingParam = params[1]
            else:
                pingParam = params[0]

            byteCount, timestamp = pingParam.split("-")
            self.txConfirmedBytes = int(byteCount)
            self.lastPingTimestamp = float(timestamp)

        except (ValueError, IndexError):
            # This must be some broken IRC server that's not preserving our ping timestamp.
            # The best we can do is assume this is the pong for the most recent ping we sent.
            log.msg("%r received bad PONG reply: %r" % (self, params))
            self.lastPingTimestamp = self.lastPingTransmitTimestamp
            self.txConfirmedBytes = self.txByteCount

        now = time.time()
        self.lastPongTimestamp = now
        self._pollMessageQueue(now)

    def connectionLost(self, reason):
        self.emptyChannels()
        log.msg("%r disconnected" % self)
        self.botNet.botDisconnected(self)
        irc.IRCClient.connectionLost(self)

    def getLag(self):
        """Calculate a single figure for the lag between us and the server.
           If pings have been coming back on time this is just the raw lag,
           but if our latest ping has been particularly late, it's the average
           of the latest successful ping's lag and the amount of time we've been
           waiting for this late ping.
           """
        if self.lastPongTimestamp is None:
            # We've never received a successful pong
            if self.signonTimestamp is None:
                # Hmm, we've also never signed on. Nothing more we can do
                return None
            else:
                # We've signed on, but never received a good pong. Let's pretend
                # the time since the last pong is just the time since signon- if
                # we really have that funky of a connection this will at least eventually
                # let us detect that.
                timeSincePong = time.time() - self.signonTimestamp
            lag = None
        else:
            timeSincePong = time.time() - self.lastPongTimestamp
            if self.lastPingTimestamp is None:
                lag = None
            else:
                lag = self.lastPongTimestamp - self.lastPingTimestamp

        if timeSincePong < self.network.pingInterval * 2:
            # We're doing fine, report the raw lag
            return lag
        else:
            # Yikes, it's been a while since we've had a good pong.
            # Weigh that in to the returned lag figure as described above.
            return ((lag or 0) + (timeSincePong - self.network.pingInterval)) / 2

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
        self.channels[channel] = ChannelInfo(channel)
        self.factory.botNet.botJoined(self, channel)

    def left(self, channel):
        """Called when part() is successful and we've left a channel.
           Implicitly also called when we're kicked via kickedFrom().
           """
        log.msg("%r left %r" % (self, channel))
        del self.channels[channel]
        self.factory.botNet.botLeft(self, channel)

    def kickedFrom(self, channel, kicker, message):
        log.msg("%r was kicked from %r by %r: %r" % (self, channel, kicker, message))
        self.left(channel)
        self.factory.botNet.botKicked(self, channel, kicker, message)

    def ctcpUnknownQuery(self, user, channel, tag, data):
        """Ignore unknown queries, so if someone sends a CTCP BAGEL to
           the channel CIA doesn't respond needlessly.
           """
        pass

    def irc_unknown(self, prefix, command, params):
        """Log unknown commands, making debugging easier. This also lets
           us see responses in the log for commands sent via debug_tool.
           """
        if command in self.ignoredCommands:
            return

        log.msg("%r received unknown IRC command %s: %r" % (self, command, params))

        self.factory.botNet.unknownMessageLog.log((
            time.time(),
            self.nickname,
            str(self.network),
            command,
            repr(params)[1:-1],
            ))

    def topicUpdated(self, user, channel, newTopic):
        self.channels[channel].topic = newTopic

    def irc_JOIN(self, prefix, params):
        """This is a modified implementation that checks the nick against
           both our current nickname and our previous one. This hopefully
           avoids a race condition when we're joining a channel and changing
           our nick at nearly the same time.
           """
        nick = prefix.split('!')[0]
        channel = params[-1]
        if nick in self.nicknames:
            self.joined(channel)
        else:
            self.userJoined(nick, channel)

    def irc_NICK(self, prefix, params):
        """This is a modified implementation that checks the nick against
           both our current nickname and our previous one. This ensures
           that we get confirmation for our own nick changes.
           """
        nick = prefix.split('!')[0]
        if nick in self.nicknames:
            self.nickChanged(params[0])
        else:
            self.userRenamed(nick, params[0])

    def irc_RPL_NAMREPLY(self, prefix, params):
        """Collect usernames from this channel. Several of these
           messages may be sent to cover the channel's full nicklist.
           An RPL_ENDOFNAMES signals the end of the list.
           """
        # We just separate these into individual nicks and stuff them in
        # the nickCollector, transferred to 'nicks' when we get the RPL_ENDOFNAMES.
        channel = self.channels[params[2]]
        for name in params[3].split():
            # Remove operator and voice prefixes
            if name[0] in '@+':
                name = name[1:]
            channel.nickCollector.append(name)

    def irc_RPL_ENDOFNAMES(self, prefix, params):
        """This is sent after zero or more RPL_NAMREPLY commands to
           terminate the list of users in a channel.
           """
        channel = self.channels[params[1]]
        channel.nicks = channel.nickCollector
        channel.nickCollector = []

    def userJoined(self, user, channel):
        """Update the channel's nick list when we see someone join"""
        self.channels[channel].nicks.append(user)

    def irc_QUIT(self, prefix, params):
        # Another user quit, remove them from any of our channel lists
        nick = prefix.split('!')[0]
        for channel in self.channels.itervalues():
            try:
                channel.nicks.remove(nick)
            except ValueError:
                pass

    def userLeft(self, user, channel):
        """Update the channel's nick list when a user voluntarily leaves"""
        self.channels[channel].nicks.remove(user)

    def userKicked(self, user, channel, kicker, message):
        """Update the channel's nick list when a user is kicked"""
        self.channels[channel].nicks.remove(user)

    def userRenamed(self, oldname, newname):
        # Blah, this doesn't give us a channel name. Search for this user
        # in each of our channels, renaming them.
        for channel in self.channels.itervalues():
            try:
                channel.nicks.remove(oldname)
                channel.nicks.append(newname)
            except ValueError:
                pass

    def queueMessage(self, target, text):
        """A msg() workalike which queues messages and provides flood protection.
           Text sent with queueMessage() isn't guaranteed to ever be sent to the
           server.
           """
        self._messageQueue.put(target, text)
        self._pollMessageQueue(time.time())

    def _pollMessageQueue(self, now):
        """Check whether it's safe to send any queued messages"""
        while self._messageQueue.peek():
            target, text = self._messageQueue.peek()

            # Length estimate: Includes the "PRIVMSG %s :%s" boilerplate, plus the CRLF
            length = len(text) + len(target) + 12

            # Make a worst-case prediction of the server's buffer fill level after we
            # would have sent this string.
            predictedFill = self.txByteCount - self.txConfirmedBytes

            # If we're up to half our buffer fill and we haven't sent a ping
            # recently, send another in an attempt to lower the predicted
            # fill estimate.
            if predictedFill > self.network.bufferSize / 2 and self.lastPingTransmitTimestamp + 5 < now:
                self.sendServerPing()

            # If we'd go above the hard limit, we can't send the message.
            if predictedFill > self.network.bufferSize:
                break
            
            self.msg(target, text)
            self._messageQueue.flush()

    def action(self, user, channel, message):
        """Just for fun"""
        text = message.lower().strip()
        me = self.nickname.lower()
        them = user.split('!')[0]

        if text == 'hugs %s' % me:
            self.me(channel, 'hugs %s' % them)

        elif text == 'kicks %s' % me:
            self.say(channel, 'ow')
        
        elif text == 'kills %s' % me:
            self.me(channel, 'dies')

        elif text == 'eats %s' % me:
            self.me(channel, 'tastes crunchy')

        elif text == "rubs %s's tummy" % me:
	    self.say(channel, "*purr*")

    def remote_msg(self, target, text):
        """A remote request directly to this bot, ignoring the usual queueing"""
        self.msg(target, text)

    def remote_getNickname(self):
        return self.nickname

    def remote_getChannels(self):
        return self.channels.keys()

    def remote_getRequestedChannels(self):
        # Convert from an InsensitiveDict to a normal one
        return self.requestedChannels.keys()

    def remote_getNetworkInfo(self):
        """Returns a (networkName, host, port) tuple"""
        host, port = self.transport.addr
        return (str(self.network), host, port)

    def remote_repr(self):
        return repr(self)

    def remote_getConnectTimestamp(self):
        return self.connectTimestamp

    def remote_getInactivity(self):
        """If this bot is inactive, returns the time at which it will be garbage
           collected. If not, returns None.
           """
        if self in self.botNet.inactiveBots:
            return self.botNet.inactiveBots[self].getTime()

    def remote_isFull(self):
        return self.isFull()

    def remote_isEmpty(self):
        return (not self.channels) and (not self.requestedChannels)

    def remote_getStatusText(self):
        """Get a textual description of this bot's status"""
        indicators = []

        if self.remote_isFull():
            indicators.append('full')

        if self.remote_isEmpty():
            indicators.append('empty')

        timer = self.remote_getInactivity()
        if timer:
            indicators.append('GC in %s' % TimeUtil.formatDuration(timer - time.time()))

        return ', '.join(indicators)

    def remote_getLag(self):
        return self.getLag()


class BotFactory(protocol.ClientFactory):
    """Twisted ClientFactory for creating Bot instances"""
    protocol = Bot

    def __init__(self, botNet, network):
        self.botNet = botNet
        self.network = network
        self.network.connect(self)

    def clientConnectionLost(self, connector, reason):
        log.msg("IRC Connection to %r lost: %r" % (self.network, reason))

    def clientConnectionFailed(self, connector, reason):
        log.msg("IRC Connection to %r failed: %r" % (self.network, reason))

### The End ###
