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
import os


class HubListener(object):
    """Receives messages from the Message.Hub, directing
       them at a BotNetwork as necessary. This listens for
       commands instructing it to add and remove rulesets
       that transform messages from the Hub into formatted
       messages sent to a particular IRC channel via
       a supplied BotNetwork instance.
       """
    def __init__(self, hub, botNet, defaultHost, defaultPort=6667, ruleFile="data/irc_rules.xml"):
        self.hub = hub
        self.botNet = botNet
        self.defaultHost = defaultHost
        self.defaultPort = defaultPort
        self.ruleFile = ruleFile
        self.addClients()

        # Maps (serverTuple, channel) tuples to IRCRuleset instances
        self.rulesets = {}

        # Assign a callback for handling bots that are kicked.
        # Since we must no longer be wanted, this sends a message that
        # deletes the associated IRC ruleset.
        botNet.kickCallback = self.kickCallback

        # Load our initial ruleset if the file exists
        if os.path.isfile(self.ruleFile):
            self.load()

    def load(self):
        """Load rulesets from self.ruleFile"""
        f = open(self.ruleFile)
        f.readline()
        xml = XML.parseString(f.read())
        f.close()

        for tag in xml.children:
            if isinstance(tag, XML.domish.Element):
                self.installIrcRuleset(tag)

    def save(self):
        """Save our currently loaded rulesets to self.ruleFile"""
        f = open(self.ruleFile, "w")
        f.write('<?xml version="1.0"?>\n')
        f.write('<ircRuleSets>\n')

        # Sort rulesets by (server,channel) so they're output in a predictable order.
        # This makes 'diff' between multiple channel lists much more reliable.
        rulesetItems = self.rulesets.items()
        rulesetItems.sort(lambda a, b: cmp(a[0], b[0]))
        for location, ruleset in rulesetItems:
            f.write("\n%s\n" % ruleset)

        f.write('\n</ircRuleSets>\n')
        f.close()

    def kickCallback(self, server, channel):
        """Called when any of our bots are kicked from a channel-
           sends a message that removes that bot's IRC rulesets,
           since it isn't wanted.
           """
        xml = XML.domish.Element((None, "message"))
        body = xml.addElement("body")
        ircRuleset = body.addElement("ircRuleset")
        ircRuleset['server'] = "%s:%s" % server
        ircRuleset['channel'] = channel
        self.hub.deliver(Message.Message(xml))

    def addClients(self):
        """Add all our initial clients to the hub"""
        # This uses an extra level of indirection so that
        # rebuild can replace references to our callbacks correctly.
        self.hub.addClient(lambda msg: self.handle_ircRuleset(msg), Message.Filter(
            '<find path="/message/body/ircRuleset">'))
        self.hub.addClient(lambda msg: self.handle_queryIrcRulesets(msg), Message.Filter(
            '<find path="/message/body/queryIrcRulesets">'))

    def parseIrcRulesetAttribs(self, element):
        """Given an ircRuleset or queryIrcRulesets element, returns a
           (serverTuple, channel) representing the server and channel
           it refers to. Either may be None if it was not included in the element.
           """
        channel = element.getAttribute('channel')
        if channel and channel[0] != '#':
            channel = '#' + channel

        server = element.getAttribute('server')
        if server:
            # Split the server into host and port, using our default if we can't
            if server.find(":") > 0:
                serverParts = server.split(":")
                serverTuple = serverParts[0], int(serverParts[1])
            else:
                serverTuple = server, self.defaultPort
        else:
            serverTuple = None
        return (serverTuple, channel)

    def handle_ircRuleset(self, message):
        """Handle messages instructing us to add, modify, or remove
           the IRCRuleset instance for a particular channel.
           These messages are of the form:

           <message><body>
               <ircRuleset channel="commits" server="irc.foo.net:6667">
                   <ruleset>
                       ...
                   </ruleset>
               </setIrcRuleset>
           </body></message>

           The leading '#' on a channel name is optional. The server can
           also be specified without a port or left off entirely, defaulting
           to the default server for this HubListener.

           An empty <ircRuleset> removes the ruleset associated with
           its channel, also causing the bot network to leave it.
           """
        self.installIrcRuleset(message.xml.body.ircRuleset)
        self.save()

    def installIrcRuleset(self, tag):
        """Parse and install an <ircRuleset> tag"""
        server, channel = self.parseIrcRulesetAttribs(tag)
        if not channel:
            raise XML.XMLValidityError("The 'channel' attribute on <ircRuleset> is required")
        if not server:
            # Default server
            server = (self.defaultHost, self.defaultPort)

        # Make sure the ruleset parses before we do anything permanent
        if tag.ruleset:
            ruleset = Message.Ruleset(tag.ruleset)
        else:
            ruleset = None

        # Remove this channel's old IRCRuleset if it has one
        try:
            oldRuleset = self.rulesets[tuple(server), channel]
            del self.rulesets[(server, channel)]
            self.hub.delClient(oldRuleset)
        except KeyError:
            pass

        if ruleset:
            # We have a ruleset. Make this channel part of the bot network
            # if it isn't already, and set up an IRCRuleset.
            self.botNet.addChannel(server, channel)
            ircRuleset = IRCRuleset(self.botNet, server, channel, ruleset)

            # Install the new ruleset
            self.rulesets[(server, channel)] = ircRuleset
            self.hub.addClient(ircRuleset)
            log.msg("Set IRC ruleset for channel %r on server %r:\n%s" %
                    (channel, server, XML.prettyPrint(ruleset.xml)))
        else:
            # No ruleset, we're removing this channel
            self.botNet.delChannel(server, channel)
            log.msg("Removed IRC ruleset for channel %r on server %r" % (channel, server))

    def handle_queryIrcRulesets(self, message):
        """Handles messages containing a <queryIrcRulesets> tag in its body.
           We search for all IRCRulesets matching the given server and/or channel,
           and return a list of matches each as a string of XML.
           """
        tag = message.xml.body.queryIrcRulesets
        server, channel = self.parseIrcRulesetAttribs(tag)

        results = []
        for (itemServer, itemChannel), ircRuleset in self.rulesets.iteritems():
            if server is not None and itemServer != server:
                continue
            if channel is not None and itemChannel != channel:
                continue
            results.append(str(ircRuleset))
        return results


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

    def __str__(self):
        """Return an XML representation of this object as an <ircRuleset> tag"""
        return "<ircRuleset channel='%s' server='%s:%s'>\n%s\n</ircRuleset>" % (
            self.channel, self.server[0], self.server[1], self.ruleset.xml.toXml())

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
