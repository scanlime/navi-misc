""" filterlib

Common code shared between server-side message filters. These filters
are small python scripts called from procmail to convert arbitrary emails,
typically from a commit mailing list, into messages for CIA. All scripts
receive incoming mail on stdin and send output via XML-RPC.
"""
#
# CIA open source notification system
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

import sys, email, xmlrpclib
from twisted.xish import domish
from StringIO import StringIO


class Filter:
    """Base class for email filters. The main() function handles
       reading the input message, parsing its headers, and calling
       the subclass-defined 'parse' function to generate an XML message.
       """
    project = None
    server = "http://localhost:3910"

    def main(self):
        self.message = email.message_from_file(sys.stdin)
        self.body = StringIO(self.message.get_payload())

        # Ignore replies
        if self.message['subject'].strip().lower().startswith("re:"):
            return

        self.initMessage()
        self.parse()
        self.deliver()

    def initMessage(self):
        """Put together a skeleton XML message that the parse()
           function can fill in the details of
           """
        self.xml = domish.Element((None, 'message'))
        self.xml.addElement('body')
        self.xml.addElement('generator')
        self.xml.addElement('source')
        self.xml.source.addElement('project', content=self.project)
        self.xml.generator.addElement('name', content='CIA Email Filters')

    def parse(self):
        """Subclasses should define this function to parse the incoming
           email and fill in self.xml with its details.
           """
        pass

    def deliver(self):
        """Deliver the message in self.xml"""
        xmlrpclib.ServerProxy(self.server).hub.deliver(self.xml.toXml())


class CommitFilter(Filter):
    """A filter with extra methods for generating commit messages"""
    def initMessage(self):
        Filter.initMessage(self)
        self.xml.body.addElement('commit')

    def addAuthor(self, author):
        self.xml.body.commit.addElement('author', content=author)

    def addFile(self, file):
        if not self.xml.body.commit.files:
            self.xml.body.commit.addElement('files')
        self.xml.body.commit.files.addElement('file', content=file)

    def addLog(self, log):
        self.xml.body.commit.addElement('log', content=log)

### The End ###
