""" LibCIA.Web.Stats.MessageViewer

An interface for viewing the individual messages stored by the stats subsystem
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

from twisted.web import resource, error, server
from twisted.internet import defer
from LibCIA.Web import Template, Info
from Nouvelle import tag, place
from LibCIA import Formatters, Message, XML, TimeUtil
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
            try:
                return MessagePage(self.statsPage, int(name, 16))
            except ValueError:
                return error.NoResource("Message ID is invalid")


class EnvelopeSection(Template.Section):
    """A section displaying general information contained in a message's envelope"""
    title = 'envelope'

    def __init__(self, target, message):
        self.target = target
        self.message = message

    def render_rows(self, context):
        if not self.message:
            return []
        rows = []

        timestamp = XML.dig(self.message.xml, "message", "timestamp")
        if timestamp:
            rows.append(self.format_timestamp(timestamp))

        generator = XML.dig(self.message.xml, "message", "generator")
        if generator:
            rows.append(self.format_generator(generator))

        return rows

    def format_generator(self, gen):
        """Format the information contained in this message's <generator> tag"""
        name = XML.digValue(gen, str, "name")
        url = XML.digValue(gen, str, "url")
        version = XML.digValue(gen, str, "version")

        if url:
            name = tag('a', href=url)[ name ]
        items = ["Generated by ", Template.value[ name ]]
        if version:
            items.extend([" version ", version])
        return items

    def format_timestamp(self, stamp):
        return ["Received ", Template.value[TimeUtil.formatRelativeDate(int(XML.shallowText(stamp)))]]


class LinksSection(Template.Section):
    """A section displaying useful links for a particular message"""
    title = 'links'

    def __init__(self, target, messageId):
        self.target = target
        self.messageId = messageId

    def render_rows(self, context):
        return [
            Link.MessageLink(self.target, self.messageId,
                             extraSegments = ('xml',),
                             text = 'Unformatted XML',
                             ),
            ]


class UnformattedMessagePage(resource.Resource):
    """A page that sends back the raw XML text of a particular message"""
    def __init__(self, target, id):
        self.target = target
        self.id = id
        resource.Resource.__init__(self)

    def render(self, request):
        xml = self.target.messages.getMessageById(self.id)
        if xml:
            request.setHeader('content-type', 'text/xml')
            request.write(unicode(XML.toString(xml)).encode('utf-8'))
            request.finish()
        else:
            request.write(error.NoResource("Message not found").render(request))
            request.finish()


class MessagePage(Template.Page):
    """A page that views one message from the stats database"""
    mainTitle = 'Archived Message'

    def __init__(self, statsPage, id):
        self.statsPage = statsPage
        self.id = id
        Template.Page.__init__(self)
        self.putChild('xml', UnformattedMessagePage(self.statsPage.target, self.id))

    def parent(self):
        return self.statsPage

    def preRender(self, context):
        # Load the message once, so multiple components can share it
        xml = self.statsPage.target.messages.getMessageById(self.id)
        if xml:
            self.message = Message.Message(xml)
        else:
            self.message = None

        context['component'] = self.statsPage.component

    def render_subTitle(self, context):
        return ["for ",
                self.statsPage.render_mainTitle(context)]

    def render_message(self, context):
        if not self.message:
            context['request'].setResponseCode(404)
	    return self.notFoundMessage

        # Try to format it using several media, in order of decreasing preference.
        # The 'xhtml-long' formatter lets messages define a special formatter to
        # use when an entire page is devoted to their one message, possibly showing
        # it in greater detail. 'xhtml' is the formatter most messages should have.
        # 'plaintext' is a nice fallback.
        #
        # This default list of media to try can be overridden with an argument in our URL.

        if 'media' in context['args']:
            mediaList = context['args']['media'][0].split()
        else:
            mediaList = ('xhtml-long', 'xhtml', 'plaintext')

        for medium in mediaList:
            try:
                formatted = Formatters.getFactory().findMedium(
                    medium, self.message).formatMessage(self.message)
            except Message.NoFormatterError:
                continue
            return formatted

        # Still no luck? Display a warning message and a pretty-printed XML tree
        return [
            tag('h1')[ "No formatter available" ],
            XML.htmlPrettyPrint(self.message.xml),
            ]

    def render_leftColumn(self, context):
        return [
            EnvelopeSection(self.statsPage.target, self.message),
            LinksSection(self.statsPage.target, self.id),
            Info.Clock(),
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
