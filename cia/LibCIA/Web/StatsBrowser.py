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
from twisted.web import resource, error
from Nouvelle import tag, place


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
    """An anchor tag linking to the given stats target.
       Text for the link may be specified, but by default the
       target's title is used.
       """
    def __init__(self, target, tagFactory=tag('a'), text=None):
        self.target = target
        self.tagFactory = tagFactory
        self.text = text

    def getURL(self, context):
        return context['statsRootPath'] + '/'.join(self.target.pathSegments)

    def render(self, context):
        text = self.text
        if text is None:
            text = self.target.getTitle()
        return self.tagFactory(href=self.getURL(context))[text]


class MetadataLink:
    """An anchor tag linking to an item in the given stats target's metadata.
       Text for the link may be specified, but by default the key is used.

       This class only works for keys that are strings. A key of None links
       to the metadata index.
       """
    def __init__(self, target, key=None, tagFactory=tag('a'), text=None):
        self.target = target
        self.tagFactory = tagFactory
        self.key = key
        self.text = text

    def getURL(self, context):
        url = context['statsRootPath'] + '/'.join(self.target.pathSegments) + '/.metadata'
        if self.key is not None:
            url += '/' + self.key
        return url

    def render(self, context):
        text = self.text
        if text is None:
            if self.key is None:
                text = "View/Edit Metadata"
            else:
                text = self.key
        return self.tagFactory(href=self.getURL(context))[text]


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


class TargetSubTargetsColumn(Nouvelle.Column):
    """A Column displaying the number of subtargets within a particular target"""
    heading = "contents"

    def getValue(self, target):
        return len(target.catalog())

    def isVisible(self, context):
        # Hide this column if none of the targets have children
        return context['table'].reduceColumn(self, max) > 0

    def render_data(self, context, target):
        return "%d items" % self.getValue(target)


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
            TargetSubTargetsColumn(),
            ])]


class Info(Template.Section):
    """A section that displays a StatsTarget's miscellaneous metadata"""
    title = "information"

    def __init__(self, target):
        self.target = target
        self.metadata = target.metadata
        self.url = self.metadata.get('url')
        self.description = self.metadata.get('description')
        self.hasPhoto = 'photo' in self.metadata

    def isVisible(self, context):
        return self.url or self.description or self.hasPhoto

    def render_rows(self, context):
        rows = []
        if self.url:
            rows.append(tag('a', href=self.url)[self.url])
        if self.hasPhoto:
            rows.append(Template.Photo(MetadataLink(self.target, 'photo').getURL(context)))
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
        return [
            Template.Table(self.messages, self.columns,
                           defaultSortReversed = True),
            ]


class RecentMessages(MessageList):
    """A section displaying recent messages from a given stats target"""
    title = "recent messages"

    def __init__(self, target, limit=20):
        messages = target.recentMessages.getLatest(limit)
        messages.reverse()
        MessageList.__init__(self, [Message.Message(m) for m in messages])

    def isVisible(self, context):
        return len(self.messages) != 0


class MetadataKeyColumn(Nouvelle.Column):
    """A column that displays a metadata key as a hyperlink to the proper MetadataValuePage.
       Rows are expected to be (target, key) tuples.
       """
    heading = 'key'

    def getValue(self, item):
        return item[1]

    def render_data(self, context, item):
        if type(item[1]) == str:
            return MetadataLink(item[0], item[1])
        else:
            return repr(item[1])


class MetadataValueColumn(Nouvelle.Column):
    """A column that displays a metadata key's associated value, formatted appropriately.
       Rows are expected to be (target, key) tuples.
       """
    heading = 'value'

    def getValue(self, item):
        """This is used for sorting and such, so don't bother
           returning anything if we're not dealing with text.
           """
        target, key = item
        if target.getType(key).startswith("text/"):
            return target.metadata[key]

    def render_data(self, context, item):
        """Farm this off to an appropriate handler for the data's MIME type"""
        target, key = item
        mime = target.metadata.getType(key)
        try:
            # First look for a handler for one particular MIME type after
            # replacing the / with an underscore
            f = getattr(self, 'renderData_' + mime.replace('/', '_'))
        except AttributeError:
            try:
                # Nope, try looking for a handler for only the content type,
                # not the subtype
                f = getattr(self, 'renderData_' + mime.split('/')[0])
            except AttributeError:
                # Nothing, use a generic handler
                f = self.renderData_other
        return f(context, mime, target, key)

    def renderData_text(self, context, mimeType, target, key):
        return target.metadata[key]

    def renderData_image(self, context, mimeType, target, key):
        """Return an <img> tag linking to the key's value"""
        return tag('img', src=MetadataLink(target, key).getURL(context))

    def renderData_other(self, context, mimeType, target, key):
        return "Unable to format data of type %r" % mimeType


class MetadataTypeColumn(Nouvelle.Column):
    """A column that displays a metadata key's MIME type.
       Rows are expected to be (target, key) tuples.
       """
    heading = 'type'

    def getValue(self, item):
        target, key = item
        return target.metadata.getType(key)


class MetadataSection(Template.Section):
    """A section displaying a table of metadata keys for one stats target"""
    title = "metadata"

    def __init__(self, target):
        self.target = target

    def render_rows(self, context):
        # Each 'row' in our table is actually a (target, key) tuple.
        # Values and types are looked up lazily.
        rows = [(self.target, key) for key in self.target.metadata.keys()]
        return [Template.Table(rows, [
            MetadataKeyColumn(),
            MetadataTypeColumn(),
            MetadataValueColumn(),
            ])]


class MetadataValuePage(resource.Resource):
    """A web resource that returns the raw value of a metadata key, with the proper MIME type"""
    def __init__(self, target, key):
        self.target = target
        self.key = key
        resource.Resource.__init__(self)

    def render(self, request):
        try:
            value = self.target.metadata[self.key]
        except KeyError:
            return error.NoResource("No such metadata key %r" % self.key).render(request)

        request.setHeader('content-type', self.target.metadata.getType(self.key))
        return str(value)


class StatsLinksSection(Template.Section):
    """A section displaying useful links for a particular stats target"""
    title = 'links'

    def __init__(self, target):
        self.target = target

    def render_rows(self, context):
        return [
            MetadataLink(self.target),
            ]


class MetadataPage(Template.Page):
    """A web page providing an interface for viewing and editing a StatsTarget's
       metadata. Children of this page are pages that render individual metadata keys
       with no extra formatting.
       """
    def __init__(self, statsPage):
        self.statsPage = statsPage

    def preRender(self, context):
        context['statsRootPath'] = self.statsPage.findRootPath(context['request'], 1)

    def render_mainTitle(self, context):
        return "Metadata for stats://%s" % "/".join(self.statsPage.target.pathSegments)

    def render_mainColumn(self, context):
        return [
            MetadataSection(self.statsPage.target),
            ]

    def getChildWithDefault(self, name, request):
        """Part of IResource, called by twisted.web to retrieve pages for URIs
           below this one. Children of a MetadataPage are MetadataValuePages
           """
        if not name:
            # Ignore empty path sections
            return self
        else:
            return MetadataValuePage(self.statsPage.target, name)

    def render_headingTabs(self, context):
        tabs = self.statsPage.render_headingTabs(context)
        tabs.append(StatsLink(self.statsPage.target, Template.headingTab))
        return tabs


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
           below this one. This just creates a StatsPage instance for our StatsTarget's child,
           with a few special cases used for metadata and editing.
           """
        if not name:
            # Ignore empty path sections
            return self
        elif name == '.metadata':
            # Return a special page that accesses this stats target's metadata
            return MetadataPage(self)
        else:
            # Return the stats page for a child
            return StatsPage(self.caps, self.storage,
                             self.target.child(name))

    def findRootPath(self, request, additionalDepth=0):
        """Find the URL path referring to the root of the current stats tree
           The returned path begins and ends with a slash.

           additionalDepth can be set to a nonzero number if the caller
           knows the request is actually for a page below this one in the URL tree.
           """
        pathSegments = [s for s in request.path.split('/') if s]
        treeDepth = len(self.target.pathSegments) + additionalDepth
        print "pathsegments %r, treeDepth %r" % (pathSegments, treeDepth)
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
            StatsLinksSection(self.target),
            Clock(),
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
