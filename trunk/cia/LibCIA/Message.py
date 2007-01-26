""" LibCIA.Message

Classes to represent, distribute, and filter messages represented
by XML documents.
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

from twisted.python import log
import time, types, re
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
    immutable = True

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

         >>> msg = Message(open('xml/samples/simple-message.xml'))

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
        path = element.getAttributeNS(None, 'path')
        xp = XML.XPath(XML.pathShortcuts.get(path, path))

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
            # Use queryobject then str() so that matched
            # nodes without any text still give us at least
            # the empty string. This is important so that <find>
            # with an empty search string can be used to test
            # for the existence of an XPath match.
            nodes = xp.queryObject(msg)
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
                if child and not child(msg):
                    return False
            return True
        return filterAnd

    def element_or(self, element):
        """Evaluates to True if and only if any child function evaluates to True"""
        childFunctions = list(self.childParser(element))
        def filterOr(msg):
            for child in childFunctions:
                if child and child(msg):
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
                if child and child(msg):
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
    # If non-none, this is a Filter string that can be tested against
    # a message to detect whether this formatter is applicable.
    filter = None

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

    def loadParametersFrom(self, xml, unused=None):
        """This is given a <formatter> element possibly containing
           extra parameters for the formatter to process and store.
           Any problems should be signalled with an XML.XMLValidityError.

           By default, this tries to find a param_* handler for each
           element it comes across.

           Returns a set object, containing the names of all unused
           parameters. This allows callers, during validation, to look
           for misspelled or otherwise unused elements.
           """
        unused = set()
        for tag in XML.getChildElements(xml):
            f = getattr(self, 'param_'+tag.nodeName, None)
            if f:
                f(tag)
            else:
                unused.add(tag.nodeName)
        return unused


class MarkAsHidden(str):
    """This object acts like an empty string, but has the side effect
       of hiding a containing <autoHide> element. This should be returned
       by components when their result is nonexistent or not applicable,
       so that the colors, prefixes, or suffixes around that component
       are also hidden.
       """
    def __new__(self):
        return str.__new__(self, "")


class ModularFormatter(Formatter):
    """A Formatter consisting of multiple components that can be rearranged
       by changing the 'format' parameter. The current component arrangement
       is stored as a DOM tree in 'componentTree', generated from the XML
       string in 'defaultComponentTree'. It is overridden in param_format().
       """
    defaultComponentTree = None
    componentTree = None

    # Cache a parsed defaultComponentTree
    _cachedDefaultTree = None
    _defaultTreeOwner = None

    def param_format(self, tree):
        """Handles the <format> parameter. Note that since the format generally
           can't be shared between different types of formatters, there is an
           optional (but recommended) 'appliesTo' attribute that lists one or more
           space separated formatter names.
           """
        appliesTo = tree.getAttributeNS(None, 'appliesTo')
        if appliesTo:
            if self.__class__.__name__ not in appliesTo.split():
                return

        self.componentTree = tree

        # Validate the component tree by parsing a null message once.
        # Any errors in the components or their arguments should
        # trigger XMLValidityErrors at this point.
        self.formatMessage(Message("<message/>"))

    def format(self, args):
        """The formatter entry point. This just finds the current component
           tree and invokes walkComponents and joinComponents on it.
           """
        # Parse the default component tree, caching it per-class
        if self.__class__._defaultTreeOwner is not self.__class__.defaultComponentTree:
            self.__class__._defaultTreeOwner = self.__class__.defaultComponentTree
            self.__class__._cachedDefaultTree = XML.parseString(self.__class__.defaultComponentTree).documentElement

        # This will use the default component tree if it hasn't been overridden in this instance
        tree = self.componentTree or self.__class__._cachedDefaultTree
        return self.joinComponents(self.walkComponents(tree.childNodes, args))

    def evalComponent(self, node, args):
        """Given a DOM node for a component, evaluate it and return the result
           list. An empty list always indicates that the component ran but
           had nothing to format- we return None if the node is ignored.

           Text nodes return a list with only their data. Newline characters
           in a text node are converted to spaces, so newlines and tabs can be used
           to prettify the XML without it affecting the final output- this combined with
           the redundant whitespace elimination in joinComponents() gives HTML-like
           semantics. Literal linebreaks can be obtained with the <br/> component.

           Elements invoke the corresponding component_* handler, which must
           return a sequence.
           """
        if node.nodeType == node.TEXT_NODE:
            return [node.data.replace("\n", " ")]

        elif node.nodeType == node.ELEMENT_NODE:
            f = getattr(self, "component_" + node.nodeName, None)
            if f:
                return f(node, args)
            else:
                raise XML.XMLValidityError("Unknown component name in %s: %r" %
                                            (self.__class__.__name__, node.nodeName))

    def walkComponents(self, nodes, args):
        """Walk through all the given XML nodes, returning a list of formatted objects."""
        results = []
        for node in nodes:
            results.extend(self.evalComponent(node, args))
        return results

    def component_autoHide(self, element, args):
        """A built-in component that evaluates all children, hiding them all if
           any return an empty list. This makes it easy to add prefixes, suffixes, or
           other formatting to a component that disappears when it does.
           """
        results = self.walkComponents(element.childNodes, args)
        for result in results:
            if isinstance(result, MarkAsHidden):
                # Hidden markers don't propagage, return a normal empty list
                return []
        return results

    def component_br(self, element, args):
        """A built-in component that just returns a newline"""
        return ["\n"]

    def textComponent(self, element, args, *path):
        """A convenience function for defining components that just look for a node
           in the message and return its shallowText.
           """
        element = XML.dig(args.message.xml, *path)
        if element:
            return [XML.shallowText(element)]
        else:
            return [MarkAsHidden()]

    def component_text(self, element, args):
        """This is a generic version of textComponent, in which 'path' can
           be specified by users. Any textComponent can be rewritten as a
           <text> component.
           """
        path = element.getAttributeNS(None, 'path')
        if not path:
            raise XML.XMLValidityError("The 'path' attribute on <text> is required.")
        xp = XML.XPath(XML.pathShortcuts.get(path, path))

        nodes = xp.queryObject(args.message)
        if nodes:
            return [XML.shallowText(nodes[0])]
        else:
            return [MarkAsHidden()]

    def joinComponents(self, results):
        """Given a list of component results, return the formatter's final result.
           The default implementation converts to strings, joins, then removes excess
           whitespace. Subclasses should override this if they're formatting Nouvelle
           trees or other non-string data types.
           """
        return re.sub(r'[ \t]+', ' ', ''.join(map(unicode, results))).strip()


filterCache = {}

def getCachedFilter(xml):
    """Get the Filter instance corresponding to the given XML
       fragment, creating one if it doesn't exist yet. As the cache
       doesn't get cleaned yet, this should only be used for
       non-user-provided Filter strings.
       """
    global filterCache
    try:
        return filterCache[xml]
    except KeyError:
        f = Filter(xml)
        filterCache[xml] = f
        return f


class NoFormatterError(Exception):
    pass


class FormatterFactory:
    """An object that keeps track of a collection of Formatter objects
       and can create formatters to match particular requirements.

       This class should be constructed with a dictionary to pull Formatter
       instances out of and catalog.
       """
    def __init__(self, *args):
        self.nameMap = {}
        self.mediumMap = {}
        for arg in args:
            self.install(arg)

    def install(self, obj):
        """Install a module, Formatter instance, or dict containing Formatters
           such that they are searchable by this FormatterFactory.
           """
        if type(obj) == type(Formatter) and issubclass(obj, Formatter):
            self.nameMap[obj.__name__] = obj
            if obj.filter and obj.medium:
                self.mediumMap.setdefault(obj.medium, []).append(obj)

        elif type(obj) == dict:
            for subobj in obj.itervalues():
                # We only look at Formtter instances inside dicts, to
                # avoid recursively drilling down into modules.
                if type(subobj) == type(Formatter) and issubclass(subobj, Formatter):
                    self.install(subobj)

        elif type(obj) == types.ModuleType:
            self.install(obj.__dict__)

    def findName(self, name):
        """Find a particular formatter by name"""
        try:
            cls = self.nameMap[name]
        except KeyError:
            raise NoFormatterError('No such formatter "%s"' % name)
        return cls()

    def _getFormattersForMedium(self, medium):
        """Return a non-empty list with all formatters for a particular medium."""
        l = self.mediumMap.get(medium)
        if l:
            return l
        else:
            raise NoFormatterError('No formatters for the "%s" medium' % medium)

    def findMedium(self, medium, message):
        """Find a formatter for the given medium and matching the given message."""
        for cls in self._getFormattersForMedium(medium):
            if getCachedFilter(cls.filter)(message):
                return cls()
        raise NoFormatterError('No matching formatters for the "%s" medium' % medium)

    def _reportUnusedParameters(self, unused):
        if unused:
            l = [ '"%s"' % el for el in unused ]
            l.sort()
            raise XML.XMLValidityError(
                "Some formatter parameters were not recognized: " + ", ".join(l))

    def fromXml(self, xml, message=None):
        """Create a formatter to match the given <formatter> element.
           The formatter element may have a 'name' attribute to specify a particular
           formatter class or a 'medium' attribute to automatically find a matching
           formatter to output to a given medium.

           If 'message' is None and a medium is requested rather than a particular
           formatter, this will return None after validating the medium.
           """
        attrNames = [attr.name for attr in xml.attributes.values()]

        # Load a single formatter, by name
        if attrNames == ['name']:
            f = self.findName(xml.getAttributeNS(None, 'name'))
            self._reportUnusedParameters(f.loadParametersFrom(xml))
            return f

        # Search for a matching formatter, by medium.  If we have no
        # message to match, this will validate the parameters against
        # all formatters for the medium.
        if attrNames == ['medium']:
            medium = xml.getAttributeNS(None, 'medium')
            if message:
                f = self.findMedium(medium, message)
                f.loadParametersFrom(xml)
                return f
            else:
                # Take the intersection of all formatters' unused parameters
                unused = None
                for cls in self._getFormattersForMedium(medium):
                    next = cls().loadParametersFrom(xml)
                    if unused is None:
                        unused = next
                    else:
                        unused.intersection_update(next)
                self._reportUnusedParameters(unused)
                return

        raise XML.XMLValidityError("<formatter> must have either a 'name' attribute or a 'medium' attribute")

### The End ###
