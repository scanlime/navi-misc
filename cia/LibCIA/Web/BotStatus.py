""" LibCIA.Web.BotStatus

A web interface showing the current status of the BotNetwork
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

import Template
from Base import tag, place


class TotalsSection(Template.Section):
    """A Section that displays fun-filled facts about our BotNetwork"""
    def __init__(self, botNet):
        self.totalServers = 0
        self.totalBots = 0
        self.totalChannels = 0
        for allocator in botNet.servers.itervalues():
            self.totalServers += 1
            for bot in allocator.bots.itervalues():
                self.totalBots += 1
                self.totalChannels += len(bot.channels)

    title = "totals"
    rows = [[
        Template.value[ place('totalBots') ],
        ' total IRC bots across ',
        Template.value[ place('totalServers') ],
        ' servers, inhabiting ',
        Template.value[ place('totalChannels') ],
        ' channels.',
        ]]


class ServerSection(Template.Section):
    """A section representing one BotAllocator, and therefore one IRC server"""
    def __init__(self, allocator):
        self.allocator = allocator

    def render_title(self, context):
        return "%s:%d" % (self.allocator.host, self.allocator.port)

    def render_tableRows(self, context):
        nicks = self.allocator.bots.keys()
        nicks.sort()
        return [tag('tr')[self.renderBot(self.allocator.bots[nick])] for nick in nicks]

    def renderBot(self, bot):
        return [
            tag('td')[ bot.nickname ],
            tag('td')[ ", ".join(bot.channels) ],
            ]

    rows = [ tag('table')[
                 tag('tr')[
                     tag('th')[ 'nickname' ],
                     tag('th')[ 'channels' ],
                 ],
                 place('tableRows'),
             ]]


class IRCBotPage(Template.Page):
    """A web page showing the status of the BotNetwork"""
    def __init__(self, botNet):
        self.botNet = botNet

    def render_mainColumn(self, context):
        """Generate one ServerSection for each BotAllocator we have, sorted by hostname"""
        allocators = self.botNet.servers.values()
        allocators.sort(lambda a,b: cmp(a.host, b.host))
        return [ServerSection(a) for a in allocators]

    def render_leftColumn(self, context):
        return [
            TotalsSection(self.botNet),
            ]

    mainTitle = "IRC Bot Status"
    subTitle = "what's that blue thing doing there?"
    headingTabs = [
        Template.headingTab(href='/')['CIA'],
        ]

### The End ###
