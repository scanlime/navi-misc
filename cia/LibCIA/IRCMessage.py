""" LibCIA.IRCMessage

Listens to messages delivered through the Message.Hub, processing
commands for manipulating IRC filters. An IRC filter listens
to messages on the Message.Hub, picking some subset of them to
format and deliver to an IRC channel.
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

from twisted.python import log
import Message, XML


class HubListener(object):
    """Receives messages from the Message.Hub, directing
       them at a BotNetwork as necessary. This listens for
       commands instructing it to add and remove rulesets
       that transform messages from the Hub into formatted
       messages sent to a particular IRC channel via
       a supplied BotNetwork instance.
       """
    def __init__(self, hub, botNet, defaultHost, defaultPort=6667):
        self.hub = hub
        self.botNet = botNet
        self.defaultHost = defaultHost
        self.defaultPort = defaultPort
        self.addClients()

        # Maps (serverTuple, channel) tuples to IRCRuleset instances
        self.rulesets = {}

    def addClients(self):
        """Add all our initial clients to the hub"""
        self.hub.addClient(self.setIrcRuleset, Message.Filter(
            '<find path="/message/body/setIrcRuleset">'))
        self.hub.addClient(self.getIrcRuleset, Message.Filter(
            '<find path="/message/body/getIrcRuleset">'))

    def setIrcRuleset(self, message):
        """Handle messages instructing us to add, modify, or remove
           the IRCRuleset instance for a particular channel.
           These messages are of the form:

           <message><body>
               <setIrcRuleset channel="commits" server="irc.foo.net:6667">
                   <ruleset>
                       ...
                   </ruleset>
               </setIrcRuleset>
           </body></message>

           The leading '#' on a channel name is optional. The server can
           also be specified without a port or left off entirely, defaulting
           to the default server for this HubListener.

           An empty <setIrcRuleset> removes the ruleset associated with
           its channel, also causing the bot network to leave it.
           """
        # Extract the channel and server
        tag = message.xml.body.setIrcRuleset
        channel = tag.getAttribute('channel')
        if not channel:
            raise XML.XMLValidityError("The 'channel' attribute on <setIrcRuleset> is required")
        if channel[0] != '#':
            channel = '#' + channel
        server = tag.getAttribute('server', self.defaultHost)

        # Split the server into host and port, using our default if we can't
        if server.find(":") > 0:
            serverTuple = server.split(":")
        else:
            serverTuple = server, self.defaultPort

        # Make sure the ruleset parses before we do anything permanent
        if tag.ruleset:
            ruleset = Message.Ruleset(tag.ruleset)
        else:
            ruleset = None

        # Remove this channel's old IRCRuleset if it has one
        try:
            oldRuleset = self.rulesets[(serverTuple, channel)]
            del self.rulesets[(serverTuple, channel)]
            self.hub.delClient(oldRuleset)
        except KeyError:
            pass

        if ruleset:
            # We have a ruleset. Make this channel part of the bot network
            # if it isn't already, and set up an IRCRuleset.
            self.botNet.addChannel(serverTuple, channel)
            ircRuleset = IRCRuleset(self.botNet, serverTuple, channel, ruleset)

            # Install the new ruleset
            self.rulesets[(serverTuple, channel)] = ircRuleset
            self.hub.addClient(ircRuleset)
        else:
            # No ruleset, we're removing this channel
            self.botNet.delChannel(serverTuple, channel)

    def getIrcRuleset(self, message):
        """Return the current <ircFilters> tag with attributes matching
           those of the <getIrcFilters> tag.
           """
        return 42


class IRCRuleset(object):
    """Ties together an IRC channel, BotNetwork, and Ruleset
       into one object that can be used as a client to the
       Message.Hub.
       """
    def __init__(self, botNet, server, channel, ruleset):
        self.botNet = botNet
        self.server = server
        self.channel = channel
        self.ruleset = ruleset

    def __call__(self, message):
        """When the IRCRuleset is used as a client to the
           message.Hub, it is called to deliver a message.
           This runs the message through our ruleset, and
           if a result pops out, sends it to our IRC channel.
           """
        result = self.ruleset(message)
        if result:
            self.botNet.msg(self.server, self.channel, result)

### The End ###
