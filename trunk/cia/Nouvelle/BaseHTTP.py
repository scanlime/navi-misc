""" Nouvelle.BaseHTTP

Glue for using Nouvelle with Python's builtin BaseHTTPServer module.
This provides a Page class that lets objects be attached as children
to it, a RequestHandler that dispatches HTTP requests to a root Page,
and a simple main function that makes it quick and easy to start a
server with a particular Page at its root.
"""
#
# Nouvelle web framework
# Copyright (C) 2003 Micah Dowty <micahjd@users.sourceforge.net>
#
#  This library is free software; you can redistribute it and/or
#  modify it under the terms of the GNU Lesser General Public
#  License as published by the Free Software Foundation; either
#  version 2.1 of the License, or (at your option) any later version.
#
#  This library is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
#  Lesser General Public License for more details.
#
#  You should have received a copy of the GNU Lesser General Public
#  License along with this library; if not, write to the Free Software
#  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
#

import Nouvelle
from Nouvelle import tag
import BaseHTTPServer, urlparse


class Page:
    """A web resource that renders a tree of tag instances from its 'document'
       attribute, and can have child resources attached to it.
       """
    serializerFactory = Nouvelle.Serializer
    responseCode = 200

    def handleRequest(self, request, args):
        """Given a RequestHandler instance, send back an HTTP response code,
           headers, and a rendition of this page.
           """
        request.send_response(self.responseCode)
        self.sendHeaders(request)

        context  = {
            'owner': self,
            'request': request,
            'args': args,
            }
        self.preRender(context)

        rendered = str(self.serializerFactory().render(self.document, context))
        request.wfile.write(rendered)

    def sendHeaders(self, request):
        """Send back HTTP headers for a given request"""
        request.send_header('Content-Type', 'text/html')
        request.end_headers()

    def preRender(self, context):
        """Called prior to rendering each request, subclasses can use this to annotate
           'context' with extra information or perform other important setup tasks.
           """
        pass

    def addChild(self, name, page):
        """Add the given Page instance as a child under this one in the URL tree"""
        if not hasattr(self, 'children'):
            self.children = {}
        self.children[name] = page

    def findChild(self, name):
        """Return the named child of this Page. By default this looks in
           self.children, and if a page isn't found returns Error404.
           """
        if not name:
            # Ignore empty path segments
            return self
        if hasattr(self, 'children') and self.children.has_key(name):
            return self.children[name]
        return Error404()


class Error404(Page):
    """A 404 error, resource not found"""
    responseCode = 404
    document = tag('html')[
                   tag('head')[
                       tag('title')[ "404 - Resource not found" ],
                   ],
                   tag('body')[
                       tag('h1')[ "404" ],
                       tag('h3')[ "Resource not found" ],
                   ],
               ]


class RequestHandler(BaseHTTPServer.BaseHTTPRequestHandler):
    def do_GET(self):
        # Parse the path we were given as a URL...
        scheme, host, path, parameters, query, fragment = urlparse.urlparse(self.path)

        # Find the page corresponding with our URL's path
        page = self.rootPage
        for segment in path.split("/"):
            page = page.findChild(segment)

        # Split the query into key-value pairs
        args = {}
        for pair in query.split("&"):
            if pair.find("=") >= 0:
                key, value = pair.split("=", 1)
                args.setdefault(key, []).append(value)
            else:
                args[pair] = []

        page.handleRequest(self, args)


def main(rootPage, port=8080):
    handler = RequestHandler
    handler.rootPage = rootPage
    BaseHTTPServer.HTTPServer(('', port), handler).serve_forever()

### The End ###
