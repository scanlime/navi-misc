""" LibCIA.Web.Server

General classes for CIA's web server, including subclasses of twisted.web's
Site and Request classes.
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

from twisted.web import server, error
from twisted.protocols import http
from twisted.python import log
from Nouvelle import tag, place, Serializer, Twisted
import Nouvelle
from LibCIA import TimeUtil
import time


class InternalErrorPage(Twisted.Page):
    def __init__(self, failure):
        self.failure = failure

    def preRender(self, context):
        request = context['request']
        request.setHeader('content-type', "text/html")
        request.setResponseCode(http.INTERNAL_SERVER_ERROR)

    def render_time(self, context):
        return TimeUtil.formatDateRFC822(time.time())

    def render_excType(self, context):
        return str(self.failure.value.__class__)

    def render_excValue(self, context):
        return str(self.failure.value)

    def render_traceback(self, context):
        return self.failure.getTraceback()

    def render_uri(self, context):
        return context['request'].uri

    document = tag('html')[
                   tag('head')[
                       tag('title')[ "Internal Server Error" ],
                   ],
                   tag('body')[
                       tag('h2')[ "Internal Server Error" ],

                       # Friendly message
                       tag('p')[
                           "Sorry, it looks like you just found a bug. If you would like to "
                           "help us identify the problem, please email a copy of this page to the "
                           "webmaster of this site along with a description of what happened. Thanks!"
                       ],

                       # Table of useful values
                       tag('table', cellpadding=5) [
                           tag('tr')[
                               tag('td')[ tag('b')[ 'Current time:' ]],
                               tag('td')[ place('time') ],
                           ],
                           tag('tr')[
                               tag('td')[ tag('b')[ 'Requested path:' ]],
                               tag('td')[ place('uri') ],
                           ],
                           tag('tr')[
                               tag('td')[ tag('b')[ 'Exception type:' ]],
                               tag('td')[ place('excType') ],
                           ],
                           tag('tr')[
                               tag('td')[ tag('b')[ 'Exception value:' ]],
                               tag('td')[ place('excValue') ],
                           ],
                       ],

                       # Traceback
                       tag('p')[
                           tag('b')[ 'Traceback:' ],
                       ],
                       tag('p')[
                           tag('pre')[ place('traceback') ],
                       ],
                   ],
               ]


class Request(server.Request):
    """A Request subclass overriding some default policies
       of twisted.web, including exception reporting
       """
    def processingFailed(self, reason):
        """A replacement for twisted.web's usual traceback page.
           We disable the traceback and only report the actual exception,
           for a few reasons:

            - Often the data structures in use are large, and repr()'ing them
              for the web page is very slow

            - Showing the values of variables on the stack may disclose capability
              keys or other values that the web users shouldn't be able to see

            - The tracebacks really aren't any more helpful than the
              ones reported to twistd.log :)
        """
        log.err(reason)
        page = InternalErrorPage(reason)
        self.write(page.render(self))
        self.finish()
        return reason

    def process(self):
        # Count this request, yay
        server.Request.process(self)
        self.site.requestCount += 1


class Site(server.Site):
    """A twisted.web.server.Site subclass, to use our modified Request class"""
    requestFactory = Request

    def __init__(self, resource):
        # Some extra widgets for tracking server uptime and hit count
        self.requestCount = 0
        self.serverStartTime = time.time()
        server.Site.__init__(self, resource)

### The End ###
