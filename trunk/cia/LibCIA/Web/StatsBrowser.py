""" LibCIA.Web.StatsBrowser

A web interface, built using nevow, for CIA's stats:// namespace
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

import time
import Template, Base
from LibCIA import TimeUtil, Message
from Base import tag, place


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
                   Template.counterValue[ place('relativeDate', 'forever', 'lastEventTime') ],
                   ' ago at ',
                   Template.counterValue[ place('date', 'forever', 'lastEventTime') ],
               ],
               [
                   Template.counterValue[ place('value', 'today', 'eventCount') ],
                   ' messages so far today, ',
                   Template.counterValue[ place('value', 'yesterday', 'eventCount') ],
                   ' messages yesterday',
               ],
               [
                   Template.counterValue[ place('value', 'thisWeek', 'eventCount') ],
                   ' messages so far this week, ',
                   Template.counterValue[ place('value', 'lastWeek', 'eventCount') ],
                   ' messages last week',
               ],
               [
                   Template.counterValue[ place('value', 'thisMonth', 'eventCount') ],
                   ' messages so far this month, ',
                   Template.counterValue[ place('value', 'lastMonth', 'eventCount') ],
                   ' messages last month',
               ],
               [
                   Template.counterValue[ place('value', 'forever', 'eventCount') ],
                   ' messages since the first one, ',
                   Template.counterValue[ place('relativeDate', 'forever', 'firstEventTime') ],
                   ' ago',
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


class StatsLink:
    """An anchor tag linking to the given stats target"""
    def __init__(self, target, tagFactory=tag('a')):
        self.target = target
        self.tagFactory = tagFactory

    def render(self, context):
        url = context['statsRootPath'] + '/'.join(self.target.pathSegments)
        return self.tagFactory(href=url)[self.target.getTitle()]


class Catalog(Template.Section):
    """A Section displaying links to all children of a StatsTarget"""
    title = "catalog"
    rows = [Template.catalogList[ place('items') ]]

    def __init__(self, target):
        self.target = target
        self.names = target.catalog()
        self.names.sort(lambda a,b: cmp(a.lower(), b.lower()))

    def isVisible(self, context):
        return len(self.names) != 0

    def render_items(self, context):
        """Sort the child stats targets case-insensitively and wrap each in a StatsLink"""
        return [tag('li')[StatsLink(self.target.child(name))] for name in self.names]


class MessageList(Template.Section):
    """A list of messages, with metadata and hyperlinks. Must be
       constructed with a list of Message instances.
       """
    title = "messages"
    rows = [ tag('table')[
               tag('tr')[
                   tag('th')[ 'date' ],
                   tag('th')[ 'project' ],
                   tag('th')[ 'message' ],
               ],
               place('tableRows'),
             ]
           ]

    def __init__(self, messages):
        self.messages = messages
        self.formatter = Message.AutoFormatter('xhtml')

    def render_tableRows(self, context):
        return [tag('tr')[self.renderMessage(m)] for m in self.messages]

    def renderMessage(self, message):
        try:
            date = TimeUtil.formatDate(int(str(message.xml.timestamp)))
        except:
            date = None

        try:
            project = str(message.xml.source.project)
        except:
            project = None

        message = Base.xml(self.formatter.format(message))

        return [
            tag('td')[ date ],
            tag('td')[ project ],
            tag('td')[ message ],
            ]


class RecentMessages(MessageList):
    """A section displaying recent messages from a given stats target"""
    title = "recent messages, newest first"

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

    def render_mainColumn(self, context):
        return [
            Counters(self.target),
            RecentMessages(self.target),
            Catalog(self.target),
            ]

    def render_leftColumn(self, context):
        return [
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
