""" LibCIA.Web.BotStatus

A web interface showing the current status of the BotNetwork
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

import Template
from Nouvelle import tag, place
import Nouvelle


class TotalsSection(Template.Section):
    """A Section that displays fun-filled facts about our BotNetwork"""
    def __init__(self, botNet):
        self.totalServers = 0
        self.totalBots = 0
        self.totalChannels = 0
        for server in botNet.servers.iterkeys():
            self.totalServers += 1
            for bot in botNet.servers[server]:
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


class BotChannelsColumn(Nouvelle.Column):
    """A table column listing a bot's channels"""
    heading = 'channels'

    def render_data(self, context, bot):
        return ", ".join(bot.channels)

    def getValue(self, bot):
        return len(bot.channels)


class ServerSection(Template.Section):
    """A section representing one IRC server"""
    def __init__(self, botNet, server):
        self.botNet = botNet
        self.server = server

    def render_title(self, context):
        return str(self.server)

    def render_rows(self, context):
        return [Template.Table(self.botNet.servers[self.server], [
            Nouvelle.AttributeColumn('nickname', 'nickname'),
            BotChannelsColumn(),
            ], id=str(self.server))]


class IRCBotPage(Template.Page):
    """A web page showing the status of the BotNetwork"""
    def __init__(self, botNet):
        self.botNet = botNet

    def render_mainColumn(self, context):
        # One server section for each server we have bots on
        servers = self.botNet.servers.keys()
        servers.sort(lambda a,b: cmp(str(a), str(b)))
        return [ServerSection(self.botNet, s) for s in servers]

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
