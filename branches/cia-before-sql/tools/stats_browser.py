#!/usr/bin/env python
""" stats_browser.py

A (currently experimental) interface from HTTP to CIA's
stats:// namespace.
"""
#
# CIA open source notification system
# Copyright (C) 2003-2004 Micah Dowty <micahjd@users.sourceforge.net>
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

import sys, os; sys.path[0] = os.path.join(sys.path[0], '..')
import xmlrpclib, BaseHTTPServer, urllib, os
from LibCIA import Message, XML, Stats


class RequestHandler(BaseHTTPServer.BaseHTTPRequestHandler):
    def do_GET(self):
        self.send_response(200)
        self.send_header('Content-Type', 'text/html')
        self.end_headers()
        self.wfile.write("<html><body>\n<h1>stats:/%s</h1>\n" % self.path)

        stats = xmlrpclib.ServerProxy("http://navi.picogui.org:3910").stats

        # Catalog of subdirectories
        self.wfile.write("<ul>\n")
        children = stats.catalog(self.path)
        children.sort()
        for child in children:
            self.wfile.write("<li><a href=%r>%s</a></li>\n" % (os.path.join(self.path, child), urllib.unquote(child)))
        self.wfile.write("</ul>\n")

        # Recent messages
        self.wfile.write("<ol>\n")
        for xml in stats.getLatestMessages(self.path, 10):
            text = XML.allText(Message.Message(xml).xml.body)
            self.wfile.write("<li>%s</li>\n" % text)
        self.wfile.write("</ol>\n")

        # Event counters
        xml = XML.parseString(stats.getCounters(self.path))
        if xml:
            self.wfile.write("<ul>\n")
            for element in xml.elements():
                counter = Stats.Counter(element)
                self.wfile.write("<li>%s: %s</li>" % (counter.name, counter.getEventCount()))
            self.wfile.write("</ul>\n")

        self.wfile.write("</body></html>\n")


def main():
    BaseHTTPServer.HTTPServer(('', 8080), RequestHandler).serve_forever()

if __name__ == '__main__':
    main()

### The End ###
