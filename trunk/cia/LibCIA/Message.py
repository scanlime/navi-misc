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

from twisted.web import xmlrpc
from twisted.xish import domish
from twisted.xish.xpath import XPathQuery
from twisted.python import log
import time
import sys, traceback
import XML, Interface


class HubInterface(xmlrpc.XMLRPC):
    """A simple interface for delivering XML messages to the hub over XML-RPC
       """
    def __init__(self, caps, hub):
        self.caps = caps
        self.hub = hub

    def deliverXml(self, xml):
        """Internal function to deliver a message in raw XML"""
        return

    def xmlrpc_deliver(self, xml, key=None):
        """Deliver an XML message, returning its result on success or a Fault on failure.
           This does not yet require a capability key, but it might eventually.
           """
        try:
            result = self.hub.deliver(Message(xml))
            if result is None:
                return True
            return result
        except:
            Interface.catchFault("Exception occurred while delivering the message:\n%s\n--------" % xml)


class Message(XML.XMLObject):
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
            raise XML.XMLValidityError("A Message's root node must be named 'message'")

        # Stamp it with the current time if it has no timestamp yet
        if not self.xml.timestamp:
            self.xml.addElement("timestamp", content="%d" % time.time())


class Hub(object):
    """A central location where messages are delivered, filtered, and dispatched
       to interested parties.
       """
    def __init__(self):
        # Maps callables to filters
        self.clients = {}
        self.updateClients()

    def addClient(self, callable, filter=None):
        """Add a callable object to the list of those notified when new messages
           arrive. If filter is not None, the callable is only called if the filter
           evaluates to True.
           If the callable returns non-None, the returned value will be returned
           by deliver(). If multiple client callables return non-None, the last
           seen value is used.
           """
        self.clients[callable] = filter
        self.updateClients()

    def delClient(self, callable):
        """Delate a callable object from the list of those notified when new messages arrive"""
        del self.clients[callable]
        self.updateClients()

    def updateClients(self):
        """Update a cached items() list for our clients dict, to speed up deliver().
           Must be called whenever clients changes.
           """
        self.clientItems = self.clients.items()

    def deliver(self, message):
        """Given a Message instance, determine who's interested
           in its contents and delivers it to them.
           """
        result = None
        for callable, filter in self.clientItems:
            if filter and not filter(message):
                continue
            itemResult = callable(message)
            if itemResult is not None:
                result = itemResult
        return result


class Filter(XML.XMLFunction):
    """A filter is a description of some subset of all valid Message objects,
       described using a simple XML-based format and a subset of XPath. The
       filter document type is described with examples and a schema in the
       'xml' directory of this project.

       To test the filter against a message, call it with the message instance.
       A boolean value will be returned.

       Some examples, matching against the sample commit message...

         >>> msg = Message(open('../xml/sample_message.xml'))

       The <match> tag returns true if the entire text content of any tag
       matched by the given XPath matches the text in the <match> tag:

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

       The match by default is case insensitive. The caseSensitive attribute can
       be set to one to change this:

         >>> f = Filter('<match path="/message/source/project">' +
         ...            '    NAVI-MISC' +
         ...            '</match>')
         >>> f(msg)
         True
         >>> f = Filter('<match path="/message/source/project" caseSensitive="1">' +
         ...            '    NAVI-MISC' +
         ...            '</match>')
         >>> f(msg)
         False

       The <find> tag is just like <match> but the given text only has to occur
       in an XPath match rather than exactly matching it:

         >>> Filter('<find path="/message/source/project">navi</find>')(msg)
         True
         >>> Filter('<find path="/message/source/project">' +
         ...        '    NAVI-MISC' +
         ...        '</find>')(msg)
         True
         >>> Filter('<find path="/message/source/project">navi-miscski</find>')(msg)
         False
         >>> Filter('<find path="/message/source">trunk</find>')(msg)
         True

       The <find> tag with an empty search string can be used to test for the
       existence of an XPath match:

         >>> Filter('<find path="/message/body/commit"/>')(msg)
         True
         >>> Filter('<find path="/message/body/snail"/>')(msg)
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

       As if we weren't already having too much fun, several of the Python bitwise
       operators can be used like logical operators to combine Filter instances
       after they're parsed but before their value has been determined:

         >>> f = Filter('<false/>') | Filter('<false/>')
         >>> f(msg)
         False
         >>> f = Filter('<true/>') | Filter('<false/>')
         >>> f(msg)
         True

         >>> f = Filter('<false/>') & Filter('<true/>')
         >>> f(msg)
         False
         >>> f = Filter('<true/>') & Filter('<true/>')
         >>> f(msg)
         True

         >>> f = ~Filter('<true/>')
         >>> f(msg)
         False
         >>> f = ~Filter('<false/>')
         >>> f(msg)
         True

       """
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
            caseSensitive = 0

        text = str(element).strip()
        if not caseSensitive:
            text = text.lower()

        def filterMatch(msg):
            # Use queryForNodes then str() so that matched
            # nodes without any text still give us at least
            # the empty string. This is important so that <find>
            # with an empty search string can be used to test
            # for the existence of an XPath match.
            nodes = xp.queryForNodes(msg.xml)
            if nodes:
                matchStrings = map(XML.allText, nodes)

                # Any of the XPath matches can make our match true
                for matchString in matchStrings:
                    matchString = matchString.strip()
                    if not caseSensitive:
                        matchString = matchString.lower()
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
        childFunctions = [self.parse(child) for child in element.elements()]
        def filterAnd(msg):
            for child in childFunctions:
                if not child(msg):
                    return False
            return True
        return filterAnd

    def element_or(self, element):
        """Evaluates to True if and only if any child function evaluates to True"""
        childFunctions = [self.parse(child) for child in element.elements()]
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
        childFunctions = [self.parse(child) for child in element.elements()]
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

    def __and__(self, other):
        """Perform a logical 'and' on two Filters without evaluating them"""
        newFilter = Filter()
        newFilter.f = lambda msg: self(msg) and other(msg)
        return newFilter

    def __or__(self, other):
        """Perform a logical 'or' on two Filters without evaluating them"""
        newFilter = Filter()
        newFilter.f = lambda msg: self(msg) or other(msg)
        return newFilter

    def __invert__(self):
        """Perform a logical 'not' on this Filter without evaluating it"""
        newFilter = Filter()
        newFilter.f = lambda msg: not self(msg)
        return newFilter


class Formatter(object):
    """An abstract object capable of creating and/or modifying alternate
       representations of a Message. This could include converting it to HTML,
       converting it to plaintext, or annotating the result of another Formatter
       with additional information.
       """
    # If non-none, this is a filter function that can be called against
    # a message to detect whether this formatter is applicable.
    detector = None

    # A string identifying this formatter's output medium. Could be 'html',
    # 'irc', etc.
    medium = None

    def format(self, message, input=None):
        """Given a message and optionally the result of a previous Formatter,
           return a formatted representation of the message.
           """
        pass


class AutoFormatter(Formatter):
    r"""A meta-formatter that, based on the provided output medium, automatically
        finds an applicable formatter and runs it.

        The AutoFormatter is constructed with the target medium you're interested in.
        All formatters with that target medium are loaded. When a message must be
        formatted, the first one with a matching detector filter is chosen and
        invoked.

          >>> f = AutoFormatter('irc')
          >>> msg = Message('<message><body><colorText><b>Hello</b>World</colorText></body></message>')
          >>> f.format(msg)
          '\x02Hello\x0fWorld'
        """
    def __init__(self, medium):
        # Load all formatters with the given medium and a non-None detector
        import Formatters
        self.formatters = []
        for cls in Formatters.__dict__.itervalues():
            if type(cls) is type and hasattr(cls, 'format'):
                if cls.medium == medium and cls.detector is not None:
                    # Load the formatter's detector function
                    # and make a list of (Formatter, Filter) instance
                    # tuples
                    filter = Filter(cls.detector)
                    self.formatters.append((cls(), filter))

    def format(self, message, input=None):
        """Find and invoke a formatter applicable to this message"""
        for formatter, filter in self.formatters:
            if filter(message):
                return formatter.format(message, input)


class NamedFormatter(Formatter):
    """A meta-formatter that loads a named Formatter class from the Formatters
       module and invokes it.
       """
    def __init__(self, name):
        import Formatters
        try:
            cls = Formatters.__dict__[name]
        except KeyError:
            raise KeyError("No such formatter %r" % name)
        if not hasattr(cls, 'format'):
            raise KeyError("%r is not a formatter" % name)
        self.formatter = cls()

    def format(self, message, input=None):
        return self.formatter.format(message, input)


def _test():
    import doctest, Message
    return doctest.testmod(Message)

if __name__ == "__main__":
    _test()

### The End ###
