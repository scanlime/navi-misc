""" LibCIA.Web.Stats.Feed

Pages for getting real-time message feeds in RSS and unformatted XML
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

from twisted.internet import defer
from twisted.protocols import http
from twisted.web import resource, server
from LibCIA import Message, Formatters, TimeUtil, XML
import Nouvelle
import Nouvelle.Twisted
from Nouvelle import tag, place, xml, quote
from LibCIA.Web import Template
import Link


class BaseFeed(Nouvelle.Twisted.Page):
    """Abstract base classes for XML message feeds, using Nouvelle
       to render text/xml pages from a list of recent messages.
       """
    defaultLimit = 20
    mimeType = 'text/xml'

    def __init__(self, statsPage, limit=None):
        if not limit:
            limit = self.defaultLimit
        self.limit = limit
        self.statsPage = statsPage
        self.target = statsPage.target
        Nouvelle.Twisted.Page.__init__(self)

    def preRender(self, context):
        context['component'] = self.statsPage.component
        context['request'].setHeader('content-type', self.mimeType)

    def render(self, request):
        """Intercept the normal rendering operations to check our
           stats target's modification time. If the browser already
           has a recent copy of this feed, we can get away without
           rendering at all.
           """
        self.target.getMTime().addCallback(
            self._render, request
            ).addErrback(request.processingFailed)
        return server.NOT_DONE_YET

    def _render(self, mtime, request):
        if (mtime is None) or (request.setLastModified(mtime) is not http.CACHED):
            # We don't know the mtime or the browser's copy is no good, render as usual
            Nouvelle.Twisted.Page.render(self, request)
        else:
            # Finish without rendering anything
            request.finish()

    def render_items(self, context):
        """Renders the most recent commits as items in the feed"""
	latest = list(self.target.messages.getLatest(self.limit))
	latest.reverse()
        return self.formatItems(latest, context)


class FormattedFeed(BaseFeed):
    """Abstract base classes that apply formatters to each message they include"""
    def __init__(self, statsPage, limit=None, medium='xhtml'):
        self.medium = medium
        BaseFeed.__init__(self, statsPage, limit)

    def formatMessage(self, m, medium=None):
        """Format a message in the requested medium"""
        if medium is None:
            medium = self.medium

        # Look for a proper format_* function to handle special feed-only media
        f = getattr(self, 'format_' + medium, None)
        try:
            if f:
                return f(m)
            else:
                # No special formatter, find a normal one
                return Formatters.getFactory().findMedium(medium, m).formatMessage(m)
        except Message.NoFormatterError:
            return self.formatError()

    def formatError(self):
        """Subclasses should override this to generate error messages for formatMessage"""
        return "(Unable to format message)"

    def format_unquoted(self, m):
        """Our 'unquoted' medium is plaintext inside an xml() marker to prevent quoting"""
        return xml(self.formatMessage(m, 'plaintext'))


class RSSFeed(FormattedFeed):
    """An abstract base class for code shared between versions of the RSS format"""
    def render_photo(self, context):
        # First figure out if we have a photo. Actually render it in the Deferred if we do.
        result = defer.Deferred()
        self.target.metadata.has_key('photo').addCallback(
            self._render_photo, context, result).addErrback(result.errback)
        return result

    def _render_photo(self, hasPhoto, context, result):
        if hasPhoto:
            result.callback(tag('image')[
                tag('url')[ Link.ThumbnailLink(self.target, 'photo', (144,400)).getURL(context) ],
                tag('title')[ place('title') ],
                tag('link')[ place('link') ],
                ])
        else:
            result.callback([])

    def render_title(self, context):
        return self.target.getTitle()

    def render_link(self, context):
        return self.target.metadata.getValue('url', Link.StatsLink(self.target).getURL(context))

    def render_description(self, context):
        return self.target.metadata.getValue('description', 'CIA Stats')


class RSS2Feed(RSSFeed):
    """A web resource representing an RSS 2.0 feed for a particular stats target,
       constructed according to the spec at http://blogs.law.harvard.edu/tech/rss
       """
    def formatItems(self, messages, context):
        items = []
        for id, content in messages:
            items.append(tag('item')[self.messageToItemContent(context, Message.Message(content), id)])
        return items

    def messageToItemContent(self, context, m, id):
        """Render an XML message as the content of an RSS <item>"""
        url = Link.MessageLink(self.target, id).getURL(context)
        tags = [
            tag('pubDate')[ TimeUtil.formatDateRFC822(XML.digValue(m.xml, int, "message", "timestamp")) ],
            tag('guid')[url],
            tag('link')[url],
            tag('description')[ quote(self.formatMessage(m)) ],
            ]

        # Generate a title if we can, but if we can't don't worry too much
        try:
            tags.append(tag('title')[ Formatters.getFactory().findMedium('title', m).formatMessage(m) ])
        except Message.NoFormatterError:
            pass

        return tags

    def render_cloud(self, context):
        """Implements the first step of supporting the RSS 2.0 <cloud> tag.
           This element describes how the RSS aggregator can subscribe for instant
           notification of changes to this resource.
           """
        return tag('cloud',
                   domain            = context['request'].getRequestHostname(),
                   port              = context['request'].host[2],
                   path              = '/RPC2',
                   protocol          = 'xml-rpc',
                   registerProcedure = 'stats.subscribe.rss2',
                   )

    document = [
        xml('<?xml version="1.0"?>\n'),
        tag('rss', version='2.0')[
            tag('channel')[
                tag('title')[ place('title') ],
                tag('link')[ place('link') ],
                tag('description')[ place('description') ],
                place('photo'),
                place('cloud'),
                place('items'),
            ],
        ],
    ]


class RSS1Feed(RSSFeed):
    """A web resource representing an RSS 1.0 feed for a particular stats target,
       constructed according to the standard at http://web.resource.org/rss/1.0/spec

       This uses mostly core RSS 1.0, with the dublin core module for datestamps.
       """
    def messageToItemContent(self, context, m, id):
        return []

    def formatItems(self, messages, context):
        """This is called after we've retrieved a list of the target's recent messages.
           Most of the document can't be formatted until this point, as we need to generate
           both <item> elements and the <items> table of contents.
           """
        # Add our channel description to the content of our <rdf>
        content = [ self.render_channel(context, messages) ]

        # Add <item>s for each message
        for id, messageContent in messages:
            content.append(self.render_item(context, id, messageContent))
        return content

    def render_channel(self, context, messages):
        """Generate our <channel> element and all of its children"""
        # Make a table of contents for the messages
        toc = []
        for id, content in messages:
            url = Link.MessageLink(self.target, id).getURL(context)
            toc.append(tag('rdf:li', resource=url))

        targetUrl = Link.StatsLink(self.target).getURL(context)
        return tag('channel', **{
                   'rdf:about': targetUrl,
               })[
                   tag('title')[ place('title') ],
                   tag('link')[ targetUrl ],
                   tag('description')[ place('description') ],
                   place('photo'),

                   tag('items')[
                       tag('rdf:Seq')[ toc ],
                   ],
               ]

    def render_item(self, context, id, content):
        url = Link.MessageLink(self.target, id).getURL(context)
        m = Message.Message(content)
        tags = [
            tag('link')[ url ],
            tag('dc:date')[ TimeUtil.formatDateISO8601(XML.digValue(m.xml, int, "message", "timestamp")) ],
            tag('description')[ quote(self.formatMessage(m)) ],
            ]

        # Generate a title if we can, but if we can't don't worry too much
        try:
            tags.append(tag('title')[ Formatters.getFactory().findMedium('title', m).formatMessage(m) ])
        except Message.NoFormatterError:
            pass

        return tag('item', **{'rdf:about': url})[tags]

    document = [
        xml('<?xml version="1.0"?>\n'),
        tag('rdf:RDF', **{
            'xmlns:rdf': "http://www.w3.org/1999/02/22-rdf-syntax-ns#",
            'xmlns:dc':  "http://purl.org/dc/elements/1.1/",
            'xmlns':     "http://purl.org/rss/1.0/",
        })[
            place('items'),
        ],
    ]


class XMLFeed(BaseFeed):
    """A web resource representing a feed of unformatted XML commits for a stats target."""
    def formatItems(self, messages, context):
        return [self.formatItem(content) for id, content in messages]

    def formatItem(self, content):
	# Convert the root node, not the document- we don't want to
	# be outputting another XML declaration inside our larger document.
	return xml(XML.toString(content.childNodes[0]))

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

    document = [
        xml('<?xml version="1.0"?>\n'),
        tag('statsTarget')[
            tag('link')[ place('statsLink') ],
            tag('counters')[ place('counters') ],
            tag('metadata')[ place('metadata') ],
            tag('recentMessages') [ place('items') ],
        ],
    ]


class CustomizeRSS(Template.Page):
    """A web page that lets the user generate a customized RSS feed for a particular
       stats target. This can change the format, message style, number of messages, and such.
       """
    def __init__(self, statsPage):
        Template.Page.__init__(self)
        self.statsPage = statsPage

    def parent(self):
        return self.statsPage

    def preRender(self, context):
        context['component'] = self.statsPage.component

    def render_subTitle(self, context):
        return ["for ", self.statsPage.render_mainTitle(context)]

    def render_form(self, context):
        return tag('form',
                   action = Link.RSSLink(self.statsPage.target).getURL(context),
                   )[place('formContent')]

    mainTitle = "Customized RSS"

    leftColumn = [
        Template.StaticSection('information')[
            "This page is a form you can use to tweak everything tweakable about "
            "the way CIA generates RSS feeds. After finding the settings you want, "
            "the submission button at the bottom will redirect you to the customized "
            "RSS feed."
        ],
    ]

    mainColumn = [
        Template.pageBody[ place('form') ],
    ]

    formContent = [
        tag('h1')[ "RSS Format" ],
        tag('p')[
            "There are two current RSS format specifications. Both are named RSS, but "
            "they are actually very different formats with different goals. RSS 2.0 is not "
            "'newer' or 'better' than RSS 1.0 just because 2 is greater than 1; they are "
            "just two separate specifications. CIA gives you the choice of either."
        ],
        tag('div', _class='formChoice')[
            tag('input', _type='radio', value='2', _name='ver', checked='checked'),
            tag('strong')[ " RSS 2.0 " ],
            tag('p')[
                "The default format. RSS 2.0 is simple, and has a publish/subscribe "
                "system that can make it possible to receive updates immediately without "
                "polling. Unfortunately, very few news aggregators currently implement "
                "the <cloud> tag necessary for publish/subscribe. "
            ],
            tag('p')[
                "The CIA server implements the <cloud> tag, so if you have a compatible "
                "aggregator and a globally routable IP address you should see RSS feed "
                "updates almost instantly. However, the only compatible aggregator we're "
                "aware of at the moment is Radio Userland, and it is a commercial product. "
                "The ability to receive RSS updates in real-time would go a long way toward "
                "CIA's goals, so please let us know if you have seen other RSS aggregators "
                "supporting the <cloud> tag."
            ],
        ],
        tag('div', _class='formChoice')[
            tag('input', _type='radio', value='1', _name='ver'),
            tag('strong')[ " RSS 1.0 " ],
            tag('p')[
                "RSS 1.0 is more of an attempt to rethink RSS and design it with extensibility "
                "in mind. It makes use of XML namespaces to provide a core set of functionality "
                "along with 'modules' that can add domain-specific elements or attributes. RSS 1.0 "
                "is based on the RDF (Resource Description Framework) W3C reccomendation, giving "
                "it a rich and well-defined way to represent metadata. "
            ],
            tag('p')[
                "CIA doesn't yet make use of an RSS 1.0 module to provide full metadata on commits. "
                "Until an RSS 1.0 module for commit messages is designed, the best way to get all "
                "possible information from CIA is to use the raw XML feeds."
            ],
            tag('p')[
                "There was a 'changedpage' module in the works for RSS 1.0 that would have provided "
                "features similar to the <cloud> element mentioned above for RSS 2.0, however the page "
                "for it seems to have disappeared. Please contact us with more information if you have any."
            ],
        ],

        tag('h1')[ "Messages" ],
        tag('p')[
            "This section controls which medium CIA tries to format messages in before embedding "
            "them in the RSS feed. The default of XHTML is optimal, but alternatives are provided "
            "if you need them."
        ],
        tag('div', _class='formChoice')[
            tag('input', _type='radio', value='xhtml', _name='medium', checked='checked'),
            tag('strong')[ " XHTML " ],
            tag('p')[
                "Format messages as XHTML with embedded CSS styles. This should "
                "work and look good in most RSS aggregators. "
            ],
        ],
        tag('div', _class='formChoice')[
            tag('input', _type='radio', value='plaintext', _name='medium'),
            tag('strong')[ " Plain Text " ],
            tag('p')[
                "Format messages in plain text, properly quoted for inclusion in RSS. "
                "This is the preferred choice if your RSS aggregator runs on a text-only "
                "console or can't handle HTML. "
            ],
        ],
        tag('div', _class='formChoice')[
            tag('input', _type='radio', value='unquoted', _name='medium'),
            tag('strong')[ " Unquoted Text " ],
            tag('p')[
                "Format messages as plain text, but instead of quoting them twice (once "
                "on account of the RSS feed being in XML, once because the content is "
                "interpreted as HTML) this only quotes the text once. This should "
                "only be used if your RSS aggregator is buggy and does not follow the "
                "specification!"
            ],
        ],
        tag('p')[
            "You can optionally change the maximum number of messages a feed will contain "
            "at once. Leave it blank to use the default of %s. There is no explicit upper limit, "
            "But the database does store a finite number of messages for each stats target. "
            "Please be reasonable. " % BaseFeed.defaultLimit
        ],
        tag('div', _class='formChoice')[
            tag('p')[ "Retrieve at most: " ],
            tag('p')[ tag('input', _type='text', _name='limit', size=10), " messages" ],
        ],

        tag('h1')[ "Your RSS Feed" ],
        tag('p')[
            "This button will now redirect you to an RSS feed with the settings above, "
            "ripe for opening in your favorite RSS aggregator or copying and pasting somewhere useful."
        ],
        tag('p')[
            tag('input', _type='submit', value='Get my customized RSS feed'),
        ],
    ]


class RSSFrontend(resource.Resource):
    """A web resource representing an RSS feed, the format and content of which depends
       on parameters passed to us. Children are supported- for now this includes the
       'customize' page that helps you build RSS URLs with non-default options.
       """
    def __init__(self, statsPage):
        resource.Resource.__init__(self)
        self.statsPage = statsPage
        self.putChild('customize', CustomizeRSS(statsPage))

    def render(self, request):
        # Check the requested RSS version
        version = request.args.get('ver')
        if version and version[-1]:
            # Pick a feed factory based on the RSS version
            factory = {
                '1': RSS1Feed,
                '2': RSS2Feed,
                }[version[-1]]
        else:
            # Use RSS 2 by default
            factory = RSS2Feed

        # The rest of the arguments get transformed into keyword args
        kwargs = {}

        # Check the requested message medium, defaulting to XHTML
        medium = request.args.get('medium')
        if medium and medium[-1]:
            kwargs['medium'] = medium[-1]

        # Get the message limit, with BaseFeed's default
        limit = request.args.get('limit')
        if limit and limit[-1]:
            kwargs['limit'] = int(limit[-1])

        # Now construct the proper feed object and render it
        return factory(self.statsPage, **kwargs).render(request)

### The End ###
