""" LibCIA.IRC

Code for managing IRC bots, formatting messages for IRC, and
a handler for the 'irc' URI scheme.
"""
#
# CIA open source notification system
# Copyright (C) 2003-2004 Micah Dowty <micahjd@users.sourceforge.net>
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
from twisted.python import log
import XML, ColorText, Ruleset, Message
import types


class IrcURIHandler(Ruleset.RegexURIHandler):
    """Handles irc:// URIs in rulesets. This is responsible for
       keeping a BotNetwork in sync with the channels and servers
       required of us by the rulesets, and dispatching completed
       messages to the BotNetwork.
       """
    scheme = 'irc'
    regex = r"""
       ^irc://(?P<host>[a-zA-Z]([a-zA-Z0-9.-]*[a-zA-Z0-9])?)
       (:(?P<port>[0-9]+))?/(?P<channel>[^\s#]\S*)$
       """

    def __init__(self, botNet):
        # A reverse map for the result of uriToTuple,
        # so a (server,channel) tuple can be uniquely
        # converted back to the URI that generated it.
        # This also lets assigned() raise an exception
        # if someone tries to add a second URI that refers
        # to a server and channel we're already in.
        self.tupleToUriMap = {}

        self.botNet = botNet
        Ruleset.RegexURIHandler.__init__(self)

    def uriToTuple(self, uri):
        """On top of the regex matching done by our superclass, separate
           the URI into host, port, and channel.
           We apply the default port number if the URI doesn't have one,
           and prepend a '#' to the channel.
           """
        d = self.parseURI(uri)
        host, port, channel = d['host'], d['port'], d['channel']
        if not port:
            port = 6667
        return ((host, port), '#' + channel)

    def assigned(self, uri, newRuleset):
        t = self.uriToTuple(uri)
        if self.tupleToUriMap.has_key(t) and self.tupleToUriMap[t] != uri:
            raise Ruleset.InvalidURIException("Another URI referring to the same server and channel already exists")
        self.tupleToUriMap[t] = uri
        self.botNet.addChannel(*t)

    def unassigned(self, uri):
        t = self.uriToTuple(uri)
        self.botNet.delChannel(*t)
        del self.tupleToUriMap[t]

    def message(self, uri, message, content):
        server, channel = self.uriToTuple(uri)
        self.botNet.msg(server, channel, content)


class Bot(irc.IRCClient):
    """An IRC bot connected to one server any any number of channels,
       sending messages on behalf of the BotController.
       """
    def connectionMade(self):
        """Called by IRCClient when we have a socket connection to the server.
           This allocates the nick we'd like to use.
           """
        self.emptyChannels()
        self.nickname = self.factory.allocator.allocateNick()
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
        self.factory.allocator.botConnected(self)

    def connectionLost(self, reason):
        self.factory.allocator.botDisconnected(self)
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
        self.factory.allocator.botJoined(self, channel)

    def left(self, channel):
        """Called when part() is successful and we've left a channel.
           Implicitly also called when we're kicked via kickedFrom().
           """
        # If we no longer are in any channels, turn off
        # automatic reconnect so we can quit without being reconnected.
        self.channels.remove(channel)
        if not self.channels:
            self.factory.reconnect = False
        self.factory.allocator.botLeft(self, channel)

    def kickedFrom(self, channel, kicker, message):
        self.left(channel)
        self.factory.allocator.botKicked(self, channel, kicker, message)


class BotFactory(protocol.ClientFactory):
    """Twisted ClientFactory for creating Bot instances"""
    protocol = Bot

    def __init__(self, allocator):
        self.allocator = allocator
        self.reconnect = True
        log.msg("Connecting a new bot to host %r, port %r" % (allocator.host, allocator.port))
        reactor.connectTCP(allocator.host, allocator.port, self)

    def clientConnectionLost(self, connector, reason):
        """There was a connection, but we lost it. This usually happens because
           we flooded off, or there was a network problem. Try to reconnect right away.
           Note that this only handles reconnecting- in the meantime, we probably will
           have already reassigned this bot's channels to another bot in the allocator's
           botDisconnected function.
           """
        log.msg("Connection to %r lost: %r" % (connector.getDestination(), reason))
        if self.reconnect:
            log.msg("Reconnecting...")
            connector.connect()
        else:
            log.msg("Not reconnecting")

    def clientConnectionFailed(self, connector, reason):
        """Our connection attempt has failed. The server we were given could be
           nonexistant, in which case it would be nice to remove whatever it is that's
           telling us to connect, but it could also be a temporary error. We can't
           decide that here, so just schedule a reconnect later.
           """
        log.msg("Connection to %r failed: %r" % (connector.getDestination(), reason))
        if self.allocator.newBotRequests:
            reconnectMinutes = 10
            log.msg("Looks like this bot is still needed, trying again in %s minutes" % reconnectMinutes)
            reactor.callLater(60 * reconnectMinutes, connector.connect)
        else:
            log.msg("Not reconnecting")


class BotAllocator:
    """Manages bot allocation for one IRC server"""
    def __init__(self, server, nickFormat, channelsPerBot=18):
        self.nickFormat = nickFormat
        self.channelsPerBot = channelsPerBot
        self.host, self.port = server
        self.kickCallback = None

        # Map nicknames to bot instances
        self.bots = {}

        # Map channel names to bots
        self.channels = {}

        # List of channels we want a new bot to join.
        # If this is non-empty, we should already have
        # a new bot in the process of connecting.
        self.newBotRequests = []

    def botConnected(self, bot):
        """Called by one of our bots when it's connected successfully and ready to join channels"""
        log.msg("Bot %r on server %r connected" % (bot.nickname, self.host))
        self.bots[bot.nickname] = bot

        # Join as many channels as this bot is allowed to,
        # transferring those channels from newBotRequests to the bot's
        # individual requestedChannels list.
        for channel in self.newBotRequests[:self.channelsPerBot]:
            log.msg("Requesting that new bot %r on server %r join %r" % (bot.nickname, self.host, channel))
            self.newBotRequests.remove(channel)
            bot.join(channel)

        # If we still have channels that need joining that
        # go beyond this new bot's capabilities, start another new bot.
        if self.newBotRequests:
            log.msg("Still need another bot, creating one...")
            BotFactory(self)

    def botJoined(self, bot, channel):
        """Called by one of our bots when it's successfully joined to a channel"""
        log.msg("Bot %r on server %r joined %r" % (bot.nickname, self.host, channel))
        self.channels[channel] = bot

    def botLeft(self, bot, channel):
        """Called by one of our bots when it has finished leaving a channel"""
        log.msg("Bot %r on server %r left %r" % (bot.nickname, self.host, channel))
        del self.channels[channel]

        # If there's no reason to keep this bot around any more, disconnect it
        if not (bot.channels or bot.requestedChannels):
            bot.quit()

    def botKicked(self, bot, channel, kicker, message):
        """Apparently we aren't wanted any longer. Log the event and report to our owner"""
        log.msg("Bot %r on server %r was kicked from %r by %r: %r" %
                (bot.nickname, self.host, channel, kicker, message))
        if self.kickCallback:
            self.kickCallback((self.host, self.port), channel, kicker, message)

    def botDisconnected(self, bot):
        """Called when one of our bots has been disconnected"""
        log.msg("Bot %r on server %r disconnected" % (bot.nickname, self.host))
        del self.bots[bot.nickname]

        # Delete any channels in self.channels that are pointed at this bot still
        for channel in self.channels.keys():
            if self.channels[channel] == bot:
                del self.channels[channel]

        # Reallocate any channels still serviced or requested by this bot
        for channel in bot.channels:
            self.addChannel(channel)
        for channel in bot.requestedChannels:
            self.addChannel(channel)

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
        if channel in self.newBotRequests:
            return None
        for bot in self.bots.itervalues():
            if channel in bot.requestedChannels:
                return None

        # Check for room in our existing bots
        for bot in self.bots.itervalues():
            if len(bot.channels) + len(bot.requestedChannels) < self.channelsPerBot:
                log.msg("Requesting that bot %r on server %r join %r" % (bot.nickname, self.host, channel))
                bot.join(channel)
                return None

        # We'll have to get a new bot to join this channel.
        # Add it to the list of channels for a new bot to join, and
        # create that new bot if we're the first
        self.newBotRequests.append(channel)
        if len(self.newBotRequests) == 1:
            log.msg("Creating a new bot on server %r" % self.host)
            BotFactory(self)

    def delChannel(self, channel):
        """Remove a channel from the list of those supported by the bots
           on this server, deleting any bots no longer necessary.
           """
        # Remove it from our requested channel lists if it's there
        try:
            self.newBotRequests.remove(channel)
            log.msg("Removing channel %r on server %r from newBotRequests" % (channel, self.host))
        except:
            pass
        for bot in self.bots.itervalues():
            try:
                bot.requestedChannels.remove(channel)
                log.msg("Removing channel %r on server %r from the requestedChannels for %r" %
                        (channel, self.host, bot.nickname))
            except:
                pass

        # If we're already connected to this channel, leave it
        if self.channels.has_key(channel):
            bot = self.channels[channel]
            log.msg("Requesting that bot %r on server %r leave %r" % (bot.nickname, self.host, channel))
            bot.leave(channel)

    def msg(self, channel, text):
        """Send a message to the specified channel, using whichever
           bot is appropriate.
           """
        self.channels[channel].msg(channel, text)


class BotNetwork:
    """A system of any number of bots on any number of IRC messages
       capable of announcing messages.

       nickFormat is a format string containing a %d, used to generate
       nicknames for the bots.
       """
    def __init__(self, nickFormat):
        self.nickFormat = nickFormat

        # A map from (host, port) to BotAllocator instances
        self.servers = {}

        # An optional callback to be called when a bot is kicked, with
        # (server, channel, kicker, message) as arguments.
        self.kickCallback = None

    def getServers(self):
        """Return a list of (host, port) tuples specifying all servers we know about"""
        return self.servers.keys()

    def getChannels(self, server):
        """Return the list of channels we're in on the given server"""
        return self.servers[tuple(server)].channels.keys()

    def getBots(self, server):
        """Return a list of all bot nicknames on the given server"""
        return self.servers[tuple(server)].bots.keys()

    def getBotChannels(self, server, bot):
        """Return a list of all the channels a particular bot is in, given its server and nickname"""
        return self.servers[tuple(server)].bots[bot].channels

    def addChannel(self, server, channel):
        """Add a new server/channel to the list supported by our bots.
           New bots are automatically started as needed.
           """
        server = tuple(server)
        if not self.servers.has_key(server):
            allocator = BotAllocator(server, nickFormat=self.nickFormat)
            allocator.kickCallback = self.kickCallback
            self.servers[server] = allocator
        self.servers[server].addChannel(channel)

    def delChannel(self, server, channel):
        """Remove a server/channel to the list supported by our bots.
           New bots are automatically deleted as they are no longer needed.
           """
        try:
            server = tuple(server)
            self.servers[server].delChannel(channel)
            if not self.servers[server].channels:
                del self.servers[server]
        except KeyError:
            # We weren't in this channel to begin with, ignore it
            pass

    def msg(self, server, channel, text):
        """Send text to the given channel on the given server.
           This will generate an exception if the server and/or
           channel isn't currently occupied by one of our bots.
           Multiple lines of text are split into multiple IRC messages.
           """
        for line in text.split("\n"):
            self.servers[tuple(server)].msg(channel, line)


class FormattingCode(object):
    """Represents a code used to format text in IRC"""
    codes = {
        # Colors
        "black"       : "\x0301",
        "dark blue"   : "\x0302",
        "dark green"  : "\x0303",
        "green"       : "\x0303",
        "red"         : "\x0304",
        "light red"   : "\x0304",
        "dark red"    : "\x0305",
        "purple"      : "\x0306",
        "brown"       : "\x0307",  # On some clients this is orange, others it is brown
        "orange"      : "\x0307",
        "yellow"      : "\x0308",
        "light green" : "\x0309",
        "aqua"        : "\x0310",
        "light blue"  : "\x0311",
        "blue"        : "\x0312",
        "violet"      : "\x0313",
        "grey"        : "\x0314",
        "gray"        : "\x0314",
        "light grey"  : "\x0315",
        "light gray"  : "\x0315",
        "white"       : "\x0316",

        # Other formatting
        "normal"      : "\x0F",
        "bold"        : "\x02",
        "reverse"     : "\x16",
        "underline"   : "\x1F",
        }

    def __init__(self, name):
        self.name = name
        self.value = self.codes[name]

    def __str__(self):
        return self.value


def format(text, *codeNames):
    """Apply each formatting code from the given list of code names
       to the given text, returnging a string ready for consumption
       by an IRC client.
       """
    if codeNames:
        codes = "".join([str(FormattingCode(codeName)) for codeName in codeNames])
        return codes + text + str(FormattingCode('normal'))
    else:
        return text


class ColortextFormatter(object):
    r"""Given a domish.Element tree with <colorText>-formatted text
        generate an equivalent message formatted for IRC.

        >>> f = ColortextFormatter()
        >>> f.format(XML.parseString('<colorText><u><b>Hello</b> World</u></colorText>'))
        '\x1f\x02Hello\x0f\x1f World\x0f'

        >>> f.format(XML.parseString(
        ...    "<colorText>" +
        ...        "<color bg='dark blue'><color fg='yellow'>" +
        ...            "<b>hello</b>" +
        ...        "</color></color>" +
        ...        "<u> world</u>" +
        ...    "</colorText>"))
        '\x0302\x16\x0308\x02hello\x0f\x1f world\x0f'

        """
    def format(self, xml):
        """Format the entire given XML tree, returns a string with the finished
           text ready to send to IRC.
           """
        return self.flatten(self.formatElement(xml))

    def formatElement(self, xml, codeStack=[]):
        """Recursively format the given XML tree, passing elements on to their
           respective handlers and letting text fall straight through.
           codeStack is used to keep track of the formatting codes we're 'inside'
           so they can be restored if we have to issue a 'normal' code.
           Returns a list of strings and/or FormattingCode instances.
           """
        if isinstance(xml, XML.domish.Element):
            # It's an element, act on it based on its name
            try:
                f = getattr(self, "element_" + xml.name)
            except AttributeError:
                raise XML.XMLValidityError("Unexpected element %r" % xml.name)
            return f(xml, codeStack)
        elif type(xml) in types.StringTypes:
            # It's a string, pass it through
            return xml
        else:
            raise TypeError("XML elements or strings required")

    def flatten(self, codes):
        """Return a flat string representing the FormattingCode instances and/or strings
           in the given codes list.
           """
        return "".join([str(code) for code in codes])

    def formatChildren(self, xml, codeStack):
        """Format each child of the given node, appending the results"""
        codes = []
        for child in xml.children:
            codes.extend(self.formatElement(child, codeStack))
        return codes

    def element_colorText(self, xml, codeStack):
        """Ignore the root node of the <colorText> document"""
        return self.formatChildren(xml, codeStack)

    def codeWrap(self, xml, codeStack, *codeNames):
        """Wrap the children of the given xml element with the given formatting codes.
           This prepends the code list and appends a 'normal' tag, using codeStack
           to restore any codes we don't want to disable with the 'normal' tag.
           """
        # Apply all the codes we're given
        codes = [FormattingCode(name) for name in codeNames]

        # Insert the children here
        codes.extend(self.formatChildren(xml, codeStack + codes))

        # An important optimization- since we're about to insert new codes
        # for everything in codeStack, remove codes from our 'codes' list until
        # we hit actual text. This prevents sequences from appearing in the output
        # where several codes are applied then immediately erased by a 'normal' code.
        # This also handles optimizing out formatting codes with no children.
        while codes and isinstance(codes[-1], FormattingCode):
            del codes[-1]

        # Now stick on a 'normal' code and the contents of our codeStack
        # to revert the codes we were given.
        codes.append(FormattingCode('normal'))
        codes.extend(codeStack)
        return codes

    def element_b(self, xml, codeStack):
        """Just wrap our contents in a bold tag"""
        return self.codeWrap(xml, codeStack, 'bold')

    def element_u(self, xml, codeStack):
        """Just wrap our contents in an underline tag"""
        return self.codeWrap(xml, codeStack, 'underline')

    def element_br(self, xml, codeStack):
        """Insert a literal newline"""
        return "\n"

    def element_color(self, xml, codeStack):
        """Generates formatting codes appropriate to represent a foreground and/or background color"""
        codes = []
        try:
            if xml['bg'] in ColorText.allowedColors:
                codes.append(xml['bg'])
                codes.append('reverse')
            else:
                raise XML.XMLValidityError("%r is not a color" % xml['bg'])
        except KeyError:
            pass
        try:
            if xml['fg'] in ColorText.allowedColors:
                codes.append(xml['fg'])
            else:
                raise XML.XMLValidityError("%r is not a color" % xml['fg'])
        except KeyError:
            pass
        return self.codeWrap(xml, codeStack, *codes)


def _test():
    import doctest, IRC
    return doctest.testmod(IRC)

if __name__ == "__main__":
    _test()

### The End ###
