""" LibCIA.Message

Classes to represent, distribute, and filter messages represented
by XML documents.
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

from twisted.python import log
import time, types
import XML, RpcServer


class HubInterface(RpcServer.Interface):
    """A simple interface for delivering XML messages to the hub over XML-RPC
       """
    def __init__(self, hub):
        RpcServer.Interface.__init__(self)
        self.hub = hub

    def xmlrpc_deliver(self, xml):
        """Deliver an XML message, returning its result on success or a Fault on failure"""
        return self.hub.deliver(Message(xml))


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
         >>> t = XML.digValue(msg.xml, int, "message", "timestamp")
         >>> time.time() - t < 2
         True

       """
    def preprocess(self):
        message = XML.dig(self.xml, "message")
        if not message:
            raise XML.XMLValidityError("A Message's root node must be named 'message'")

        # Stamp it with the current time if it has no timestamp yet
        if not XML.dig(message, "timestamp"):
            XML.addElement(message, "timestamp", "%d" % time.time())


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
    def pathMatchTag(self, element, function, textExtractor=XML.shallowText):
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
        xp = XML.XPath(element.getAttributeNS(None, 'path'))

        # Are we doing a case sensitive match? Default is no.
        caseSensitive = element.getAttributeNS(None, 'caseSensitive')
        if caseSensitive:
            caseSensitive = int(caseSensitive)
        else:
            caseSensitive = 0

        text = XML.shallowText(element).strip()
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
                matchStrings = map(textExtractor, nodes)

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
        return self.pathMatchTag(element, lambda matchString, text: matchString.find(text) >= 0,
                                 textExtractor = XML.allText)

    def element_and(self, element):
        """Evaluates to True if and only if all child functions evaluate to True"""
        childFunctions = list(self.childParser(element))
        def filterAnd(msg):
            for child in childFunctions:
                if not child(msg):
                    return False
            return True
        return filterAnd

    def element_or(self, element):
        """Evaluates to True if and only if any child function evaluates to True"""
        childFunctions = list(self.childParser(element))
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
        childFunctions = list(self.childParser(element))
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


class FormatterArgs:
    """Contains all arguments passed between formatters. This includes
       the input string, preferences dictionary, and the message being processed.
       """
    def __init__(self, message, input=None, preferences={}):
        self.message = message
        self.input = input
        self.preferences = dict(preferences)

    def copy(self, **kwargs):
        """Make a copy of the arguments, optionally overriding attributes at the same time"""
        new = FormatterArgs(self.message, self.input, self.preferences)
        new.__dict__.update(kwargs)
        return new

    def getPreference(self, name, default):
        """If a preference with the given name has been set, converts it to the same type
           as the supplied default and returns it. Otherwise, returns the default.
           """
        if name in self.preferences:
            return type(default)(self.preferences[name])
        else:
            return default


class Formatter:
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

    def formatMessage(self, message):
        """A convenience function to format a message without any prior input
           or other arguments.
           """
        return self.format(FormatterArgs(message))

    def format(self, args):
        """Given a FormatterArgs instance, return a formatted representation of the message."""
        pass

    def loadParametersFrom(self, xml):
        """This is given a <formatter> element possibly containing
           extra parameters for the formatter to process and store.
           Any problems should be signalled with an XML.XMLValidityError.

           By default, this tries to find a param_* handler for each
           element it comes across.
           """
        for tag in XML.getChildElements(xml):
            f = getattr(self, 'param_'+tag.name, None)
            if f:
                f(tag)


class CompositeFormatter(Formatter):
    """A composite formatter builds a new formatter from other formatters
       and simple built-in primitives. A composite formatter is defined
       only by its XML parameters, which may consist of any of the elements
       described below. The results from these elements are concatenated.

        <formatter *>         : Chooses and applies another formatter, according
                                to the rules in FormatterFactory.fromXml.

        <join [sep=...]>      : Explicitly join the contents, optionally using a separator string

        <value path=...>      : Evaluate an XPath from the message being formatted

        <value preference=...> : Evaluate a preference, using the element's contents as a default.

        <value>text</value>   : Literal text. (<value> is very similar to a ruleset's <return>)
                                Text may be included anywhere, but it is usually stripped of leading
                                and trailing whitespace. <value> does not do this.

        <input>               : The input to this formatter

        <pipe>                : This element must have two child elements.
                                The first element is evaluated normally, then its
                                output is used as input when evaluating the second element.

        <tag name=...>        : Generate a Nouvelle tag. Attributes can be defined with <attribute> elements,
                                or by attributes of the <tag> element. The contents of this element are concatenated
                                and placed inside the Nouvelle tag.

        <attribute name=...>  : Define an attribute on the enclosing <tag>

        <preference name=...> : Set a preference to the enclosed string if it hasn't already been set.
                                This only affects formatters below it, not formatters above it.
                                Only the first preference of a particular name is used- this makes
                                the outermost user of a formatter ultimately responsible for setting preferences,
                                while inner formatters can still easily set defaults.
       """
    def loadParametersFrom(self, xml):
        self.format = CompositeFormatterParser(xml).result


class CompositeFormatterParser(XML.XMLObjectParser):
    """Parses CompositeFormatter's arguments into a format() function"""
    def join(self, l, args, separator=""):
        """Given a list of literals and functions, reduces them to a
           single string or list as appropriate, optionally inserting
           a separator string between each.
           """
        results = []
        allStrings = True
        for f in l:
            if f is None:
                # Ignore Nones
                continue
            elif callable(f):
                # Call child functions, recording their result
                result = f(args)
                if not result:
                    continue
            elif f:
                # Allow non-callable parse results for literals
                result = f

            results.append(result)
            if type(result) not in types.StringTypes:
                allStrings = False

        # If all results are strings, we can join the list now.
        # This might not be the case if, for example, we're generating
        # a Nouvelle document tree.
        if allStrings:
            return separator.join(results)
        else:
            joinedResultList = []
            for result in results:
                if joinedResultList:
                    joinedResultList.append(separator)
                joinedResultList.append(result)
            return joinedResultList

    def element_formatter(self, element):
        """Another formatter, or possibly our root element. If it has any
           attributes, we need to get a formatter using the FormatterFactory.
           If not, parse and concatenate the child elements.
           """
        if element.attributes:
            # We need to let the formatter factory resolve this
            import Formatters

            # Evaluate once at parse time to check validity
            Formatters.factory.fromXml(element)

            def formatUsingFactory(args):
                # This must bind to a particular formatter at runtime rather than parse time
                # to handle autoformatting properly.
                return Formatters.factory.fromXml(element, args.message).format(args)
            return formatUsingFactory

        else:
            # This is a nested CompositeFormatterParser, we must handle these
            # here since that's what this class is for. We always have at least
            # one of these, at the root of the parsed XML document.
            children = list(self.childParser(element))
            def joinChildren(args):
                return self.join(children, args)
            return joinChildren

    def element_join(self, element):
        children = list(self.childParser(element))
        sep = element.attributes.get("sep", "")
        def explicitJoin(args):
            return self.join(children, args, sep)
        return explicitJoin

    def element_value(self, element):
        """Include a value obtained from an XPath, a preference, or from this element's contents"""
        if element.hasAttributeNS(None, 'path'):
            xp = XML.XPath(element.getAttributeNS(None, 'path'))
            def formatXPathValue(args):
                nodes = xp.queryForNodes(args.message.xml)
                if nodes:
                    return XML.allText(nodes[0]).strip()
            return formatXPathValue

        elif element.hasAttributeNS(None, 'preference'):
            name = element.getAttributeNS(None, 'preference')
            default = XML.shallowText(element)
            def evaluatePref(args):
                return args.getPreference(name, default)
            return evaluatePref

        else:
            # No path, return this node's contents
            return str(element)

    def element_preference(self, element):
        """Generates no output on its own, but sets a preference in the current FormatterArgs"""
        name = element.getAttributeNS(None, 'name')
        value = XML.shallowText(element)
        def setPreference(args):
            args.preferences.setdefault(name, value)
        return setPreference

    def element_input(self, element):
        """Returns the formatter's input"""
        def formatInput(args):
            return args.input
        return formatInput

    def element_pipe(self, element):
        """Evaluate the first child element, using its result as the input to the second child"""
        children = list(XML.getChildElements(element))
        if len(children) != 2:
            raise XML.XMLValidityError("<pipe> must have exactly two child elements")
        pipeInput = self.parse(children[0])
        pipeOutput = self.parse(children[1])

        def formatPipe(args):
            return pipeOutput(args.copy(input = pipeInput(args)))
        return formatPipe

    def element_attribute(self, element):
        """<attribute> elements inside a <tag> are handled by element_tag(), this just
           returns an error if an attribute is found elsewhere.
           """
        raise XML.XMLValidityError("<attribute> is only allowed inside a <tag>")

    def element_tag(self, element):
        """Support for generating Nouvelle tags"""
        from Nouvelle import tag

        # Make a dict of attributes defined in the element,
        # as these are always static. Other attributes may depend
        # on the input or message, so they have to be filled in dynamically.
        staticAttrs = dict(element.attributes)
        tagName = staticAttrs['name']
        del staticAttrs['name']

        # For each dynamically defined attribute, this maps an attribute
        # name to a parsed element list that can be passed to self.join()
        dynamicAttrs = {}

        # Scan child nodes, separating them into attributes and data. Attributes
        # get parsed, and sorted into staticAttrs and dynamicAttrs. Other children
        # get parsed and added to
        children = []
        for child in element.childNodes:
            if child.nodeType == child.ELEMENT_NODE and child.nodeName == "attribute":
                attrName = child.getAttributeNS(None, 'name')
                dynamicAttrs[attrName] = self.childParser(child)
            else:
                children.append(child)

        def formatTag(args):
            # Generate a final attribute list from both static and dynamic attributes
            attrs = dict(staticAttrs)
            for name, value in attrs.iteritems():
                attrs[name] = self.join(value, args)

            # Join our non-attribute children to form the tag's content
            content = self.join(children, args)

            return tag(tagName, **attrs)[ content ]
        return formatTag

    def parseString(self, element):
        """If there is any non-whitespace text, include it literally"""
        text = str(element).strip()
        if text:
            return text


def parseCompositeFormatter(xml):
    """A convenience function for defining format functions as XML
       composite formatters at import-time.
       """
    return CompositeFormatterParser(xml).result


class NoFormatterError(Exception):
    pass


class FormatterFactory:
    """An object that keeps track of a collection of Formatter objects
       and can create formatters to match particular requirements.

       This class should be constructed with a dictionary to pull Formatter
       instances out of and catalog.
       """
    def __init__(self, d):
        self.nameMap = {}
        self.mediumMap = {}
        for name, obj in d.iteritems():
            try:
                if issubclass(obj, Formatter):
                    self.nameMap[name] = obj
                    if obj.detector and obj.medium:
                        self.mediumMap.setdefault(obj.medium, []).append(obj)
            except TypeError:
                pass

    def findName(self, name):
        """Find a particular formatter by name"""
        try:
            cls = self.nameMap[name]
        except KeyError:
            raise NoFormatterError("No such formatter %r" % name)
        return cls()

    def findMedium(self, medium, message=None):
        """Find a formatter for the given medium and matching the given message.
           If None is given as the message, this will only validate the medium
           and return None if the medium itself is fine.
           """
        try:
            l = self.mediumMap[medium]
        except KeyError:
            raise NoFormatterError("No formatters for the %r medium" % medium)
        if message:
            for cls in l:
                if cls.detector(message):
                    return cls()
            raise NoFormatterError("No matching formatters for the %r medium" % medium)

    def fromXml(self, xml, message=None):
        """Create a formatter to match the given <formatter> element.
           The formatter element may have a 'name' attribute to specify a particular
           formatter class or a 'medium' attribute to automatically find a matching
           formatter to output to a given medium. If neither of these are given,
           a CompositeFormatter is created.

           If 'message' is None and a medium is requested rather than a particular
           formatter, this will return None after validating the medium.
           """
        attrNames = [attr.name for attr in xml.attributes.itervalues()]

        if not attrNames:
            f = CompositeFormatter()

        elif attrNames == ['name']:
            f = self.findName(xml.getAttributeNS(None, 'name'))

        elif attrNames == ['medium']:
            f = self.findMedium(xml.getAttributeNS(None, 'medium'), message)

        else:
            # It's important to disallow unknown attributes here, so that we know for
            # sure which formatters must be delegated to this method and which can be
            # handled by CompositeFormatter safely.
            raise XML.XMLValidityError("<formatter> must have either a 'name' attribute, a 'medium' attribute, or no attributes")

        if f:
            f.loadParametersFrom(xml)
        return f


def _test():
    import doctest, Message
    return doctest.testmod(Message)

if __name__ == "__main__":
    _test()

### The End ###
