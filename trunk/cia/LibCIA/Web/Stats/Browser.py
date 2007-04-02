""" LibCIA.Web.Stats.Browser

Implements CIA's actual stats browser pages. This uses Sections provided
by other modules in this package, and detects magic URLs that rediect one
to metadata or RSS pages.
"""
#
# CIA open source notification system
# Copyright (C) 2003-2007 Micah Dowty <micah@navi.cx>
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

from twisted.python import log
from twisted.internet import defer
from twisted.protocols import http
from twisted.web import error, resource, server
from LibCIA.Web import Template, Info, Server
from LibCIA import Stats, Message, TimeUtil, Formatters, XML
from Nouvelle import tag, place
import Nouvelle, time, sys, posixpath, re
import Metadata, Catalog, Feed, Link, MessageViewer, Graph


class Component(Server.Component):
    """A server component representing the whole stats interface"""
    name = "Stats"

    def __init__(self):
        self.resource = Page(self)

    def __contains__(self, page):
        for cls in (Page,
                    Feed.CustomizeRSS,
                    MessageViewer.MessagePage,
                    ):
            if isinstance(page, cls):
                return True
        return False


class Page(Template.Page):
    """A web page providing an interface to one StatsTarget.
       The root of the stats namespace should be created with the
       capabilities database and StatsStorage. Children will
       be automatically created with child targets.
       """
    def __init__(self, component, target=None):
        if target is None:
            target = Stats.Target.StatsTarget()
        self.component = component
        self.target = target

    def parent(self):
        parentTarget = self.target.parent()
        if parentTarget:
            return self.__class__(self.component, parentTarget)

    def getURL(self, context):
        return posixpath.join(self.component.url, self.target.path)

    def getChildWithDefault(self, name, request):
        """Part of IResource, called by twisted.web to retrieve pages for URIs
           below this one. This just creates a Page instance for our StatsTarget's child,
           with a few special cases used for metadata and editing.
           """
        childFactories = {
            '.message':  MessageViewer.RootPage,
            '.rss':      Feed.RSSFrontend,
            '.xml':      Feed.XMLFeed,
            }

        if not name:
            # Ignore empty path sections
            return self
        elif name in childFactories:
            return childFactories[name](self)
        else:
            # Return the stats page for a child
            try:
                child = self.target.child(name)
            except ValueError:
                # Reserved word in stats target
                return error.NoResource("Invalid stats path")
            else:
                return self.__class__(self.component, child)

    def preRender(self, context):
        context['component'] = self.component

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
            Graph.RelatedSection(self.target),
            Info.Clock(),
            ]

    def render_extraHeaders(self, context):
        # Add a <link> tag pointing at our RSS feed. Some RSS
        # aggregators can use this to automatically detect feeds.
        return tag('link',
                   rel   = 'alternate',
                   type  = 'application/rss+xml',
                   title = 'RSS',
                   href  = Link.RSSLink(self.target).getURL(context),
                   )


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
	try:
            return XML.digValue(message.xml, int, "message", "timestamp")
        except ValueError:
            return None

    def render_data(self, context, message):
        value = self.getValue(message)
        if value:
            return TimeUtil.formatRelativeDate(value)
        else:
            return Template.error[ "Invalid Date" ]


class MessageProjectColumn(Nouvelle.Column):
    """A column that displays the project a message originated from"""
    heading = 'project'

    def getValue(self, message):
        project = XML.dig(message.xml, "message", "source", "project")
        if project:
            return XML.shallowText(project)


class MessageContentColumn(Nouvelle.Column):
    """A column that displays a message, formatted in XHTML"""
    heading = 'content'

    def getValue(self, message):
        try:
            return Formatters.getFactory().findMedium('xhtml', message).formatMessage(message)
        except:
            return Template.error[str(sys.exc_info()[1])]


class MessageList(Template.Section):
    """A list of messages, with metadata and hyperlinks. Must be
       constructed with a list of Message instances.
       """
    title = "messages"

    columns = [
        MessageDateColumn(),
        MessageProjectColumn(),
        MessageContentColumn(),
        Nouvelle.AttributeColumn('link', 'hyperlink'),
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
        parsed = []
        for id, xml in self.target.messages.getLatest(self.limit):
            m = Message.Message(xml)
            m.hyperlink = Link.MessageLink(self.target, id, text="#")
            parsed.append(m)

        if parsed:
            return self.renderMessages(context, parsed)
        else:
            return []


class LinksSection(Template.Section):
    """A section displaying useful links for a particular stats target. All links
       are classes in the Link module that take only our stats target as a constructor argument.
       We have a list of allowed links and default links, but the exact links we display may
       be modified by the links-filter metadata key.

       The links-filter format is simple- one entry per line, each entry consists of an
       action and a link name regex, separated by whitespace. The action can be "+" to add the
       link(s) or "-" to remove the link(s).
       """
    title = 'syndicate'

    availableLinkNames = [
        "RSSLink",
        "RSSCustomizer",
        "XMLLink",
        ]

    defaultLinkNames = [
        "RSSLink",
        "RSSCustomizer",
        "XMLLink",
        ]

    def __init__(self, target):
        self.target = target

    def render_rows(self, context):
        # First look for a links-filter metadata key for this target.
        # The default is empty.
        result = defer.Deferred()
        self.target.metadata.getValue("links-filter", default="").addCallback(
            self._render_rows, context, result
            ).addErrback(result.errback)
        return result

    def _render_rows(self, linksFilter, context, result):
        linkNames = list(self.defaultLinkNames)

        for line in linksFilter.split("\n"):
            line = line.strip()
            if line:
                action, name = line.split(None, 1)
                regex = re.compile(name)

                if action == "+":
                    for available in self.availableLinkNames:
                        if regex.match(available):
                            linkNames.append(available)

                elif action == "-":
                    filtered = []
                    for name in linkNames:
                        if not regex.match(name):
                            filtered.append(name)
                    linkNames = filtered

        result.callback([getattr(Link, linkName)(self.target) for linkName in linkNames])

### The End ###
