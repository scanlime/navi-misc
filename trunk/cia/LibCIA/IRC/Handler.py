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
        # A map from (server, channel) tuple to ChannelMessageQueue
        self.channelQueueMap = {}

        # Map from (server, channel) tuples to URIs, so we can avoid duplicates
        self.uriMap = {}

        self.botNet = botNet
        Ruleset.RegexURIHandler.__init__(self)

    def uriToTuple(self, uri):
        """Convert a URI to a (server, channel) tuple"""
        d = self.parseURI(uri)
        return (Bots.Server(d['host'], d['port']), '#' + d['channel'])

    def assigned(self, uri, newRuleset):
        t = self.uriToTuple(uri)

        # Check for duplicates
        if self.uriMap.has_key(t) and self.uriMap[t] != uri:
            raise Ruleset.InvalidURIException("Another URI referring to the same server and channel already exists")
        self.uriMap[t] = uri

        # Create a new channel queue if we need one
        if not self.channelQueueMap.has_key(t):
            self.channelQueueMap[t] = Bots.ChannelMessageQueue(self.botNet, t[0], t[1])

    def unassigned(self, uri):
        t = self.uriToTuple(uri)
        self.channelQueueMap[t].cancel()
        del self.channelQueueMap[t]
        del self.uriMap[t]

    def message(self, uri, message, content):
        self.channelQueueMap[self.uriToTuple(uri)].send(content)

### The End ###
