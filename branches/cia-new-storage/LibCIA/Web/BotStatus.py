""" LibCIA.Web.BotStatus

A web interface showing the current status of the BotNetwork
"""
#
# CIA open source notification system
# Copyright (C) 2003-2005 Micah Dowty <micah@navi.cx>
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

import Template, Server, Info
from LibCIA import TimeUtil
from Nouvelle import tag, place
import Nouvelle
from twisted.internet import defer
import time


class Component(Server.Component):
    """A server component for the IRC bot status section"""
    name = 'IRC Bots'

    def __init__(self, remoteBots):
        self.resource = IRCBotPage(remoteBots)

    def __contains__(self, page):
        return isinstance(page, IRCBotPage)


class TotalsSection(Template.Section):
    """A Section that displays fun-filled facts about our BotNetwork"""
    title = "totals"

    def __init__(self, remoteBots):
        self.remoteBots = remoteBots

    def render_rows(self, context):
        if self.remoteBots.botNet:
            result = defer.Deferred()
            self.remoteBots.botNet.callRemote("getTotals").addCallback(
                self._render_rows, result).addErrback(
                result.errback)
            return result
        else:
            return [
                Template.error[ "Bot server not available" ]
                ]

    def _render_rows(self, totals, result):
        result.callback([
            [
                Template.value[ totals['bots'] ],
                ' total IRC bots across ',
                Template.value[ totals['networks'] ],
                ' networks, inhabiting ',
                Template.value[ totals['channels'] ],
                ' channels with a total of ',
                Template.value[ totals['users'] ],
                ' users.',
            ],
            [
                Template.value[ totals['requests'] ],
                ' requests are currently registered with the bot network. ',
                Template.value[ totals['unfulfilled'] ],
                ' of these are unfulfilled.',
            ],
       ])


class ListIndexedColumn(Nouvelle.IndexedColumn):
    """An IndexedColumn that formats lists nicely, and sorts by list length"""
    def render_data(self, context, obj):
        return ", ".join(obj[self.index])

    def isVisible(self, context):
        return context['table'].reduceColumn(self, max)

    def getValue(self, obj):
        return len(obj[self.index])


class LagColumn(Nouvelle.IndexedColumn):
    def render_data(self, context, row):
        lag = self.getValue(row)
        if lag is None:
            return Template.error["unknown"]
        else:
            return "%.2fs" % lag


class BotServerColumn(Nouvelle.IndexedColumn):
    """A column showing the server a bot is connected to.
       For sorting purposes, this returns the network, so
       bots on the same network are grouped together.
       """
    def render_data(self, context, row):
        network, host, port = self.getValue(row)
        if port == 6667:
            return host
        else:
            return "%s:%d" % (host, port)


class BotsSection(Template.Section):
    """A section holding a table listing all bots"""
    title = 'bots'

    def __init__(self, remoteBots):
        self.remoteBots = remoteBots

        self.columns = [
            BotServerColumn('server', 0),
            Nouvelle.IndexedColumn('nickname', 1),
            ListIndexedColumn('channels', 2),
            ListIndexedColumn('requested', 3),
            LagColumn('lag', 4),
            TimeElapsedColumn('uptime', 6),
            Nouvelle.IndexedColumn('status', 5),
            ]

    def render_rows(self, context):
        # First, we need to get a list of bots
        if self.remoteBots.botNet:
            result = defer.Deferred()
            self.remoteBots.botNet.callRemote("getBots").addCallback(
                self._getBotInfo, result).addErrback(result.errback)
            return result
        else:
            return []

    def _getBotInfo(self, bots, result):
        if not bots:
            result.callback([])
            return

        deferreds = []
        for bot in bots:
            deferreds.append(defer.gatherResults([
                bot.callRemote("getNetworkInfo"),
                bot.callRemote("getNickname"),
                bot.callRemote("getChannels"),
                bot.callRemote("getRequestedChannels"),
                bot.callRemote("getLag"),
                bot.callRemote("getStatusText"),
                bot.callRemote("getConnectTimestamp"),
                ]))

        defer.gatherResults(deferreds).addCallback(
            self._render_rows, result).addErrback(result.errback)

    def _render_rows(self, tableRows, result):
        result.callback([Template.Table(tableRows, self.columns, id='bots')])


class OptionalAttributeStringColumn(Nouvelle.AttributeColumn):
    """A column displaying an object's attribute that converts the
       result into a string and allows it to be blank if the attribute
       doesn't exist in a particular object.
       """
    def getValue(self, obj):
        value = getattr(obj, self.attribute, '')
        if value is None:
            return ()
        else:
            return str(value)


class RequestStatusColumn(Nouvelle.IndexedColumn):
    """A column that gives a quick indication of a request's fulfillment status"""
    def render_data(self, context, req):
        if self.getValue(req):
            return 'ok'
        else:
            return Template.error["not fulfilled"]


class RequestUsersColumn(Nouvelle.IndexedColumn):
    """A column that displays the number of users a request serves"""
    def render_data(self, context, row):
        uc = self.getValue(row)
        if uc is None:
            return Template.error["unavailable"]
        else:
            return uc

    def cmp(self, a, b):
        # Reverse sort
        return cmp(self.getValue(b), self.getValue(a))


class RequestsSection(Template.Section):
    """A section listing all requests being asked of the BotNetwork"""
    title = 'requests'

    columns = [
        Nouvelle.IndexedColumn('channel', 0),
        Nouvelle.IndexedColumn('network', 1),
        Nouvelle.IndexedColumn('# of bots', 2),
        RequestStatusColumn('status', 3),
        RequestUsersColumn('users', 4),
        ListIndexedColumn('current bots', 5),
        ]

    def __init__(self, remoteBots):
        self.remoteBots = remoteBots

    def render_rows(self, context):
        # First, we need to get a list of request instances
        if self.remoteBots.botNet:
            result = defer.Deferred()
            self.remoteBots.botNet.callRemote("getRequests").addCallback(
                self._getRequestInfo, result).addErrback(result.errback)
            return result
        else:
            return []

    def _getRequestInfo(self, requests, result):
        if not requests:
            result.callback([])
            return

        deferreds = []
        for request in requests.itervalues():
            deferreds.append(defer.gatherResults([
                request.callRemote("getChannel"),
                request.callRemote("getNetworkName"),
                request.callRemote("getNumBots"),
                request.callRemote("isFulfilled"),
                request.callRemote("getUserCount"),
                request.callRemote("getBotNicks"),
                ]))

        defer.gatherResults(deferreds).addCallback(
            self._render_rows, result).addErrback(result.errback)

    def _render_rows(self, tableRows, result):
        result.callback([Template.Table(tableRows, self.columns, id='requests')])


class TimeRemainingColumn(Nouvelle.IndexedColumn):
    """An indexed column that shows the amount of time remaining until the given timestamp"""
    def render_data(self, context, row):
        return TimeUtil.formatDuration(self.getValue(row) - time.time())

class TimeElapsedColumn(Nouvelle.IndexedColumn):
    """An indexed column that shows the amount of time elapsed since the given timestamp"""
    def render_data(self, context, row):
        return TimeUtil.formatDuration(time.time() - self.getValue(row))


class NewBotsSection(Template.Section):
    """A section listing new bots we're waiting on connections from"""
    title = 'new bots'

    columns = [
        Nouvelle.IndexedColumn('connecting to...', 0),
        TimeRemainingColumn('time remaining', 1),
       ]

    def __init__(self, remoteBots):
        self.remoteBots = remoteBots

    def render_rows(self, context):
        if self.remoteBots.botNet:
            result = defer.Deferred()
            self.remoteBots.botNet.callRemote("getNewBots").addCallback(
                self._render_rows, result).addErrback(result.errback)
            return result
        else:
            return []

    def _render_rows(self, rows, result):
        if rows:
            result.callback([Template.Table(rows, self.columns, id='newBots')])
        else:
            result.callback([])


class DateColumn(Nouvelle.IndexedColumn):
    """An IndexedColumn that formats unix-style timestamps as dates"""
    def render_data(self, context, message):
        return TimeUtil.formatDate(self.getValue(message))


class MessageLogSection(Template.Section):
    """A section listing recent unknown IRC messages, including errors"""
    title = 'recent unhandled messages'

    columns = [
        DateColumn('time', 0),
        Nouvelle.IndexedColumn('bot', 1),
        Nouvelle.IndexedColumn('network', 2),
        Nouvelle.IndexedColumn('code', 3),
        Nouvelle.IndexedColumn('parameters', 4),
        ]

    def __init__(self, remoteBots):
        self.remoteBots = remoteBots

    def render_rows(self, context):
        # Retrieve the message buffer
        if self.remoteBots.botNet:
            result = defer.Deferred()
            self.remoteBots.botNet.callRemote("getMessageLog").addCallback(
                self._render_rows, result).addErrback(result.errback)
            return result
        else:
            return []

    def _render_rows(self, buffer, result):
        if buffer:
            result.callback([Template.Table(buffer, self.columns, id='msglog')])
        else:
            result.callback([])


class IRCBotPage(Template.Page):
    """A web page showing the status of the BotNetwork"""
    mainTitle = "IRC Bot Status"
    subTitle = "When you give a mouse a cookie, everything looks like a nail"

    def __init__(self, remoteBots):
        self.remoteBots = remoteBots

    def render_mainColumn(self, context):
        return [
            BotsSection(self.remoteBots),
            NewBotsSection(self.remoteBots),
            MessageLogSection(self.remoteBots),
            RequestsSection(self.remoteBots),
            ]

    def render_leftColumn(self, context):
        return [
            TotalsSection(self.remoteBots),
            Info.Clock(),
            ]

### The End ###
