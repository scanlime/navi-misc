""" LibCIA.Message

Abstractly represents a notification message and ways it may
be formatted, filtered, and distributed.
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

from twisted.xish import domish
from twisted.xish.xpath import XPathQuery
import time


class Message(object):
    """Abstract container for a notification message. All messages
       are represented by XML DOM trees.

       A real DTD/schema is forthcoming...

       'xml' is either a twisted.xish.domish.Element or a string containing
       the message in XML.
       """
    def __init__(self, xml):
        if isinstance(xml, domish.Element):
            self.loadFromElement(xml)
        else:
            self.loadFromString(xml)

    def loadFromString(self, string):
        """Parse the given string as XML and set the contents of the message"""
        parser = DomishStringParser()
        parser.parse(string)
        self.loadFromElement(parser.root)

    def loadFromElement(self, root):
        """Set the contents of the Message from a parsed document tree given
           as a twisted.xish.domish.Element instance. This may add missing
           information to the message.
           """
        self.xml = root

        # Stamp it with the current time if it has no timestamp yet
        if not self.xml.timestamp:
            self.xml.addElement("timestamp", content="%d" % time.time())


class DomishStringParser(domish.SuxElementStream):
    """Because domish doesn't include a parseString()..."""
    def __init__(self):
        domish.SuxElementStream.__init__(self)
        self.DocumentStartEvent = self.docStart
        self.ElementEvent = self.elem
        self.DocumentEndEvent = self.docEnd
        self.done = 0

    def docStart(self, elem):
        self.root = elem

    def elem(self, elem):
        self.root.addChild(elem)

    def docEnd(self):
        self.done = 1


if __name__ == "__main__":
    msg = Message("""
       <message>
           <generator>
               <name>CIA client for Subversion</name>
               <version>0.52</version>
               <url>http://navi/foo</url>
           </generator>
           <source>
               <project>navi-misc</project>
               <module>cia</module>
               <branch>trunk</branch>
           </source>
           <body>
               <commit>
                   <revision>1132</revision>
                   <files>
                       <file>trunk/cia/LibCIA/Message.py</file>
                       <file>trunk/cia/LibCIA/Message.py</file>
                   </files>
                   <log>
                       Another commit.. the whole log message would
                       go here, and it's up to message formatters to
                       choose how much to display
                   </log>
               </commit>
           </body>
       </message>
       """)
    print msg.xml.toXml()
    xp = XPathQuery("/message/source/project")
    print xp.queryForString(msg.xml)


### The End ###
