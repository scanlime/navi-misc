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
       commands instructing it to add and remove filters that
       collect messages from the Hub, format them, and send
       them to a particular IRC channel via the BotNetwork.
       """
    def __init__(self, hub, botNet):
        self.hub = hub
        self.botNet = botNet
        self.addClients()

    def addClients(self):
        """Add all our initial clients to the hub"""
        self.hub.addClient(self.handleIrcFilters, Message.Filter(
            '<find path="/message/body/ircFilters">'))
        self.hub.addClient(self.handleGetIrcFilters, Message.Filter(
            '<find path="/message/body/getIrcFilters">'))

    def handleIrcFilters(self, message):
        """Process messages containing and <ircFilters> tag in
           their body. These messages set the list of filters
           and formatters being used by a particular IRC channel.
           An empty <ircFilters/> tag removes all filters.
           The BotNetwork is automatically updated so that if a
           channel has any filters assigned, a bot will inhabit it.

           An <ircFilters> tag has attributes specifying the server,
           port, and channel it applies to. The leading '#' on a channel
           name is optional.
           It contains zero or more <ircFilter> tags, followed by tags
           describing transforms applied to all messages delivered to the channel.

           An <ircFilter> tag's first child must be a valid Message.Filter.
           All other children are treated as transforms applied to the
           message, in order, resulting in the text finally delivered
           to the channel.

           An couple example <ircFilters> messages..

              <message><body>
                 <ircFilters channel="commits" host="irc.freenode.net" port="6667">
                    <ircFilter>
                       <find path="/message/body/commit"/>
                       <formatter name="commit"/>
                    </ircFilter>
                    <ircFilter>
                       <find path="/message/body/colorText"/>
                       <formatter name="colorText"/>
                    </ircFilter>
                    <formatter name="projectPrefix"/>
                 </ircFilters>
              </body></message>
           """
        ircFilters = message.xml.body.ircFilters
        server = ircFilters['host'], int(ircFilters['port'])
        channel = ircFilters['channel']
        if channel[0] != '#':
            channel = '#' + channel

        if ircFilters.ircFilter:
            self.botNet.addChannel(server, channel)
        else:
            self.botNet.delChannel(server, channel)

    def handleGetIrcFilters(self, message):
        """Return the current <ircFilters> tag with attributes matching
           those of the <getIrcFilters> tag.
           """
        return 42

### The End ###
