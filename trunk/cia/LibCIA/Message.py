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

       'xml' is either a twisted.xish.domish.Element, a string containing
       the message in XML, or any object with a read() method.
       """
    def __init__(self, xml):
        if isinstance(xml, domish.Element):
            self.loadFromElement(xml)
        elif hasattr(xml, 'read'):
            self.loadFromString(xml.read())
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

       For the most part, a message is very free-form and its structure
       isn't validated. A few exceptions...

       A message must have a <message> tag at its root:

         >>> msg = Message('<message/>')
         >>> msg = Message('<monkey/>')
         Traceback (most recent call last):
         ...
         XMLValidityError: A Message's root node must be named 'message'

       If a message doesn't include a timestamp, one will be added with
       the current time:

         >>> msg = Message('<message/>')
         >>> t = int(str(msg.xml.timestamp))
         >>> time.time() - t < 2
         True

       """
    def preprocess(self):
        if self.xml.name != "message":
            raise XMLValidityError("A Message's root node must be named 'message'")

        # Stamp it with the current time if it has no timestamp yet
        if not self.xml.timestamp:
            self.xml.addElement("timestamp", content="%d" % time.time())


class Filter(XMLObject):
    """A filter is a description of some subset of all valid Message objects,
       described using a simple XML-based format and a subset of XPath. The
       filter document type is described with examples and a schema in the
       'xml' directory of this project.

       To test the filter against a message, call it with the message instance.
       A boolean value will be returned.

       Some examples, matching against the sample commit message...

         >>> msg = Message(open('../xml/sample_message.xml'))

       The <match> tag returns true if the entire text content of any tag
       matched by the given XPath matches the text in the tag.

         >>> f = Filter('<match path="/message/source/project">navi-misc</match>')
         >>> f(msg)
         True
         >>> Filter('<match path="/message/source/project">jetstream</match>')(msg)
         False

       It's important to note that the <match> tag can match any of the XPath
       matches independently. Here, the XPath will return multiple <file> tags,
       one of which satisfies the <match> tag:

         >>> f = Filter('<match path="/message/body/commit/files/file">' +
         ...            '    trunk/cia/LibCIA/Message.py' +
         ...            '</match>')
         >>> f(msg)
         True

       By the same rule, if the XPath never matches, the <match> tag never
       gets a chance to either:

         >>> Filter('<match path="/path/that/doesnt/exist"/>')(msg)
         False

       The match by default is case sensitive. The caseSensitive attribute can
       be set to zero to disable this:

         >>> f = Filter('<match path="/message/source/project">' +
         ...            '    NAVI-MISC' +
         ...            '</match>')
         >>> f(msg)
         False
         >>> f = Filter('<match path="/message/source/project" caseSensitive="0">' +
         ...            '    NAVI-MISC' +
         ...            '</match>')
         >>> f(msg)
         True

       The <find> tag is just like <match> but the given text only has to occur
       in an XPath match rather than exactly matching it:

         >>> Filter('<find path="/message/source/project">navi</find>')(msg)
         True
         >>> f = Filter('<find path="/message/source/project" caseSensitive="0">' +
         ...            '    NAVI-MISC' +
         ...            '</find>')
         >>> f(msg)
         True
         >>> Filter('<find path="/message/source/project">navi-miscski</find>')(msg)
         False

       For completeness, there are tags that always evaluate to a constant value:

         >>> Filter('<true/>')(msg)
         True
         >>> Filter('<false/>')(msg)
         False

       Tags can be combined using boolean algebra:

         >>> Filter('<and><true/><false/><true/></and>')(msg)
         False
         >>> Filter('<and><true/><true/><true/></and>')(msg)
         True
         >>> Filter('<or><false/><false/><false/></or>')(msg)
         False
         >>> Filter('<or><false/><false/><true/></or>')(msg)
         True

       The <not> tag, in its simplest use, negates a single argument:

         >>> Filter('<not><true/></not>')(msg)
         False
         >>> Filter('<not><false/></not>')(msg)
         True

       Of course, it would be silly for a tag to only work with one child.
       It would be intuitive for <not> to also be useful for listing several items,
       any of which can make the entire expression false. The <not> tag therefore
       actually implements a logical NOR function:

         >>> Filter('<not><false/><false/><false/></not>')(msg)
         True
         >>> Filter('<not><false/><true/><false/></not>')(msg)
         False

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

    def pathMatchTag(self, element, function):
        """Implements the logic common to all tags that test the text matched by
           an XPath against the text inside our element. The given function is used
           to determine if the text matches. This implements the properties common to
           several elements:

             - The caseSensitive attribute defaults to 1, but can be set to zero
               to force both strings to lowercase.

             - Each XPath match is tested separately, with 'or' semantics:
               if any of the XPath matches cause the provided function to match,
               this returns True

             - If there are no XPath matches, returns False
           """
        # This works through the beautiful scary evil magic of lexical scoping.
        # When we return a reference to f, it includes a dict with this function's
        # scope. This conveniently gives us a way to attach the parsed xpath and the
        # text to match.
        xp = XPathQuery(element['path'])

        # Are we doing a case sensitive match? Default is yes.
        try:
            caseSensitive = int(element['caseSensitive'])
        except KeyError:
            caseSensitive = 1

        if caseSensitive:
            text = str(element).strip()
        else:
            text = str(element).strip().lower()

        def filterMatch(msg):
            # Any of the XPath matches can make our match true
            matchStrings = xp.queryForStringList(msg.xml)
            if matchStrings:
                for matchString in matchStrings:
                    if caseSensitive:
                        matchString = matchString.strip()
                    else:
                        matchString = matchString.strip().lower()
                    if function(matchString, text):
                        return True
            return False
        return filterMatch

    def element_match(self, element):
        """Evaluates to True if the text matched by our 'path' attribute matches
           this element's content, not including leading and trailing whitespace.
           """
        return self.pathMatchTag(element, lambda matchString, text: matchString == text)

    def element_find(self, element):
        """Evaluates to True if the text in this tag is contained within any of the
           XPath match strings.
           """
        return self.pathMatchTag(element, lambda matchString, text: matchString.find(text) >= 0)

    def element_and(self, element):
        """Evaluates to True if and only if all child functions evaluate to True"""
        childFunctions = [self.parseFilter(child) for child in element.elements()]
        def filterAnd(msg):
            for child in childFunctions:
                if not child(msg):
                    return False
            return True
        return filterAnd

    def element_or(self, element):
        """Evaluates to True if and only if any child function evaluates to True"""
        childFunctions = [self.parseFilter(child) for child in element.elements()]
        def filterOr(msg):
            for child in childFunctions:
                if child(msg):
                    return True
            return False
        return filterOr

    def element_not(self, element):
        """The NOR function, returns false if and only if any child function evaluates to True.
           For the reasoning behind calling this 'not', see the doc string for this class.
           """
        childFunctions = [self.parseFilter(child) for child in element.elements()]
        def filterNot(msg):
            for child in childFunctions:
                if child(msg):
                    return False
            return True
        return filterNot

    def element_true(self, element):
        """Always evaluates to True"""
        def alwaysTrue(msg):
            return True
        return alwaysTrue

    def element_false(self, element):
        """Always evaluates to False"""
        def alwaysFalse(msg):
            return False
        return alwaysFalse


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

    def gotText(self, data):
        # This is (another) hack that seems to be necessary
        # to properly parse text included directly into the root node.
        if self.currElem:
            domish.SuxElementStream.gotText(self, data)
        else:
            self.root.addContent(data)

    def elem(self, elem):
        self.root.addChild(elem)

    def docEnd(self):
        self.done = 1


def _test():
    import doctest, Message
    return doctest.testmod(Message)

if __name__ == "__main__":
    _test()

### The End ###
