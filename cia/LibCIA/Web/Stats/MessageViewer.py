""" LibCIA.Web.Stats.MessageViewer

An interface for viewing the individual messages stored by the stats subsystem
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

from twisted.web import resource, error
from twisted.internet import defer
from LibCIA.Web import Template
from Nouvelle import tag, place
from LibCIA import Formatters, Message, XML
import Link


class RootPage(resource.Resource):
    """A page that doesn't generate any interesting output, but whose children are message IDs"""
    def __init__(self, statsPage):
        self.statsPage = statsPage
        resource.Resource.__init__(self)

    def render(self, request):
        return error.NoResource("There's no index here, you need a message ID").render(request)

    def getChildWithDefault(self, name, request):
        if not name:
            # Ignore empty path sections
            return self
        else:
            return MessagePage(self.statsPage, int(name))


class LinksSection(Template.Section):
    """A section displaying useful links for a particular message"""
    title = 'links'

    def __init__(self, target, messageId):
        self.target = target
        self.messageId = messageId

    def render_rows(self, context):
        return [
            Link.MessageLink(self.target, self.messageId,
                             extraSegments = ('printable',),
                             text = 'Printable',
                             ),
            Link.MessageLink(self.target, self.messageId,
                             extraSegments = ('pretty-xml',),
                             text = 'Pretty-printed XML',
                             ),
            Link.MessageLink(self.target, self.messageId,
                             extraSegments = ('xml',),
                             text = 'Raw XML',
                             ),
            ]


class MessagePage(Template.Page):
    """A page that views one message from the stats database"""
    isLeaf = 1

    def __init__(self, statsPage, id):
        self.statsPage = statsPage
        self.id = id

    def parent(self):
        return self.statsPage

    def preRender(self, context):
        context['component'] = self.statsPage.component

    def render_mainTitle(self, context):
        return "Message #%d" % self.id

    def render_subTitle(self, context):
        return ["for ",
                self.statsPage.render_mainTitle(context)]

    def render_message(self, context):
        # Grab the message from our database first
        result = defer.Deferred()
        self.statsPage.target.messages.getMessageById(self.id).addCallback(
            self._render_message, context, result).addErrback(result.errback)
        return result

    def _render_message(self, xml, context, result):
        # No message?
        if not xml:
            result.callback(self.notFoundMessage)
            return
        m = Message.Message(xml)

        # Try to format it using several media, in order of decreasing preference.
        # The 'xhtml-long' formatter lets messages define a special formatter to
        # use when an entire page is devoted to their one message, possibly showing
        # it in greater detail. 'xhtml' is the formatter most messages should have.
        # 'plaintext' is a nice fallback.
        for medium in ('xhtml-long', 'xhtml', 'plaintext'):
            try:
                formatted = Formatters.factory.findMedium(medium, m).format(m)
            except Message.NoFormatterError:
                continue
            result.callback(formatted)
            return

        # Still no luck? Display a warning message and a pretty-printed XML tree
        result.callback([
            tag('h1')[ "No formatter available" ],
            XML.htmlPrettyPrint(m.xml),
            ])

    def render_leftColumn(self, context):
        return [
            LinksSection(self.statsPage.target, self.id),
            ]

    notFoundMessage = [
        tag('h1')[ "Not Found" ],
        tag('p')[
            "This message was not found in our database. The number could be "
            "incorrect, or the message may be old enough that it was deleted "
            "from the database. Each stats target archives a fixed number of messages, "
            "so you might be able to find another copy of it on a different stats "
            "target with less traffic."
        ]
    ]

    mainColumn = [
        Template.pageBody[ place('message') ],
        ]

### The End ###
