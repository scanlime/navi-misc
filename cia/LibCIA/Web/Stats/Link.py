""" LibCIA.Web.Stats.Link

Classes for forming hyperlinks between stats browser pages
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

from Nouvelle import tag
from urllib import quote
from LibCIA.Web import Template
import posixpath


class TargetRelativeLink:
    """Abstract base class for a link to a stats target or something relative to it"""
    def __init__(self, target, relativePathSegments=()):
        self.target = target
        self.relativePathSegments = tuple(relativePathSegments)

    def getURL(self, context):
        # Make this an absolute URL- currently it's required for
        # links placed in the RSS and XML feeds, and won't
        # hurt elsewhere.
        req = context['request']
        port = req.host[2]
        hostname = req.getRequestHostname()
        if req.isSecure():
            default = 443
        else:
            default = 80
        if port == default:
            hostport = ''
        else:
            hostport = ':%d' % port
        path = posixpath.join(context['component'].url,
                              *(tuple(self.target.pathSegments) + self.relativePathSegments))
        return quote('http%s://%s%s%s' % (
            req.isSecure() and 's' or '',
            hostname,
            hostport,
            path), "/:")


class StatsLink(TargetRelativeLink):
    """An anchor tag linking to the given stats target.
       Text for the link may be specified, but by default the
       target's title is used.
       """
    def __init__(self, target, tagFactory=tag('a'), text=None):
        TargetRelativeLink.__init__(self, target)
        self.tagFactory = tagFactory
        self.text = text

    def render(self, context):
        text = self.text
        if text is None:
            text = self.target.getTitle()
        return self.tagFactory(href=self.getURL(context))[text]


class MessageLink(TargetRelativeLink):
    """A link to a particular message delivered to a stats target"""
    def __init__(self, target, id, extraSegments=(), tagFactory=tag('a'), text=None):
        TargetRelativeLink.__init__(self, target, ('.message', "%x" % id) + extraSegments)
        self.tagFactory = tagFactory
        self.text = text

    def render(self, context):
        return self.tagFactory(href=self.getURL(context))[self.text]


class MetadataLink(TargetRelativeLink):
    """An anchor tag linking to an item in the given stats target's metadata.
       Text for the link may be specified, but by default the key is used.

       This class only works for keys that are strings. A key of None links
       to the metadata index.
       """
    def __init__(self, target, key=None, tagFactory=tag('a'), text=None):
        segments = ['.metadata']
        if key:
            segments.append(key)
        TargetRelativeLink.__init__(self, target, segments)

        self.tagFactory = tagFactory
        self.key = key
        self.text = text

    def render(self, context):
        text = self.text
        if text is None:
            if self.key is None:
                text = "View/Edit Metadata"
            else:
                text = self.key
        return self.tagFactory(href=self.getURL(context))[text]


class RSSLink(TargetRelativeLink):
    """An anchor tag linking to the default RSS feed for a particular stats target"""
    def __init__(self, target, text="RSS 2.0 Feed", extraSegments=()):
        TargetRelativeLink.__init__(self, target, ('.rss',) + extraSegments)
        self.text = text

    def render(self, context):
        return Template.SubscriptionLink(self.getURL(context), self.text)


class RSSCustomizer(RSSLink):
    """An anchor tag leading to a page that lets the user customize the generated RSS"""
    def __init__(self, target, text="Customized RSS"):
        RSSLink.__init__(self, target, text, ('customize',))

    def render(self, context):
        return tag('a', href=self.getURL(context))[self.text]


class XMLLink(TargetRelativeLink):
    """An anchor tag linking to the XML feed for a given stats target"""
    def __init__(self, target, tagFactory=tag('a'), text=None):
        TargetRelativeLink.__init__(self, target, ('.xml',))
        self.tagFactory = tagFactory
        self.text = text

    def render(self, context):
        text = self.text
        if text is None:
            text = "Unformatted XML"
        return self.tagFactory(href=self.getURL(context))[text]

### The End ###
