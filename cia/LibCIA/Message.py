""" LibCIA.Message

Classes to represent, distribute, and filter messages represented
by XML documents.
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


class XMLObject(object):
    """An object based on an XML document tree. This class provides
       methods to load it from a string or a DOM element tree, and convert
       it back to an XML string.

       'xml' is either a twisted.xish.domish.Element or a string containing
       the message in XML.
       """
    def __init__(self, xml):
        if isinstance(xml, domish.Element):
            self.loadFromElement(xml)
        else:
            self.loadFromString(xml)

    def __str__(self):
        return self.xml.toXml()

    def loadFromString(self, string):
        """Parse the given string as XML and set the contents of the message"""
        parser = DomishStringParser()
        parser.parse(string)
        self.loadFromElement(parser.root)

    def loadFromElement(self, root):
        """Set the contents of the Message from a parsed document tree given
           as a twisted.xish.domish.Element instance.
           """
        self.xml = root
        self.preprocess()

    def preprocess(self):
        """A hook where subclasses can add code to inspect a freshly
           loaded XML document and/or fill in any missing information.
           """
        pass


class XMLValidityError(Exception):
    """This error is raised by subclasses of XMLObject that encounter problems
       in the structure of XML documents presented to them. Normally this should
       correspond with the document not being valid according to its schema,
       but we don't actually use a validating parser.
       """
    pass


class Message(XMLObject):
    """Abstract container for a notification message. All messages
       are represented by XML DOM trees. The message document type
       is described with examples and a schema in the 'xml' directory
       of this project.
       """
    def preprocess(self):
        if self.xml.name != "message":
            raise XMLValidityError("A Message's root node must be named 'message'")

        # Stamp it with the current time if it has no timestamp yet
        if not self.xml.timestamp:
            self.xml.addElement("timestamp", content="%d" % time.time())


class Filter(XMLObject):
    """A filter is a description of some subset of all valid Message objects,
       described using a simple XML-based format and XPath. The filter document
       type is described with examples and a schema in the 'xml' directory
       of this project.

       To test the filter against a message, call it with the message instance.
       A boolean value will be returned.
       """
    def preprocess(self):
        """Upon loading an XML document, this recursively converts the XML filter
           to a self.__call__() function that, when called with a Message instance,
           returns a boolean indicating whether it passes the filter.
           """
        self.matchFunc = self.parseFilter(self.xml)

    def __call__(self, message):
        """Test the given message against the filter defined by this class,
           returning a boolean value.
           """
        return self.matchFunc(message)

    def parseFilter(self, element):
        """Given an XML element, recursively builds a python function
           implementing the filter it describes.
           """
        # Pass control on to the appropriate element_* function
        try:
            f = getattr(self, "element_" + element.name)
        except AttributeError:
            raise XMLValidityError("Unknown element name in Filter: %r" % element.name)
        return f(element)

    def element_match(self, element):
        """Evaluates to True if the text matched by our 'path' attribute matches
           this element's content, not including leading and trailing whitespace.
           """
        # This works through the beautiful scary evil magic of lexical scoping.
        # When we return a reference to f, it includes a dict with this function's
        # scope. This conveniently gives us a way to attach the parsed xpath and the
        # text to match.
        xp = XPathQuery(element['path'])
        text = str(element).strip()
        print element.children
        print repr(element)
        def filterMatch(msg):
            content = xp.queryForString(msg.xml).strip()
            print "%r, %r" % (text, content)
            return text == content
        return filterMatch

    def element_and(self, element):
        """Evaluates to True if all child functions evaluate to True"""
        childFunctions = [self.parseFilter(child) for child in element.children]
        def filterAnd(msg):
            for child in childFunctions:
                if not child(msg):
                    return False
            return True
        return filterAnd


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
<<<<<<< .mine
    print msg
    f = Filter('<and><match path="/message/source/project">navi-misc</match></and>')
    print f
    print f(msg)
    f = Filter('<match path="/message/source/project">navi-misc</match>')
    print f
    print f(msg)
=======
    print msg.xml.toXml()
    xp = XPathQuery("/message/source/project < 10")
    print xp.queryForString(msg.xml)
>>>>>>> .r1135


### The End ###
