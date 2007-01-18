""" LibCIA.XML

Classes and utility functions to make the DOM suck less. CIA has been
ported across DOM implementations multiple times, and may need to be
ported again in the future. This file, in addition to making life easier,
should hide quirks of particular DOM implementations as best as possible.

This implementation uses Minidom.

"""
#
# CIA open source notification system
# Copyright (C) 2003-2007 Micah Dowty <micah@navi.cx>
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

import re
from xml.dom import minidom
from xml.sax import SAXParseException as ParseException


class XMLObject(object):
    """An object based on an XML document tree. This class provides
       methods to load it from a string or a DOM tree, and convert
       it back to an XML string.

       'doc' is either a DOM node, a string containing
       the message in XML, or a stream-like object.
       """
    # Subclasses can set this to enable some caches
    immutable = False
    _xpcache = None
    
    def __init__(self, doc=None, uri=None):
        if type(doc) in (str, unicode):
            self.loadFromString(doc, uri)
        elif hasattr(doc, 'read'):
            self.loadFromStream(doc, uri)
        elif hasattr(doc, 'nodeType'):
            self.loadFromDom(doc)

    def __str__(self):
        return toString(self.xml)

    def loadFromString(self, string, uri=None):
        """Parse the given string as XML and set the contents of the message"""
        self.loadFromDom(parseString(string))

    def loadFromStream(self, stream, uri=None):
        """Parse the given stream as XML and set the contents of the message"""
        self.loadFromDom(parseStream(stream))

    def loadFromDom(self, root):
        """Set the contents of the Message from a parsed DOM tree"""
        if hasattr(root, "documentElement"):
            self.xml = root
        else:
            # Encase the given tree fragment in a Document
            self.xml = createRootNode()
            self.xml.appendChild(self.xml.importNode(root, True))
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
            rootElement = None
            if self.xml.nodeType == self.xml.DOCUMENT_NODE:
                rootElement = self.xml.documentElement
            elif self.xml.nodeType == self.xml.ELEMENT_NODE:
                rootElement = self.xml

            if (not rootElement) or rootElement.nodeName != self.requiredRootElement:
                raise UnknownElementError("Missing a required %r root element" %
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
        raise UnknownElementError("Invalid element in %s: '%s'" % (self.__class__.__name__, element.nodeName))


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

class UnknownElementError(XMLValidityError):
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
    for child in node.childNodes:
        if child.nodeType == child.TEXT_NODE:
            yield child.data


def shallowText(node):
    """Concatenate all text immediately within the given node"""
    return "".join(shallowTextGenerator(node))


def dig(node, *subElements):
    """Search for the given named subelements inside a node. Returns
       None if any subelement isn't found.
       """
    if not node:
        return None
    for name in subElements:
        nextNode = None
        for child in node.childNodes:
            if child.nodeType == child.ELEMENT_NODE and child.nodeName == name:
                nextNode = child
                break
        if nextNode:
            node = nextNode
        else:
            return None
    return node


def digValue(node, _type, *subElements):
    """Search for a subelement using 'dig', then retrieve all contained
       text and convert it to the given type. Strips extra whitespace.
       """
    foundNode = dig(node, *subElements)
    if foundNode:
        return _type(shallowText(foundNode).strip())


def bury(node, *subElements):
    """Search for the given named subelements inside a node,
       creating any that can't be found.
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
            node = addElement(node, name)
    return node


def buryValue(node, value, *subElements):
    """Search for a subelement using 'bury' then store the given value,
       overwriting the previous content of that node.
       """
    node = bury(node, *subElements)

    for child in list(node.childNodes):
        if child.nodeType == child.TEXT_NODE:
            node.removeChild(child)

    node.appendChild(node.ownerDocument.createTextNode(str(value)))


def addElement(node, name, content=None, attributes={}):
    """Add a new child element to the given node, optionally containing
       the given text and attributes. The attributes are specified as a
       simple mapping from name string to value string. This function
       does not support namespaces.
       """
    if node.nodeType == node.DOCUMENT_NODE:
        doc = node
    else:
        doc = node.ownerDocument

    newElement = doc.createElementNS(None, name)
    if content:
        newElement.appendChild(doc.createTextNode(content))
    for attrName, attrValue in attributes.iteritems():
        newElement.setAttributeNS(None, attrName, attrValue)
    node.appendChild(newElement)
    return newElement


parseStream = minidom.parse

def parseString(string):
    # Grarr.. minidom can't directly parse Unicode objects
    if type(string) is unicode:
        string = string.encode('utf-8')

    return minidom.parseString(string)


def createRootNode():
    return minidom.getDOMImplementation().createDocument(None, None, None)

def toString(doc):
    """Convert a DOM tree back to a string"""
    return doc.toxml()


def getChildElements(doc):
    """A generator that returns all child elements of a node"""
    for child in doc.childNodes:
        if child.nodeType == child.ELEMENT_NODE:
            yield child


def firstChildElement(doc):
    try:
        return getChildElements(doc).next()
    except StopIteration:
        return None


def hasChildElements(doc):
    # Force a boolean result
    return firstChildElement(doc) is not None


class XPathBase:
    """Abstract base XPath implementation"""

    def queryObject(self, obj):
        """Query an XMLObject, using its cache if possible"""        
        if obj.immutable:
            idc = id(self.compiled)
            if not obj._xpcache:
                obj._xpcache = {}
            if idc in obj._xpcache:
                return obj._xpcache[idc]
            else:
                r = self.queryForNodes(obj.xml)
                obj._xpcache[idc] = r
                return r
        return self.queryForNodes(obj.xml)


class XPathFull(XPathBase):
    """Full XPath implementation, using 4XPath.  Caches all xpaths by
       default, in a global dictionary. XPaths are never evicted from
       the cache.
       """
    cache = {}
    enableCache = True

    def __init__(self, path):
        if self.enableCache:
            try:
                self.compiled = self.cache[path]
            except KeyError:
                self.compiled = self._compile(path)
                self.cache[path] = self.compiled
        else:
            self.compiled = self._compile(path)

    def _compile(self, path):
        # xpath.Compile() seems to have broken error handling-
        # it's reporting syntax errors as RuntimeException.INTERNAL.
        # Work around this by instantiating the parser directly.

        import xml.xpath

        try:
            return xml.xpath.parser.new().parse(path)
        except xml.xpath.yappsrt.SyntaxError, e:
            raise XMLValidityError('XPath syntax error in "%s" at char %d: %s' % (
                path, e.pos, e.msg))

    def queryForNodes(self, doc):
        """Query an XML DOM, returning the result set"""
        return self.compiled.evaluate(xml.xpath.CreateContext(doc))


class XPathTiny(XPathBase):
    """Implements a very tiny XPath subset, which only supports absolute
       paths using only the default (child::) axis.
       """
    _validation_re = re.compile(r"^(/[a-zA-Z_][a-zA-Z0-9_\.-]*)+$")

    def __init__(self, path):
        if not self._validation_re.match(path):
            raise XMLValidityError('"%s" is not valid in the supported subset of XPath' % path)
        self.compiled = path.split("/")[1:]

    def queryForNodes(self, doc):
        ELEMENT = doc.ELEMENT_NODE
        current = [doc]
        for name in self.compiled: 
            next = []
            for el in current:
                for child in el.childNodes:
                    if child.nodeType == ELEMENT and child.nodeName == name:
                        next.append(child)
            current = next
        return current


# Our big switch to choose the default XPath implementation
XPath = XPathTiny

### The End ###
