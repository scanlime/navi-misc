""" Template

Template classes for building web pages using our particular style
"""
#
# Copyright (C) 2005 David Trowbridge
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License
# as published by the Free Software Foundation; either version 2
# of the License, or (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
#

import Nouvelle, Nouvelle.Twisted, types
from twisted.internet import defer
from Nouvelle import tag, place, xml

class Page (Nouvelle.Twisted.Page):
    mainTitle = None
    subTitle = []

    leftColumn  = []
    mainColumn  = []

    siteName = ''
    extraHeaders = []

    site_bottomOfFooter = []

    document = [
        xml ('<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.1//EN" '
            '"http://www.w3.org/TR/xhtml11/DTD/xhtml11.dtd">\n'),
        tag ('html', xmlns="http://www.w3.org/1999/xhtml")[
            tag ('head')[
                xml ('<meta http-equiv="Content-Type" content="text/html; charset=iso-8859-1" />'),
                tag ('title')[ place ("pageTitle") ],
                place ('baseTag'),
                tag ('link', rel='stylesheet', href='/default.css', type='text/css'),
                place ('extraHeaders'),
                ],
            tag ('body')[
                tag ('div', _class='heading'),
                place ('site_bottomOfFooter'),
                ],
            ],
        ]

    def render_pageTitle(self, context):
        # Wait for the title and site name to resolve into strings so we can mess with them a bit more
        result = defer.Deferred ()
        defer.gatherResults ([
            defer.maybeDeferred (self.render_mainTitle, context),
            defer.maybeDeferred (self.render_siteName, context),
        ]).addCallback (self._render_pageTitle, context, result).addErrback (result.errback)
        return result

    def _render_pageTitle(self, titleAndSite, context, result):
        # Now that the title and site name have fully resolved, we can apply some heuristics...
        title, siteName = titleAndSite

        if type (title) in types.StringTypes and type (siteName) in types.StringTypes:
            # The title and site are plain strings. If it starts with or ends with the site name,
            # just use it as-is to avoid being overly redundant.
            if title == siteName or title.startswith (siteName + ' ') or title.endswith (' ' + siteName):
                result.callback (title)
                return

        # Otherwise, stick the title and site name together
        result.callback([title, ' - ', siteName])

    def render_siteName(self, context):
        return self.siteName

    def render_mainTitle(self, context):
        return self.mainTitle

    def render_baseTag(self, context):
        """Return an HTML <base> tag pointing at this page's original URL.
           This keeps the page from breaking if it's saved to disk or copied elsewhere.
        """
        return tag('base', href = context['request'].prePathURL())
