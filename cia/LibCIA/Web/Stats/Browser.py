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
from twisted.web import resource
from LibCIA.Web import Template, Info, Server
from LibCIA import Stats, Message, TimeUtil, Formatters, Database
from Nouvelle import tag, place
import Nouvelle, time, sys, posixpath
import Metadata, Catalog, Feed, Link, MessageViewer


class Component(Server.Component):
    """A server component representing the whole stats interface"""
    name = "Stats"

    def __init__(self):
        self.resource = Page(self)

    def __contains__(self, page):
        for cls in (Page,
                    Metadata.MetadataPage,
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
            '.metadata': Metadata.MetadataPage,
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
            return self.__class__(self.component, self.target.child(name))

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
            RelatedSection(self.target),
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
        try:
            return Formatters.factory.findMedium('xhtml', message).format(message)
        except Message.NoFormatterError:
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
        result = defer.Deferred()
        self.target.messages.getLatest(self.limit).addCallback(
            self._render_rows, context, result).addErrback(result.errback)
        return result

    def _render_rows(self, messages, context, result):
        """Actually render the rows, called after the message list has been retrieved"""
        if messages:
            # Parse the messages and attach URLs to them
            parsed = []
            for id, xml in messages:
                m = Message.Message(xml)
                m.hyperlink = Link.MessageLink(self.target, id, text="#%d" % id)
                parsed.append(m)
            result.callback(self.renderMessages(context, parsed))
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
            Link.RSSCustomizer(self.target),
            Link.XMLLink(self.target),
            ]


class RelatedSection(Template.Section):
    """A section showing links to related stats targets. This works by looking for
       nodes connected to this one in the stats_relations graph. The paths and
       titles of related nodes are fetched using one SQL query, for efficiency.

       The query sorts first by parent path, so we can extract each section in one
       piece, and second in descending order by freshness. This way, instead of
       constantly having the strongest associations at the top, every time an
       association is reinforced it pops up to the top, showing our visitors what's
       cool and hip.
       """
    title = 'related'

    query = """
    SELECT
        C.parent_path,
        PARENT_TITLE.value,
        C.target_path,
        TARGET_TITLE.value
    FROM stats_relations R
        LEFT OUTER JOIN stats_catalog C
            ON (C.target_path = IF(R.target_a_path = %(path)s, R.target_b_path, R.target_a_path))
        LEFT OUTER JOIN stats_metadata TARGET_TITLE
            ON (TARGET_TITLE.name = 'title' AND TARGET_TITLE.target_path = C.target_path)
        LEFT OUTER JOIN stats_metadata PARENT_TITLE
            ON (PARENT_TITLE.name = 'title' AND PARENT_TITLE.target_path = C.parent_path)
        WHERE (R.target_a_path = %(path)s or R.target_b_path = %(path)s)
            AND C.parent_path != %(path)s
    ORDER BY C.parent_path, R.freshness DESC
    """

    sectionLimit = 15

    def __init__(self, target):
        self.target = target

    def makeLink(self, path, title):
        """Link to a stats target when we already know the title"""
        target = Stats.Target.StatsTarget(path)
        if title is None:
            title = target.name
        return Link.StatsLink(target, text=title)

    def render_rows(self, context):
        # Run our big SQL query to get all data for this section first
        result = defer.Deferred()
        Database.pool.runQuery(self.query % {
            'path': Database.quote(self.target.path, 'varchar'),
            }).addCallback(
            self._render_rows, context, result
            ).addErrback(result.errback)
        return result

    def _render_rows(self, queryResults, context, result):
        # From the rows returned from our SQL query, construct a
        # dictionary that maps from a parent hyperlink to a list
        # of child hyperlinks sorted by decreasing freshness.
        currentParentLink = None
        currentParentPath = None
        d = {}
        for parentPath, parentTitle, targetPath, targetTitle in queryResults:
            if parentPath != currentParentPath:
                currentParentPath = parentPath
                currentParentLink = self.makeLink(parentPath, parentTitle)
            d.setdefault(currentParentLink, []).append(self.makeLink(targetPath, targetTitle))

        # Sort these parent sections by decreasing size. We want
        # the most interesting ones at the top, and those are usually the biggest.
        sections = d.keys()
        sections.sort(lambda a,b: cmp(len(d[b]), len(d[a])))
        result.callback([self.render_section(section, d[section]) for section in sections])

    def render_section(self, section, contents):
        """Given a heading renderable and a list of contents for that
           heading, render one section of the 'related' box.
           """
        # Truncate the contents if we need to
        if len(contents) > self.sectionLimit:
            contents = contents[:self.sectionLimit] + ['(%d others)' % (len(contents) - self.sectionLimit)]

        return [
            tag('div', _class='relatedHeading')[ section ],
            tag('ul', _class='related')[[
                tag('li', _class='related')[ item ]
                for item in contents
            ]],
        ]

### The End ###
