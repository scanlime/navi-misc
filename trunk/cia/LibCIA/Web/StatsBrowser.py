""" LibCIA.Web.StatsBrowser

A web interface for CIA's stats:// namespace
"""
#
# CIA open source notification system
# Copyright (C) 2003-2004 Micah Dowty <micahjd@users.sourceforge.net>
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

from __future__ import division
import time, math
import Template, Nouvelle
from LibCIA import TimeUtil, Message
from Nouvelle import tag, place


class Clock(Template.Section):
    title = "UTC Clock"

    def render_rows(self, context):
        return [TimeUtil.formatDate(time.time())]


class Counters(Template.Section):
    """A Section displaying the counters from a StatsTarget"""
    title = "event counters"
    rows = [
               [
                   'The last message was received ',
                   Template.value[ place('relativeDate', 'forever', 'lastEventTime') ],
                   ' ago at ',
                   Template.value[ place('date', 'forever', 'lastEventTime') ],
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
                   Template.value[ place('relativeDate', 'forever', 'firstEventTime') ],
                   ' ago',
                   place('averagePeriod', 'forever'),
               ],
        ]

    def __init__(self, target):
        self.counters = target.counters

    def isVisible(self, context):
        """Hide the counters if this target has never received an event"""
        return self.counters.getCounter('forever').get('eventCount', 0) != 0

    def render_value(self, context, counterName, valueName):
        return self.counters.getCounter(counterName).get(valueName, 0)

    def render_date(self, context, counterName, valueName):
        value = self.counters.getCounter(counterName).get(valueName)
        if value is not None:
            return TimeUtil.formatDate(value)

    def render_duration(self, context, counterName, valueName):
        value = self.counters.getCounter(counterName).get(valueName)
        if value is not None:
            return TimeUtil.formatDuration(value)

    def render_relativeDate(self, context, counterName, valueName):
        value = self.counters.getCounter(counterName).get(valueName, 0)
        if value is not None:
            return TimeUtil.formatDuration(time.time() - value)

    def render_averagePeriod(self, context, counterName):
        counter = self.counters.getCounter(counterName)
        events = counter.get('eventCount', 0)
        first = counter.get('firstEventTime')
        last = counter.get('lastEventTime')
        if events < 2 or (not first) or (not last) or first == last:
            return ''
        return [
            ', for an average of ',
            Template.value[ TimeUtil.formatDuration( (last - first) / events ) ],
            ' between messages',
            ]


class StatsLink:
    """An anchor tag linking to the given stats target"""
    def __init__(self, target, tagFactory=tag('a')):
        self.target = target
        self.tagFactory = tagFactory

    def render(self, context):
        url = context['statsRootPath'] + '/'.join(self.target.pathSegments)
        return self.tagFactory(href=url)[self.target.getTitle()]


class TargetTitleColumn(Nouvelle.Column):
    """A Column displaying a target's title as a StatsLink"""
    heading = 'title'

    def getValue(self, target):
        # For case-insensitive sorting by title
        return target.getTitle().lower()

    def render_data(self, context, target):
        return StatsLink(target)


class TargetCounterColumn(Nouvelle.Column):
    """A Column displaying a counter value from each target"""
    def __init__(self, heading, counterName):
        self.heading = heading
        self.counterName = counterName

    def getValue(self, target):
        return target.counters.getCounter(self.counterName).get('eventCount', 0)

    def cmp(self, a, b):
        # Reverse the default sort, show larger counters first
        return -Nouvelle.Column.cmp(self, a, b)

    def isVisible(self, context):
        return context['table'].reduceColumn(self, max) > 0


class TargetLastEventColumn(Nouvelle.Column):
    """A Column displaying the amount of time since the last message was delivered to each target"""
    heading = "last event"

    def getValue(self, target):
        """Returns the number of seconds since the last event"""
        lastTime = target.counters.getCounter('forever').get('lastEventTime')
        if lastTime:
            return time.time() - lastTime

    def isVisible(self, context):
        # Hide this column if none of the targets have had any messages
        return context['table'].reduceColumn(TargetCounterColumn(None, 'forever'), max) > 0

    def render_data(self, context, target):
        value = self.getValue(target)
        if value is None:
            return ''
        else:
            return "%s ago" % TimeUtil.formatDuration(self.getValue(target))


class TargetBargraphColumn(TargetCounterColumn):
    """A Column that renders a counter value as a logarithmic bar chart"""
    def render_data(self, context, target):
        value = self.getValue(target)
        if not value:
            return ''
        logMax = math.log(context['table'].reduceColumn(self, max))
        if logMax > 0:
            fraction = math.log(value) / logMax
        else:
            fraction = 1
        return Template.Bargraph(fraction)[ value ]


class TargetPercentColumn(TargetCounterColumn):
    """A Column that renders a counter value as a percent of the column's total"""
    def render_data(self, context, target):
        value = self.getValue(target)
        if not value:
            return ''
        total = context['table'].reduceColumn(self, sum)
        return "%.03f" % (value / total * 100)


class Catalog(Template.Section):
    """A Section displaying links to all children of a StatsTarget, with
       other information about the children displayed as applicable.
       """
    title = "catalog"

    def __init__(self, target):
        self.target = target
        self.names = target.catalog()
        self.names.sort(lambda a,b: cmp(a.lower(), b.lower()))
        self.childTargets = [self.target.child(name) for name in self.names]

    def isVisible(self, context):
        return len(self.names) != 0

    def render_rows(self, context):
        return [Template.Table(self.childTargets, [
            TargetTitleColumn(),
            TargetBargraphColumn('events today', 'today'),
            TargetBargraphColumn('events yesterday', 'yesterday'),
            TargetBargraphColumn('total events', 'forever'),
            TargetPercentColumn('% total', 'forever'),
            TargetLastEventColumn(),
            ])]


class Info(Template.Section):
    """A section that displays a StatsTarget's miscellaneous metadata"""
    title = "information"

    def __init__(self, target):
        metadata = target.metadata
        self.url = metadata.get('url')
        self.description = metadata.get('description')
        self.photo_url = metadata.get('photo_url')

    def isVisible(self, context):
        return self.url or self.description

    def render_rows(self, context):
        rows = []
        if self.url:
            rows.append(tag('a', href=self.url)[self.url])
        if self.photo_url:
            rows.append(Template.photo(src=self.photo_url))
        if self.description:
            rows.append(self.description)
        return rows


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
        return Nouvelle.xml(Message.AutoFormatter('xhtml').format(message))


class MessageList(Template.Section):
    """A list of messages, with metadata and hyperlinks. Must be
       constructed with a list of Message instances.
       """
    title = "messages"

    def __init__(self, messages):
        self.messages = messages
        self.columns = [
            MessageDateColumn(),
            MessageProjectColumn(),
            MessageContentColumn(),
            ]

    def render_rows(self, context):
        return [Template.Table(self.messages, self.columns,
                               defaultSortReversed = True)]


class RecentMessages(MessageList):
    """A section displaying recent messages from a given stats target"""
    title = "recent messages"

    def __init__(self, target, limit=20):
        messages = target.recentMessages.getLatest(limit)
        messages.reverse()
        MessageList.__init__(self, [Message.Message(m) for m in messages])

    def isVisible(self, context):
        return len(self.messages) != 0


class StatsPage(Template.Page):
    """A web page providing an interface to one StatsTarget.
       The root of the stats namespace should be created with the
       capabilities database and StatsStorage. Children will
       be automatically created with child targets.
       """
    def __init__(self, caps, storage, target=None):
        if target is None:
            target = storage.getRoot()
        self.caps = caps
        self.storage = storage
        self.target = target

    def getChildWithDefault(self, name, request):
        """Part of IResource, called by twisted.web to retrieve pages for URIs
           below this one. This just creates a StatsPage instance for our StatsTarget's child.
           """
        if name:
            return StatsPage(self.caps, self.storage,
                             self.target.child(name))
        else:
            # Ignore empty path sections
            return self

    def findRootPath(self, request):
        """Find the URL path referring to the root of the current stats tree
           The returned path begins and ends with a slash.
           """
        pathSegments = [s for s in request.path.split('/') if s]
        treeDepth = len(self.target.pathSegments)
        if treeDepth:
            pathSegments = pathSegments[:-treeDepth]
        return '/' + '/'.join(pathSegments) + '/'

    def preRender(self, context):
        context['statsRootPath'] = self.findRootPath(context['request'])

    def render_mainTitle(self, context):
        return self.target.getTitle()

    def render_subTitle(self, context):
        return self.target.metadata.get('subtitle', 'Real-time open source activity stats')

    def render_mainColumn(self, context):
        return [
            Counters(self.target),
            RecentMessages(self.target),
            Catalog(self.target),
            ]

    def render_leftColumn(self, context):
        return [
            Info(self.target),
            Clock()
            ]

    def render_headingTabs(self, context):
        """Create tabs linking to all our parent stats targets and to the CIA root"""
        tabs = []
        node = self.target.parent()
        while node:
            tabs.insert(0, StatsLink(node, Template.headingTab))
            node = node.parent()
        tabs.insert(0, Template.headingTab(href='/')['CIA'])
        return tabs

### The End ###
