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

### The End ###
