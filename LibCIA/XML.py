""" LibCIA.XML

Utilities for dealing with objects built on top of XML trees
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

from twisted.xish import domish
import types, os, shutil


class XMLObject(object):
    """An object based on an XML document tree. This class provides
       methods to load it from a string or a DOM element tree, and convert
       it back to an XML string.

       'xml' is either a twisted.xish.domish.Element, a string containing
       the message in XML, or any object with a read() method.
       """
    def __init__(self, xml=None):
        if isinstance(xml, domish.Element):
            self.loadFromElement(xml)
        elif hasattr(xml, 'read'):
            self.loadFromString(xml.read())
        elif xml is not None:
            self.loadFromString(xml)

    def __str__(self):
        return self.xml.toXml()

    def loadFromString(self, string):
        """Parse the given string as XML and set the contents of the message"""
        self.loadFromElement(parseString(string))

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


class XMLObjectParser(XMLObject):
    """An XMLObject that is parsed recursively on creation into any
       python object, stored in 'resultAttribute'. parse() dispatches
       control to an element_* method when it finds an element, and
       to parseString when it comes to character data.
       """
    requiredRootElement = None
    resultAttribute = 'result'

    def preprocess(self):
        """Upon creating this object, parse the XML tree recursively
           into a function that will be invoked by __call__.
           """
        # Validate the root element type if the subclass wants us to.
        # This is hard to do elsewhere, since the element handlers don't
        # know where they are in the XML document.
        if self.requiredRootElement is not None and self.xml.name != self.requiredRootElement:
            raise XMLValidityError("Found a %r element where a root element of %r is required" %
                                   (self.xml.name, self.requiredRootElement))

        setattr(self, self.resultAttribute, self.parse(self.xml))

    def parse(self, element):
        """Given an XML element, recursively builds a python function
           implementing the functionality it describes.
           """
        # Pass control on to the appropriate element_* function
        try:
            if type(element) in types.StringTypes:
                f = self.parseString
            else:
                f = getattr(self, "element_" + element.name)
        except AttributeError:
            return self.unknownElement(element)
        return f(element)

    def parseString(self, s):
        """The analogue to element_* for character data"""
        pass

    def unknownElement(self, element):
        """An unknown element was found, by default just generates an exception"""
        raise XMLValidityError("Unknown element name in %s: %r" % (self.__class__.__name__, element.name))


class XMLFunction(XMLObjectParser):
    """An XMLObject that is parsed on creation into a function,
       making this class callable. The parser relies on member functions
       starting with 'element_' to recursively parse each element of the XML
       tree, returning a function implementing it.
       """
    resultAttribute = 'f'

    def __call__(self, *args, **kwargs):
        return self.f(*args, **kwargs)


class XMLValidityError(Exception):
    """This error is raised by subclasses of XMLObject that encounter problems
       in the structure of XML documents presented to them. Normally this should
       correspond with the document not being valid according to its schema,
       but we don't actually use a validating parser.
       """
    pass


class DomishStringParser(domish.SuxElementStream):
    """Because domish doesn't include a parseString()..."""
    def __init__(self):
        domish.SuxElementStream.__init__(self)
        self.DocumentStartEvent = self.docStart
        self.ElementEvent = self.elem
        self.DocumentEndEvent = self.docEnd
        self.done = 0
        self.root = None

    def docStart(self, elem):
        self.root = elem

    def gotText(self, data):
        # This is (another) hack that seems to be necessary
        # to properly parse text included directly into the root node.
        if self.currElem:
            domish.SuxElementStream.gotText(self, data)
        elif self.root:
            self.root.addContent(data)

    def elem(self, elem):
        if self.root:
            self.root.addChild(elem)
        else:
            self.root = elem

    def docEnd(self):
        self.done = 1


def parseString(string):
    """Parse the given string as XML, return a domish.Element"""
    parser = DomishStringParser()
    parser.parse(string)
    return parser.root


def prettyPrint(xml):
    """Given a domish.XML object, return a nice-looking string
       representation.
       """
    # This is gross, but it works...
    from xml.dom import minidom
    s = minidom.parseString(xml.toXml()).toprettyxml()

    # Filter out blank lines
    return "\n".join([line for line in s.split("\n") if line.strip()])


def allText(xml):
    """Concatenate all text under the given domish.Element and return it.
       For a document like:

          <a>boing<b> </b>ha</a>

       The str() operator will return only 'boing' but this function
       will return 'boing ha'.
       """
    if type(xml) in types.StringTypes:
        return xml
    else:
        return "".join([allText(child) for child in xml.children])

### The End ###
