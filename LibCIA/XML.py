""" LibCIA.XML

Classes and utility functions to make the DOM suck less
"""
#
# CIA open source notification system
# Copyright (C) 2003-2004 Micah Dowty <micah@navi.cx>
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

import types, weakref
import Nouvelle
from Ft.Xml import Domlette
from cStringIO import StringIO

# 4Suite requires a URI for everything, but it doesn't make sense for most of our XML snippets
defaultURI = "cia://anonymous-xml"


class XMLObject(object):
    """An object based on an XML document tree. This class provides
       methods to load it from a string or a DOM tree, and convert
       it back to an XML string.

       'xml' is either a DOM node, a string containing
       the message in XML, or a stream-like object.
       """
    def __init__(self, xml=None, uri=None):
        if type(xml) in types.StringTypes:
            self.loadFromString(xml, uri)
        elif hasattr(xml, 'read'):
            self.loadFromStream(xml, uri)
        elif hasattr(xml, 'nodeType'):
            self.loadFromDom(xml)

    def __str__(self):
        return toString(self.xml)

    def loadFromString(self, string, uri=None):
        """Parse the given string as XML and set the contents of the message"""
        self.loadFromDom(Domlette.NonvalidatingReader.parseString(string, uri or defaultURI))

    def loadFromStream(self, stream, uri=None):
        """Parse the given stream as XML and set the contents of the message"""
        self.loadFromDom(Domlette.NonvalidatingReader.parseStream(string, uri or defaultURI))

    def loadFromDom(self, root):
        """Set the contents of the Message from a parsed DOM tree"""
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
        """Upon creating this object, parse the XML tree recursively.
           The result returned from parsing the tree's root element
           is set to our resultAttribute.
           """
        # Validate the root element type if the subclass wants us to.
        # This is hard to do elsewhere, since the element handlers don't
        # know where they are in the XML document.
        if self.requiredRootElement is not None:
            if not dig(self.xml, self.requiredRootElement):
                raise XMLValidityError("Missing a required %r root element" %
                                       self.requiredRootElement)

        setattr(self, self.resultAttribute, self.parse(self.xml))

    def parse(self, node, *args, **kwargs):
        """Given a DOM node, finds an appropriate parse function and invokes it"""
        if node.nodeType == node.TEXT_NODE:
            return self.parseString(node.data, *args, **kwargs)

        elif node.nodeType == node.ELEMENT_NODE:
            f = getattr(self, "element_" + node.nodeName, None)
            if f:
                return f(node, *args, **kwargs)
            else:
                return self.unknownElement(node, *args, **kwargs)

        elif node.nodeType == node.DOCUMENT_NODE:
            return self.parse(node.documentElement, *args, **kwargs)

    def childParser(self, node, *args, **kwargs):
        """A generator that parses all relevant child nodes, yielding their return values"""
        parseableTypes = (node.TEXT_NODE, node.ELEMENT_NODE)
        for child in node.childNodes:
            if child.nodeType in parseableTypes:
                yield self.parse(child, *args, **kwargs)

    def parseString(self, s):
        """The analogue to element_* for character data"""
        pass

    def unknownElement(self, element):
        """An unknown element was found, by default just generates an exception"""
        raise XMLValidityError("Unknown element name in %s: %r" % (self.__class__.__name__, element.nodeName))


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


def allTextGenerator(node):
    """A generator that, given a DOM tree, yields all text fragments in that tree"""
    if node.nodeType == node.TEXT_NODE:
        yield node.data
    for child in node.childNodes:
        for text in allTextGenerator(child):
            yield text


def allText(node):
    """Concatenate all text under the given element recursively, and return it"""
    return "".join(allTextGenerator(node))


def shallowTextGenerator(node):
    """A generator that, given a DOM tree, yields all text fragments contained immediately within"""
    if node.nodeType == node.TEXT_NODE:
        yield node.data


def shallowText(node):
    """Concatenate all text immediately within the given node"""
    return "".join(shallowTextGenerator(node))


def dig(node, *subElements):
    """Search for the given named subelements inside a node. Returns
       None if any subelement isn't found.
       """
    for name in subElements:
        nextNode = None
        for child in node.childNodes:
            if child.nodeType == child.ELEMENT_NODE and child.nodeName == name:
                nextNode = child
                break
        if nextNode:
            node = nextNode
        else:
            return
    return node


def addElement(node, name, content=None, attributes={}):
    """Add a new child element to the given node, optionally containing
       the given text and attributes. The attributes are specified as a
       simple mapping from name string to value string. This function
       does not support namespaces.
       """
    newElement = node.ownerDocument.createElementNS(None, name)
    if content:
        newElement.appendChild(node.ownerDocument.createTextNode(content))
    for attrName, attrValue in attributes.iteritems():
        newElement.setAttributeNS(None, attrName, attrValue)
    node.appendChild(newElement)


def parseString(s, uri=defaultURI):
    """A parseString wrapper that doesn't require a URI"""
    return Domlette.NonvalidatingReader.parseString(s, uri)


def toString(xml):
    """Convert a DOM tree back to a string"""
    io = StringIO()
    Domlette.Print(xml, io)
    return io.getvalue()


def getChildElements(xml):
    """A generator that returns all child elements of a node"""
    for child in xml.childNodes:
        if child.nodeType == child.ELEMENT_NODE:
            yield child


class HTMLPrettyPrinter(XMLObjectParser):
    """An object parser that converts arbitrary XML to pretty-printed
       representations in the form of Nouvelle-serializable tag trees.
       """
    def parseString(self, s):
        s = s.strip()
        if s:
            return Nouvelle.tag('p', _class='xml-text')[ s ]
        else:
            return ()

    def unknownElement(self, element):
        # Format the element name and attributes
        elementName = Nouvelle.tag('span', _class="xml-element-name")[ element.nodeName ]
        elementContent = [ elementName ]
        for attr in element.attributes.itervalues():
            elementContent.extend([
                ' ',
                Nouvelle.tag('span', _class='xml-attribute-name')[ attr.name ],
                '="',
                Nouvelle.tag('span', _class='xml-attribute-value')[ attr.value ],
                '"',
                ])

        # Now the contents...
        if element.hasChildNodes():
            completeElement = [
                "<", elementContent, ">",
                Nouvelle.tag('blockquote', _class='xml-element-content')[
                    [self.parse(e) for e in element.childNodes],
                ],
                "</", elementName, ">",
                ]
        else:
            completeElement = ["<", elementContent, "/>"]

        return Nouvelle.tag('div', _class='xml-element')[ completeElement ]

htmlPrettyPrint = HTMLPrettyPrinter().parse


xPathCache = weakref.WeakValueDictionary()

class XPath:
    """A precompiled XPath class that caches parsed XPaths in a global weakref'ed
       dictionary. This should help CIA a bit with load time and memory usage, since
       we use many of the same XPaths in rulesets and filters.
       """
    def __init__(self, path, context=None):
        global xPathCache
        from Ft.Xml import XPath

        try:
            self.compiled = xPathCache[path]
        except KeyError:
            self.compiled = XPath.Compile(path)
            xPathCache[path] = self.compiled

        if context is None:
            context = XPath.Context.Context(None, processorNss={})
        self.context = context

    def queryForNodes(self, doc):
        from Ft.Xml import XPath
        return XPath.Evaluate(self.compiled, doc, self.context)

### The End ###
