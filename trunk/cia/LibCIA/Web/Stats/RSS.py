""" LibCIA.Web.Stats.RSS

Implement an RSS 2.0 feed showing the latest commits from a particular stats target
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
from LibCIA import Message
import Nouvelle
import Nouvelle.Twisted
from Nouvelle import tag, place
import Link


class RSSFeed(Nouvelle.Twisted.Page):
    """A web resource representing an RSS feed for a particular stats target.
       We use Nouvelle here to generate RSS rather than XHTML :)
       """
    def __init__(self, statsPage):
        self.statsPage = statsPage
        self.target = statsPage.target
        Nouvelle.Twisted.Page.__init__(self)

    def preRender(self, context):
        context['request'].setHeader('content-type', 'text/xml')
        context['statsRootPath'] = self.statsPage.findRootPath(context['request'], 1, absolute=True)

    def render_title(self, context):
        return self.target.getTitle()

    def render_link(self, context):
        return self.target.metadata.get('url', Link.StatsLink(self.target).getURL(context))

    def render_description(self, context):
        return self.target.metadata.get('description', 'CIA Stats')

    def render_photo(self, context):
        # First figure out if we have a photo. Actually render it in the Deferred if we do.
        result = defer.Deferred()
        self.target.metadata.has_key('photo').addCallback(
            self._render_photo, context, result).addErrback(result.errback)
        return result

    def _render_photo(self, hasPhoto, context, result):
        if hasPhoto:
            result.callback(tag('image')[
                tag('url')[ MetadataLink(self.target, 'photo').getURL(context) ],
                tag('title')[ place('title') ],
                tag('link')[ place('link') ],
                ])
        else:
            result.callback([])

    def render_items(self, context, limit=15):
        """Renders the most recent commits as items in the RSS feed"""
        # Get the messages, render them in our Deferred
        result = defer.Deferred()
        self.target.messages.getLatest(limit).addCallback(
            self._render_items, context, result).addErrback(result.errback)
        return result

    def _render_items(self, messages, context, result):
        formatter = Message.AutoFormatter('rss')
        items = []
        for m in messages:
            i = formatter.format(Message.Message(m))
            if i:
                items.append(i)
            else:
                # We can't find a formatter, stick in a placeholder noting this fact
                items.append(tag('item')[ tag('description')[
                    "(Unable to format message)"
                    ]])
        result.callback(items)

    document = tag('rss', version='2.0')[ tag('channel')[
        tag('title')[ place('title') ],
        tag('link')[ place('link') ],
        tag('description')[ place('description') ],
        place('photo'),
        place('items'),
        ]]

### The End ###
