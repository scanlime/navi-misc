""" LibCIA.IRC.Handler

The irc:// URI handler, acting as a frontend to our network of bots
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

from LibCIA import Ruleset
import Bots


class IrcURIHandler(Ruleset.RegexURIHandler):
    """Handles irc:// URIs in rulesets. This creates a message queue
       for each URI, and delivers formatted messages to the proper
       message queue instance.

       This follows a subset of the internet draft at:

          http://www.w3.org/Addressing/draft-mirashi-url-irc-01.txt

       In short, the following sorts of URIs are valid:

          irc://irc.foo.net/boing
            Refers to the channel #boing at irc.foo.net on the default port

          irc://irc.foo.net:1234/boing
            Refers to the channel #boing at irc.foo.net, on port 1234

          irc://irc.foo.net/muffin,isnick
            Refers to a users with the nick 'muffin' on irc.foo.net's default port
       """
    scheme = 'irc'
    regex = r"""
       ^irc://(?P<host>[a-zA-Z]([a-zA-Z0-9.-]*[a-zA-Z0-9])?)
       (:(?P<port>[0-9]+))?/(?P<target>[^\s#,]+)(?P<isnick>,isnick)?$
       """

    def __init__(self, botNet):
        # A map from URI to message queue
        self.queueMap = {}
        self.botNet = botNet
        Ruleset.RegexURIHandler.__init__(self)

    def createQueueFromURI(self, uri):
        """Convert a URI to a new message queue instance"""
        d = self.parseURI(uri)
        server = Bots.Server(d['host'], d['port'])

        if d['isnick']:
            # This refers to a nickname- deliver private messages to that user
            return PrivateMessageQueue(self.botNet, server, d['target'])
        else:
            # It's a channel, without the '#'
            return ChannelMessageQueue(self.botNet, server, '#' + d['target'])

    def assigned(self, uri, newRuleset):
        # If this URI is new, create a new message queue for it
        if not uri in self.queueMap:
            q = self.createQueueFromURI(uri)
            self.queueMap[uri] = q

    def unassigned(self, uri):
        self.queueMap[uri].cancel()
        del self.queueMap[uri]

    def message(self, uri, message, content):
        self.queueMap[uri].send(content)


class MessageQueue:
    """Abstract base class for a queue we can deliver IRC messages to"""
    def __init__(self, botNet, server, channel, target):
        self.target = target
        self.request = Bots.Request(botNet, server, channel)
        self.queuedLines = []

    def cancel(self):
        """Cancel the request associated with this message queue"""
        self.request.cancel()

    def send(self, message):
        """Split up a message into lines and queue it for transmission"""
        self.queuedLines.extend(message.split('\n'))
        self.checkQueue()

    def checkQueue(self):
        # FIXME: for now, send the whole queue contents to the first bot.
        #        This is where rate limiting and load balancing goes.
        while self.queuedLines:
            if not self.request.bots:
                # For now, since we have no protection for the buffer getting huge,
                # flush it when this happens.
                del self.queuedLines[:]
                return
            self.request.bots[0].msg(self.target, self.queuedLines[0])
            del self.queuedLines[0]


class PrivateMessageQueue(MessageQueue):
    """Send private messages to a particular user, using one bot"""
    def __init__(self, botNet, server, nick):
        MessageQueue.__init__(self, botNet, server, None, nick)


class ChannelMessageQueue(MessageQueue):
    """Send messages to a channel, using multiple bots if necessary"""
    def __init__(self, botNet, server, channel):
        MessageQueue.__init__(self, botNet, server, channel, channel)

### The End ###
