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
from Nouvelle import tag, place, Serializer
from LibCIA import TimeUtil
import time


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
        self.setHeader('content-type', "text/html")
        self.setResponseCode(http.INTERNAL_SERVER_ERROR)

        # Now we can use Nouvelle to generate the error page :)
        self.write(Serializer().render(
            tag('html')[
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
                            tag('td')[ TimeUtil.formatDateRFC822(time.time()) ],
                        ],
                        tag('tr')[
                            tag('td')[ tag('b')[ 'Requested path:' ]],
                            tag('td')[ self.uri ],
                        ],
                        tag('tr')[
                            tag('td')[ tag('b')[ 'Exception type:' ]],
                            tag('td')[ str(reason.value.__class__) ],
                        ],
                        tag('tr')[
                            tag('td')[ tag('b')[ 'Exception value:' ]],
                            tag('td')[ str(reason.value) ],
                        ],
                    ],

                    # Traceback
                    tag('p')[
                        tag('b')[ 'Traceback:' ],
                    ],
                    tag('p')[
                        tag('pre')[ reason.getTraceback() ],
                    ],
                ],
            ]))
        self.finish()
        return reason


class Site(server.Site):
    """A twisted.web.server.Site subclass, to use our modified Request class"""
    requestFactory = Request


### The End ###
