""" LibCIA.Web.Stats.Browser

Implements CIA's actual stats browser pages. This uses Sections provided
by other modules in this package, and detects magic URLs that rediect one
to metadata or RSS pages.
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

from twisted.internet import defer
from LibCIA.Web import Template
from LibCIA import Stats, Message, TimeUtil
from Nouvelle import tag, place
import Nouvelle, time
import Metadata, Link, Catalog, RSS


class Page(Template.Page):
    """A web page providing an interface to one StatsTarget.
       The root of the stats namespace should be created with the
       capabilities database and StatsStorage. Children will
       be automatically created with child targets.
       """
    def __init__(self, target=None):
        if target is None:
            target = Stats.StatsTarget()
        self.target = target

    def getChildWithDefault(self, name, request):
        """Part of IResource, called by twisted.web to retrieve pages for URIs
           below this one. This just creates a Page instance for our StatsTarget's child,
           with a few special cases used for metadata and editing.
           """
        if not name:
            # Ignore empty path sections
            return self
        elif name == '.metadata':
            # Return a special page that accesses this stats target's metadata
            return Metadata.MetadataPage(self)
        elif name == '.rss':
            # Return an RSS feed with this stats target's most recent commits in XML
            return RSS.RSSFeed(self)
        else:
            # Return the stats page for a child
            return self.__class__(self.target.child(name))

    def findRootPath(self, request, additionalDepth=0, absolute=False):
        """Find the URL path referring to the root of the current stats tree
           The returned path begins and ends with a slash.

           additionalDepth can be set to a nonzero number if the caller
           knows the request is actually for a page below this one in the URL tree.

           If 'absolute' is True, a full absolute URL is returned.
           """
        pathSegments = [s for s in request.path.split('/') if s]
        treeDepth = len(self.target.pathSegments) + additionalDepth
        if treeDepth:
            pathSegments = pathSegments[:-treeDepth]
        path = '/' + '/'.join(pathSegments) + '/'

        # Cheesily stick together an absolute URL
        if absolute:
            ns, host, port = request.host
            if port != 80:
                host += ':' + str(port)
            path = 'http://' + host + path

        return path

    def preRender(self, context):
        context['statsRootPath'] = self.findRootPath(context['request'])

    def render_mainTitle(self, context):
        return self.target.getTitle()

    def render_subTitle(self, context):
        return self.target.metadata.getValue('subtitle', 'Real-time open source activity stats')

    def render_mainColumn(self, context):
        return [
            Counters(self.target),
            Catalog.CatalogSection(self.target),
            RecentMessages(self.target),
            ]

    def render_leftColumn(self, context):
        return [
            Metadata.Info(self.target),
            LinksSection(self.target),
            Clock(),
            ]

    def render_headingTabs(self, context):
        """Create tabs linking to all our parent stats targets and to the CIA root"""
        tabs = []
        node = self.target.parent()
        while node:
            tabs.insert(0, Link.StatsLink(node, Template.headingTab))
            node = node.parent()
        tabs.insert(0, Template.headingTab(href='/')['CIA'])
        return tabs


class Clock(Template.Section):
    title = "UTC clock"

    def render_rows(self, context):
        return [TimeUtil.formatDate(time.time())]


class Counters(Template.Section):
    """A Section displaying the counters from a StatsTarget"""
    title = "event counters"

    rows = [
               [
                   'The last message was received ',
                   Template.value[ place('value', 'forever', 'lastEventTime', 'relativeDate') ],
                   ' ago at ',
                   Template.value[ place('value', 'forever', 'lastEventTime', 'date') ],
               ],
               [
                   Template.value[ place('value', 'today', 'eventCount') ],
                   ' messages so far today, ',
                   Template.value[ place('value', 'yesterday', 'eventCount') ],
                   ' messages yesterday',
               ],
               [
                   Template.value[ place('value', 'thisWeek', 'eventCount') ],
                   ' messages so far this week, ',
                   Template.value[ place('value', 'lastWeek', 'eventCount') ],
                   ' messages last week',
               ],
               [
                   Template.value[ place('value', 'thisMonth', 'eventCount') ],
                   ' messages so far this month, ',
                   Template.value[ place('value', 'lastMonth', 'eventCount') ],
                   ' messages last month',
               ],
               [
                   Template.value[ place('value', 'forever', 'eventCount') ],
                   ' messages since the first one, ',
                   Template.value[ place('value', 'forever', 'firstEventTime', 'relativeDate') ],
                   ' ago',
                   place('averagePeriod', 'forever'),
               ],
        ]

    def __init__(self, target):
        self.counters = target.counters

    def render_rows(self, context):
        """If this target has received at least one event, render the rows normally..
           otherwise, hide this section completely.
           """
        result = defer.Deferred()
        self.counters.getCounter('forever').addCallback(
            self._render_rows, result).addErrback(result.errback)
        return result

    def _render_rows(self, foreverCounter, result):
        if foreverCounter and foreverCounter['eventCount'] > 0:
            result.callback(self.rows)
        else:
            result.callback([])

    def render_value(self, context, counterName, valueName, filter=None):
        """Fetch a counter value, rendering its value optionally via
           a filter function. 'filter' is a string which will be used to
           look up a filter_* method from this class.
           """
        result = defer.Deferred()
        self.counters.getCounter(counterName).addCallback(
            self._render_value, valueName, filter, result).addErrback(result.errback)
        return result

    def _render_value(self, counter, valueName, filter, result):
        if counter:
            value = counter.get(valueName, 0)
        else:
            value = 0
        if filter:
            value = getattr(self, 'filter_'+filter)(value)
        result.callback(value)

    def filter_date(self, value):
        return TimeUtil.formatDate(value)

    def filter_relativeDate(self, value):
        return TimeUtil.formatDuration(time.time() - value)

    def render_averagePeriod(self, context, counterName):
        result = defer.Deferred()
        self.counters.getCounter(counterName).addCallback(
            self._render_averagePeriod, result).addErrback(result.errback)
        return result

    def _render_averagePeriod(self, counter, result):
        if not counter:
            result.callback('')
            return
        events = counter.get('eventCount', 0)
        first = counter.get('firstEventTime')
        if events < 2 or not first:
            result.callback('')
            return
        result.callback([
            ', for an average of ',
            Template.value[ TimeUtil.formatDuration( (time.time() - first) / events ) ],
            ' between messages',
            ])


class MessageDateColumn(Nouvelle.Column):
    """A column that displays a message's date"""
    heading = 'date'

    def getValue(self, message):
        return int(str(message.xml.timestamp))

    def render_data(self, context, message):
        return TimeUtil.formatDate(self.getValue(message))


class MessageProjectColumn(Nouvelle.Column):
    """A column that displays the project a message originated from"""
    heading = 'project'

    def getValue(self, message):
        if message.xml.source:
            return str(message.xml.source.project)


class MessageContentColumn(Nouvelle.Column):
    """A column that displays a message, formatted in XHTML"""
    heading = 'content'

    def getValue(self, message):
        return Message.AutoFormatter('xhtml').format(message)


class MessageList(Template.Section):
    """A list of messages, with metadata and hyperlinks. Must be
       constructed with a list of Message instances.
       """
    title = "messages"

    columns = [
        MessageDateColumn(),
        MessageProjectColumn(),
        MessageContentColumn(),
        ]

    def renderMessages(self, context, messages):
        return [
            Template.Table(messages, self.columns,
                           defaultSortReversed = True,
                           id = 'message'),
            ]


class RecentMessages(MessageList):
    """A section displaying recent messages from a given stats target"""
    title = "recent messages"

    def __init__(self, target, limit=20):
        self.target = target
        self.limit = limit

    def render_rows(self, context):
        result = defer.Deferred()
        self.target.messages.getLatest(self.limit).addCallback(
            self._render_rows, context, result).addErrback(result.errback)
        return result

    def _render_rows(self, messages, context, result):
        """Actually render the rows, called after the message list has been retrieved"""
        if messages:
            result.callback(self.renderMessages(context, [Message.Message(m) for m in messages]))
        else:
            result.callback(None)


class LinksSection(Template.Section):
    """A section displaying useful links for a particular stats target"""
    title = 'links'

    def __init__(self, target):
        self.target = target

    def render_rows(self, context):
        return [
            Link.MetadataLink(self.target),
            Link.RSSLink(self.target),
            ]

### The End ###
