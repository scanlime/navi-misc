""" LibCIA.Ruleset

Rulesets tie together all the major components of CIA. They hold
filters and formatters that define which messages they apply
to and how to display those messages, and specify a URI that
gives the finished message a destination.

A Ruleset by itself is just a function that, given a message,
returns a formatted version of that message or None. To be of
any use for actually processing messages, a URIHandler for the
message's URI is looked up, and a RulesetDelivery object is used
to join the Ruleset and URIHandler. The RulesetDelivery is registered
as a client of the Message.Hub. Upon receiving a message, it runs
it through the ruleset and if a result comes out, sends that result
to the URIHandler.

RulesetStorage holds a persistent list of rulesets, with URIHandlers
assigned to each via URIRegistry.

RulesetController attaches to a Message.Hub and listens for messages
used to store and query rulesets in a RulesetStorage.
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

import XML, Message
from twisted.python import log
import sys, traceback


class RulesetReturnException(Exception):
    """Used to implement <return> in a Ruleset.
       This exception is caught in the <ruleset> root node,
       causing it to return with the current result value right away.
       """
    pass


class Ruleset(XML.XMLFunction):
    r"""A ruleset is a tree that makes decisions about an incoming message
        using filters and generates output using formatters.
        A ruleset may contain the following elements, which are evaluated sequentially.
        The output from the last formatter is returned upon calling this ruleset, and
        each formatter is given the previous formatter's output as input, so they may be stacked.

        <formatter name="foo">   : Applies the formatter named 'foo'

        <formatter medium="irc"> : Automatically picks a formatter for the particular
                                   input message and the given medium type

        any Filter tag           : Evaluates the filter, terminating the current rule
                                   if it returns false.

        <rule>                   : Marks a section of the ruleset that can be exited
                                   when a filter returns false. A <rule> with a filter
                                   as the first child can be used to create conditionals.

        <return>                 : Normally the result of the last formatter is returned,
                                   this causes the text inside the <return>
                                   tag to be returned immediately. An empty <return>
                                   tag causes None to be returned from this ruleset..

        <break>                  : Return immediately from the ruleset, but don't change
                                   the current return value

        <ruleset [uri="foo://bar"]> : Like <rule>, but this is always the root tag.
                                      It may include a 'uri' attribute specifying the
                                      destination of a message passed through this ruleset.
                                      This ruleset object will store a URI if one is present,
                                      but does not require one. The typical application of
                                      rulesets however does require a URI to be specified.

        >>> msg = Message.Message(
        ...           '<message>' +
        ...              '<source>' +
        ...                 '<project>robo-hamster</project>' +
        ...              '</source>' +
        ...              '<body>' +
        ...                 '<colorText>' +
        ...                    '<b>Hello</b>World' +
        ...                 '</colorText>' +
        ...              '</body>' +
        ...           '</message>')

        >>> r = Ruleset('<ruleset><formatter medium="irc"/></ruleset>')
        >>> r(msg)
        '\x02Hello\x0fWorld'

        >>> r = Ruleset('<ruleset><return>Boing</return><formatter medium="irc"/></ruleset>')
        >>> r(msg)
        'Boing'

        >>> r = Ruleset('<ruleset>' +
        ...                 '<formatter medium="irc"/>' +
        ...                 '<rule>' +
        ...                    '<match path="/message/source/project">robo-hamster</match>' +
        ...                    '<return>*censored*</return>' +
        ...                 '</rule>' +
        ...             '</ruleset>')
        >>> r(msg)
        '*censored*'

        >>> r = Ruleset('<ruleset>' +
        ...                 '<formatter medium="irc"/>' +
        ...                 '<rule>' +
        ...                    '<match path="/message/source/project">duck-invader</match>' +
        ...                    '<return>Quack</return>' +
        ...                 '</rule>' +
        ...                 '<formatter name="IRCProjectName"/>' +
        ...             '</ruleset>')
        >>> r(msg)
        '\x02robo-hamster:\x0f \x02Hello\x0fWorld'

        >>> r = Ruleset('<ruleset><return/></ruleset>')
        >>> r(msg) is None
        True

        >>> Ruleset('<ruleset/>').uri is None
        True
        >>> Ruleset('<ruleset uri="sponge://"/>').uri
        'sponge://'
        """
    requiredRootElement = "ruleset"

    def element_ruleset(self, element):
        """<ruleset> for the most part works just like <rule>, but since
           it's the root node it's responsible for initializing and returning
           the ruleset's result.
           """
        # Go ahead and store the URI attribute if we have one.
        # If not, this will be None.
        self.uri = element.getAttribute('uri')

        # Create a function to evaluate this element as a <rule> would be evaluated
        ruleFunc = self.element_rule(element)

        # Now wrap this function in one that performs our initialization and such
        def rulesetRoot(msg):
            self.result = None
            try:
                ruleFunc(msg)
            except RulesetReturnException:
                pass
            result = self.result
            del self.result
            return result
        return rulesetRoot

    def element_rule(self, element):
        """Evaluate each child element in sequence until one returns False"""
        childFunctions = [self.parse(child) for child in element.elements()]
        def rulesetRule(msg):
            for child in childFunctions:
                if not child(msg):
                    break
            return True
        return rulesetRule

    def element_return(self, element):
        """Set the current result and exit the ruleset immediately"""
        def rulesetReturn(msg):
            self.result = str(element)
            if not self.result:
                self.result = None
            raise RulesetReturnException()
        return rulesetReturn

    def element_break(self, element):
        """Just exit the ruleset immediately"""
        def rulesetBreak(msg):
            raise RulesetReturnException()
        return rulesetBreak

    def element_formatter(self, element):
        """Creates a Formatter instance matching the element's description,
           returns a function that applies the formatter against the current
           message and result.
           """
        if element.hasAttribute('name'):
            formatter = Message.NamedFormatter(element['name'])
        elif element.hasAttribute('medium'):
            formatter = Message.AutoFormatter(element['medium'])
        else:
            raise XML.XMLValidityError("<formatter> must have a 'name' or 'medium' attribute")

        def rulesetFormatter(msg):
            self.result = formatter.format(msg, self.result)
            return True
        return rulesetFormatter

    def unknownElement(self, element):
        """Check whether this element is a filter before giving up"""
        try:
            f = Message.Filter(element)
        except XML.XMLValidityError:
            # Nope, not a filter.. let XMLFunction give an error
            XML.XMLFunction.unknownElement(self, element)

        # We can just return the filter, since it has the same calling
        # signature as any of our other element implementation functions.
        return f


class BaseURIHandler(object):
    """A URIHandler instance defines a particular URI scheme, actions to
       be taken when a URI matching that scheme is assigned or unassigned
       a ruleset, and a way to deliver messages to a matching URI.
       This is an abstract base class, it must be subclassed
       to implement a particular URI scheme.
       """
    # Subclasses must either set this to the name of the URI scheme
    # (such as 'irc' or 'http') or override the 'detect' function.
    scheme = None

    def detect(self, uri):
        """Return True if this URI handler is applicable to the given URI.
           The default implementation checks it against our URI scheme.
           """
        return uri.startswith(self.scheme + ':')

    def assigned(self, uri, newRuleset):
        """This optional function is called when a URI matching this handler is
           assigned a new ruleset. This includes being assigned a ruleset after
           previously not having one. Generally this is used whenever a connection
           must be initialized to the object referred to by the URI.
           """
        pass

    def unassigned(self, uri):
        """This optional function is called when a URI matching this handler
           has its ruleset removed. Generally this is used to disconnect any
           connection formed by assigned().
           """
        pass

    def message(self, uri, content):
        """Deliver a message to the given URI. The 'content' will be whatever
           results from parsing a ruleset generally.
           """
        pass


class RulesetDelivery(object):
    """Combines the given Ruleset and URIHandler handlers into a callable
       object that can be used as a Message.Hub client. Incoming messages
       are tested against the ruleset, and if a non-None result is generated
       that is sent to the given URIHandler.
       """
    def __init__(self, ruleset, uriHandler):
        self.ruleset = ruleset
        self.uriHandler = uriHandler

    def __call__(self, message):
        # We catch exceptions here and log them, preventing one bad ruleset
        # or URI handler from preventing message delivery to other clients.
        # We can't do this in the Message.Hub because sometimes it's good for
        # exceptions to be propagated to the original sender of the message.
        # In ruleset delivery however, messages never have a return value
        # and shouldn't raise exceptions.
        try:
            result = self.ruleset(message)
            if result:
                self.uriHandler.message(self.ruleset.uri, result)
        except:
            e = sys.exc_info()[1]
            log.msg("Exception occurred in RulesetDelivery\n" +
                    "--- Original message\n%s\n--- Exception\n%s" %
                    (message, "".join(traceback.format_exception(*sys.exc_info()))))


class UnsupportedURI(Exception):
    """Raised on failure in URIRegistry.query()"""
    pass


class URIRegistry(object):
    """A central authority for storing URIHandler instances and looking up
       one appropriate for a given URI.
       """
    def __init__(self, handlers=[]):
        self.handlers = handlers

    def register(self, handler):
        self.handlers.append(handler)

    def query(self, uri):
        """Return a URIHandler instance appropriate for the given URI.
           Raises an UnsupportedURI exception on failure.
           """
        for handler in self.handlers:
            if handler.detect(uri):
                return handler
        raise UnsupportedURI("No registered URI handler supports %r" % uri)


class RulesetStorage(XML.XMLStorage):
    """A persistent list of Rulesets, stored in RulesetDelivery objects
       with URIHandlers looked up from the provided URIRegistry.
       The generated RulesetDelivery objects are automatically added
       to and removed from the supplied Message.Hub.
       """
    def __init__(self, fileName, hub, uriRegistry):
        self.uriRegistry = uriRegistry
        self.hub = hub
        XML.XMLStorage.__init__(self, fileName, rootName='rulesets')

    def emptyStorage(self):
        # Remove any existing rulesets from the Message.Hub
        if hasattr(self, 'rulesetMap'):
            for value in self.rulesetMap.itervalues():
                self.hub.delClient(value)

        # self.rulesetMap maps URIs to RulesetDelivery instances
        self.rulesetMap = {}

    def store(self, rulesetXml):
        """Find a URIHandler for the given ruleset and add it to
           our mapping and to the hub. 'ruleset' is given as a
           domish.Element.

           Storing an empty ruleset for a particular URI is equivalent
           to removing that URI's ruleset.

           It is important that this function doesn't actually
           remove or change the ruleset in quesion unless any possible
           input errors have already been detected.
           """
        # Important to make sure the ruleset parses first of all
        ruleset = Ruleset(rulesetXml)

        # We need to find an appropriate URI handler whether our ruleset
        # is empty or not, since we have to be able to notify the handler.
        handler = self.uriRegistry.query(ruleset.uri)

        # Is this ruleset non-empty?
        # Note that the obvious xml.elements() doesn't work- it will
        # always be true, because the returned object is a generator.
        if ruleset.xml.firstChildElement():
            # It's important that we give the URI handler a chance
            # to return errors before removing the old ruleset.
            handler.assigned(ruleset.uri, ruleset)

            # If there was an old ruleset, remove its hub client
            if self.rulesetMap.has_key(ruleset.uri):
                self.hub.delClient(self.rulesetMap[ruleset.uri])

            # Stick on an appropriate URI handler and add the
            # resulting RulesetDelivery instance to the message hub
            delivery = RulesetDelivery(ruleset, handler)
            self.rulesetMap[ruleset.uri] = delivery
            self.hub.addClient(delivery)
            log.msg("Set ruleset for %r:\n%s" % (ruleset.uri,
                                                 XML.prettyPrint(ruleset.xml)))
        else:
            # Remove the ruleset completely if there was one
            if self.rulesetMap.has_key(ruleset.uri):
                self.hub.delClient(self.rulesetMap[ruleset.uri])
                del self.rulesetMap[ruleset.uri]

            log.msg("Removed ruleset for %r" % ruleset.uri)
            handler.unassigned(ruleset.uri)

    def flatten(self):
        """Return a flat list of all Ruleset objects so we can store 'em"""
        return [delivery.ruleset for delivery in self.rulesetMap.itervalues()]


class RulesetController(object):
    """Listens for messages used to store and query rulesets"""
    def __init__(self, hub, storage):
        self.storage = storage
        self.hub = hub
        self.addClients()

    def addClients(self):
        """Add our clients to the Message.Hub. This uses an extra
           level of indirection so that rebuild() can replace references
           to our callbacks correctly.
           """
        self.hub.addClient(lambda msg: self.storeRuleset(msg),
                           Message.Filter('<find path="/message/body/ruleset">'))
        self.hub.addClient(lambda msg: self.queryRulesets(msg),
                           Message.Filter('<find path="/message/body/queryRulesets">'))
        self.hub.addClient(lambda msg: self.queryUriList(msg),
                           Message.Filter('<find path="/message/body/queryUriList">'))

    def storeRuleset(self, message):
        """Handle messages instructing us to add, modify, or remove
           rulesets. The message body contains one or more <ruleset>
           elements with 'uri' attributes. An empty ruleset for a particular
           URI is equivalent to removing that URI's ruleset.
           """
        for child in message.xml.body.elements():
            if child.name == 'ruleset':
                self.storage.store(child)
        self.storage.save()

    def queryRulesets(self, message):
        """Handles messages containing a <queryRulesets> tag in its body.
           If queryRulesets includes a 'uri' attribute, the ruleset for that
           URI is returned in a 1-item list, or an empty list if that URI has no
           ruleset. Without the 'uri' attribute, returns a list of strings representing
           all rulesets.
           """
        tag = message.xml.body.queryRulesets
        uri = tag.getAttribute('uri')
        results = []
        for delivery in self.storage.rulesetMap.itervalues():
            if uri is not None and delivery.ruleset.uri == uri:
                results.append(str(delivery.ruleset))
        return results

    def queryUriList(self, message):
        """Handles messages containing a <queryUriList> tag in its body.
           Returns a list of URIs with rulesets attached.
           """
        return self.storage.rulesetMap.keys()


class InvalidURIException(Exception):
    """An exception that URI handlers can raise when a URI is invalid"""
    pass


def _test():
    import doctest, Ruleset
    return doctest.testmod(Ruleset)

if __name__ == "__main__":
    _test()

### The End ###
