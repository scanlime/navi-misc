""" LibCIA.Web.Stats.Feed

Pages for getting real-time message feeds in RSS and unformatted XML
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
from LibCIA import Message, Formatters
import Nouvelle
import Nouvelle.Twisted
from Nouvelle import tag, place, xml
import Link


class BaseFeed(Nouvelle.Twisted.Page):
    """Abstract base classes for XML message feeds, using Nouvelle
       to render text/xml pages from a list of recent messages.
       """
    def __init__(self, statsPage):
        self.statsPage = statsPage
        self.target = statsPage.target
        Nouvelle.Twisted.Page.__init__(self)

    def preRender(self, context):
        context['component'] = self.statsPage.component
        context['request'].setHeader('content-type', 'text/xml')

    def render_title(self, context):
        return self.target.getTitle()

    def render_link(self, context):
        return self.target.metadata.getValue('url', Link.StatsLink(self.target).getURL(context))

    def render_description(self, context):
        return self.target.metadata.getValue('description', 'CIA Stats')

    def render_items(self, context, limit=20):
        """Renders the most recent commits as items in the RSS feed"""
        # Get the messages, render them in our Deferred
        result = defer.Deferred()
        self.target.messages.getLatest(limit).addCallback(
            self.formatItems, context, result).addErrback(result.errback)
        return result


class RSSFeed(BaseFeed):
    """A web resource representing an RSS feed for a particular stats target."""
    def render_photo(self, context):
        # First figure out if we have a photo. Actually render it in the Deferred if we do.
        result = defer.Deferred()
        self.target.metadata.has_key('photo').addCallback(
            self._render_photo, context, result).addErrback(result.errback)
        return result

    def _render_photo(self, hasPhoto, context, result):
        if hasPhoto:
            result.callback(tag('image')[
                tag('url')[ Link.MetadataLink(self.target, 'photo').getURL(context) ],
                tag('title')[ place('title') ],
                tag('link')[ place('link') ],
                ])
        else:
            result.callback([])

    def formatItems(self, messages, context, result):
        items = []
        for m in messages:
            try:
                m = Message.Message(m)
                items.append(Formatters.factory.findMedium('rss', m).format(m))
            except Message.NoFormatterError:
                # We can't find a formatter, stick in a placeholder noting this fact
                items.append(tag('item')[ tag('description')[
                    "(Unable to format message)"
                    ]])
        result.callback(items)

    document = tag('rss', version='2.0')[ tag('channel')[
        tag('title')[ place('title') ],
        tag('link')[ place('link') ],
        tag('ttl')[ 15 ],
        tag('description')[ place('description') ],
        place('photo'),
        place('items'),
        ]]


class XMLFeed(BaseFeed):
    """A web resource representing a feed of unformatted XML commits for a stats target."""
    def formatItems(self, messages, context, result):
        result.callback(map(xml, messages))

    def render_metadata(self, context):
        # Look up all the metadata first
        result = defer.Deferred()
        self.target.metadata.dict().addCallback(
            self._render_metadata, context, result).addErrback(result.errback)
        return result

    def _render_metadata(self, metadict, context, result):
        result.callback([self.renderMetadataItem(name, t[0], t[1], context)
                         for name, t in metadict.iteritems()])

    def renderMetadataItem(self, name, value, mimeType, context):
        """Render a single metadata item. If the content is short and in
           a text format, we include it directly. Otherwise, just link to it.
           """
        if mimeType.startswith('text/') and len(value) < 1024:
            valueTag = tag('value', _type=mimeType)[ value ]
        else:
            valueTag = tag('url')[ Link.MetadataLink(self.target, name).getURL(context) ]
        return tag('item', _name=name)[ valueTag ]

    def render_counters(self, context):
        # Look up all the counters first
        result = defer.Deferred()
        self.target.counters.dict().addCallback(
            self._render_counters, context, result).addErrback(result.errback)
        return result

    def _render_counters(self, counterdict, context, result):
        tags = []
        for name, valueDict in counterdict.iteritems():
            eventCount = valueDict.get('eventCount', 0)
            try:
                del valueDict['eventCount']
            except KeyError:
                pass
            tags.append(tag('counter', _name = name, **valueDict)[ eventCount ])
        result.callback(tags)

    def render_statsLink(self, context):
        return Link.StatsLink(self.target).getURL(context)

    document = tag('statsTarget')[
        tag('link')[ place('statsLink') ],
        tag('counters')[ place('counters') ],
        tag('metadata')[ place('metadata') ],
        tag('recentMessages') [ place('items') ],
        ]

### The End ###
