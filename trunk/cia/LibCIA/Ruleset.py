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

import XML, Message, Debug, Security, RpcServer, Formatters
from twisted.python import log
from twisted.internet import defer, reactor
import sys, traceback, re, os


class InvalidURIException(Exception):
    """An exception that URI handlers can raise when a URI is invalid"""
    pass


class RulesetInterface(RpcServer.Interface):
    """An XML-RPC interface used to set and query the rulesets in a RulesetStorage"""
    def __init__(self, storage):
        self.storage = storage
        RpcServer.Interface.__init__(self)

    def protected_store(self, xml):
        """Stores a ruleset provided as XML text. Deleting a ruleset is equivalent
           to storing an empty one with the same URI.
           """
        self.storage.store(xml)

    def caps_store(self, path, xml):
        """Generate a list of acceptable capabilities to grant access to 'store'.
           In addition to the usual ones, allow ('ruleset.uri', x) where x is the
           ruleset's URI.
           """
        uri = XML.parseString(xml).documentElement.getAttributeNS(None, 'uri')
        return self.makeDefaultCaps(path) + [('ruleset.uri', uri)]

    def protected_grantUri(self, uri, uid):
        """Returns a key for the capability ('ruleset.uri', uri).
           This can be used to delegate control of a particular URI's ruleset-
           an administrator would call this function to retrieve a key for a particular
           URI, then hand that to someone else who would only have the ability
           to edit that URI.
           """
        return Security.User(int(uid)).grant(('ruleset.uri', str(uri)))

    def xmlrpc_getUriList(self):
        """Return a list of all URIs with non-empty rulesets"""
        return self.storage.rulesetMap.keys()

    def xmlrpc_getRuleset(self, uri):
        """Return the ruleset associated with the given URI, or False if there is no ruleset"""
        try:
            return str(self.storage.rulesetMap[uri].ruleset)
        except KeyError:
            return False

    def xmlrpc_getRulesetMap(self):
        """Returns all rulesets in the form of a mapping from URI to ruleset text"""
        results = {}
        for delivery in self.storage.rulesetMap.itervalues():
            results[delivery.ruleset.uri] = str(delivery.ruleset)
        return results


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

        <formatter *>               : Chooses and applies a formatter. All attributes and
                                      contents are passed on to the FormatterFactory, refer
                                      to its documentation for more information.

        any Filter tag              : Evaluates the filter, terminating the current rule
                                      if it returns false.

        <rule>                      : Marks a section of the ruleset that can be exited
                                      when a filter returns false. A <rule> with a filter
                                      as the first child can be used to create conditionals.

        <return [path='/foo/bar']>  : Normally the result of the last formatter is returned.
                                      This returns from the ruleset immediately. If a path
                                      is supplied, the string value of that XPath match is
                                      returned. If not, the content of the <return> tag is
                                      returned. An empty tag or no XPath match will cause
                                      the ruleset to return None.

        <break>                     : Return immediately from the ruleset, but don't change
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
        u'Boing'

        >>> r = Ruleset('<ruleset>' +
        ...                 '<formatter medium="irc"/>' +
        ...                 '<rule>' +
        ...                    '<match path="/message/source/project">robo-hamster</match>' +
        ...                    '<return>*censored*</return>' +
        ...                 '</rule>' +
        ...             '</ruleset>')
        >>> r(msg)
        u'*censored*'

        >>> r = Ruleset('<ruleset>' +
        ...                 '<formatter medium="irc"/>' +
        ...                 '<rule>' +
        ...                    '<match path="/message/source/project">duck-invader</match>' +
        ...                    '<return>Quack</return>' +
        ...                 '</rule>' +
        ...                 '<formatter name="IRCProjectName"/>' +
        ...             '</ruleset>')
        >>> r(msg)
        u'\x02robo-hamster:\x0f \x02Hello\x0fWorld'

        >>> r = Ruleset('<ruleset><return/></ruleset>')
        >>> r(msg) is None
        True

        >>> r = Ruleset('<ruleset><return path="/message/source/project"/></ruleset>')
        >>> r(msg)
        u'robo-hamster'

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
        self.uri = element.getAttributeNS(None, 'uri') or None

        # URIs are always encoded if necessary, since just about everywhere we'd need to
        # use a URI we can't support Unicode yet. Specific examples are IRC servers/channels
        # and as dict keys in an XML-RPC response.
        if type(self.uri) is unicode:
            self.uri = self.uri.encode()

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
        childFunctions = list(self.childParser(element))
        def rulesetRule(msg):
            for child in childFunctions:
                if child:
                    if not child(msg):
                        break
            return True
        return rulesetRule

    def element_return(self, element):
        """Set the current result and exit the ruleset immediately"""
        if element.hasAttributeNS(None, 'path'):
            path = element.getAttributeNS(None, 'path')
            xp = XML.XPath(XML.pathShortcuts.get(path, path))
            # Define a rulesetReturn function that returns the value of the XPath
            def rulesetReturn(msg):
                nodes = xp.queryObject(msg)
                if nodes:
                    self.result = XML.allText(nodes[0]).strip()
                else:
                    self.result = None
                raise RulesetReturnException()
            return rulesetReturn

        else:
            # No path, define a rulesetReturn function that returns this element's string value
            def rulesetReturn(msg):
                self.result = XML.shallowText(element)
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
        # Evaluate this once at parse-time so any silly errors
        # like unknown formatters or media can be detected.
        Formatters.getFactory().fromXml(element)

        def rulesetFormatter(msg):
            args = Message.FormatterArgs(msg, self.result)
            self.result = Formatters.getFactory().fromXml(element, msg).format(args)
            return True
        return rulesetFormatter

    def unknownElement(self, element):
        """Check whether this element is a filter before giving up"""
        try:
            f = Message.Filter(element)
        except XML.UnknownElementError:
            # Nope, not a filter.. let XMLFunction give an error
            XML.XMLFunction.unknownElement(self, element)

        # We can just return the filter, since it has the same calling
        # signature as any of our other element implementation functions.
        return f

    def isEmpty(self):
        """Returns True if the ruleset has no contents"""
        return not XML.hasChildElements(self.xml.documentElement)


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

    def message(self, uri, message, content):
        """Deliver a message to the given URI. The 'message' is the original
           unprocessed message resulting in this content, and the 'content'
           is the result of passing the message through whatever ruleset
           caused it to end up here.
           """
        pass

    def rulesetsRefreshed(self):
        """If the handler is interested, it can override this method to be
           notified when rulesets are done refreshing.
           """
        pass


class RegexURIHandler(BaseURIHandler):
    """A URIHandler that validates and parses URIs using a regular expression.
       This class provides  a parseURI method that generates a dictionary of
       groups matched in the regex, or raises an UnsupportedURI exception if
       the regex does not match. It implements a default assigned() function
       that runs parseURI just to cause an error in setting the URI if it's invalid.
       """
    regex = None
    regexFlags = re.VERBOSE

    def parseURI(self, uri):
        """Given a valid URI, return a dictionary of named groups in the regex match"""
        match = re.match(self.regex, uri, self.regexFlags)
        if not match:
            raise InvalidURIException("Invalid URI: %r" % uri)
        return match.groupdict()

    def assigned(self, uri, newRuleset):
        self.parseURI(uri)


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
                self.uriHandler.message(self.ruleset.uri, message, result)
        except:
            e = sys.exc_info()[1]
            log.msg("Exception occurred in RulesetDelivery for %r\n" +
                    "--- Original message\n%s\n--- Exception\n%s" %
                    (self.ruleset.uri,
                     unicode(message).encode('ascii', 'replace'),
                     "".join(traceback.format_exception(*sys.exc_info()))))


class UnsupportedURI(Exception):
    """Raised on failure in URIRegistry.query()"""
    pass


class URIRegistry(object):
    """A central authority for storing URIHandler instances and looking up
       one appropriate for a given URI.
       """
    def __init__(self, *handlers):
        self.handlers = list(handlers)

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

    def rulesetsRefreshed(self):
        """Notify all URI handlers that rulesets are done being refreshed"""
        for handler in self.handlers:
            handler.rulesetsRefreshed()


class RulesetStorage:
    """Abstract base class for a persistent list of Rulesets, stored in
       RulesetDelivery objects with URIHandlers looked up from the provided
       URIRegistry. The generated RulesetDelivery objects are automatically added
       to and removed from the supplied Message.Hub.

       At startup, rulesets are loaded from some persistent source and any
       modifications are recorded. Subclasses must implement this persistence.
       """
    def __init__(self, hub, uriRegistry):
        self.uriRegistry = uriRegistry
        self.hub = hub
        self._emptyStorage()
        self.refreshUntilDone()

    def refreshUntilDone(self, interval=5):
        """Refresh rulesets, retrying if an error occurs"""
        self.refresh().addErrback(self._reRefresh, interval)

    def _reRefresh(self, failure, interval):
        log.msg("Refresh failed, retrying in %r seconds (%s)" %
                (interval, failure.getBriefTraceback()))
        reactor.callLater(interval, self.refreshUntilDone)

    def refresh(self):
        """Begin the process of loading our rulesets in from the SQL database.
           Returns a Deferred that signals the operation's completion.
           """
        log.msg("Starting to refresh rulesets...")
        result = defer.Deferred()
        defer.maybeDeferred(self.dbIter).addCallback(
            self._refresh, result).addErrback(result.errback)
        return result

    def _refresh(self, seq, result):
        self._emptyStorage()
        count = 0
        for ruleset in seq:
            try:
                self._store(Ruleset(ruleset))
                count += 1
            except:
                log.msg("Failed to load ruleset %r:\n%s" % (
                    ruleset,
                    "".join(traceback.format_exception(*sys.exc_info())),
                    ))

        log.msg("%d rulesets loaded" % count)
        self.uriRegistry.rulesetsRefreshed()
        result.callback(None)
        return seq

    def _emptyStorage(self):
        # Remove any existing rulesets from the Message.Hub
        if hasattr(self, 'rulesetMap'):
            for value in self.rulesetMap.itervalues():
                self.hub.delClient(value)

        # self.rulesetMap maps URIs to RulesetDelivery instances
        self.rulesetMap = {}

    def store(self, rulesetXml):
        """Find a URIHandler for the given ruleset and add it to
           our mapping and to the hub. 'ruleset' is given as a DOM tree.

           Storing an empty ruleset for a particular URI is equivalent
           to removing that URI's ruleset.

           It is important that this function doesn't actually
           remove or change the ruleset in quesion unless any possible
           input errors have already been detected.

           This updates both our in-memory mapping of compiled rulesets,
           and the table of XML rulesets in our persistent storage.

           Returns a deferred, the callback of which is executed
           when the SQL database for this ruleset has been updated.
           The in-memory database will be updated immediately.
           """
        ruleset = Ruleset(rulesetXml)
        self._store(ruleset)
        self.dbStore(ruleset)

    def _store(self, ruleset):
        """Internal version of store() that doesn't update the database,
           and requires the ruleset to already be parsed.
           """
        # We need to find an appropriate URI handler whether our ruleset
        # is empty or not, since we have to be able to notify the handler.
        handler = self.uriRegistry.query(ruleset.uri)

        # Is this ruleset non-empty?
        if not ruleset.isEmpty():
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
            log.msg("Set ruleset for %r" % ruleset.uri)
        else:
            # Remove the ruleset completely if there was one
            if self.rulesetMap.has_key(ruleset.uri):
                self.hub.delClient(self.rulesetMap[ruleset.uri])
                del self.rulesetMap[ruleset.uri]

            log.msg("Removed ruleset for %r" % ruleset.uri)
            handler.unassigned(ruleset.uri)

    def flatten(self):
        """Return a flat list of all Ruleset objects so we can store 'em"""
        for delivery in self.rulesetMap.itervalues():
            yield delivery.ruleset

    def dbIter(self):
        """Returns (optionally via a Deferred) an iterable that
           contains a Ruleset object (or a string or DOM representing
           a ruleset) for every ruleset stored persistently.
           Implemented by subclasses.
           """
        return []

    def dbStore(self, ruleset):
        """Stores the given Ruleset object persistently in our
           database. If ruleset.isEmpty(), this should end up
           deleting the stored ruleset for the given URI if it exists.
           """
        pass


class DatabaseRulesetStorage(RulesetStorage):
    """A RulesetStorage that keeps rulesets in an SQL database"""
    def dbIter(self):
        """Begin the process of loading our rulesets in from the SQL database.
           Returns the list of rulesets via a deferred.
           """
        import Database
        result = defer.Deferred()
        d = Database.pool.runQuery("SELECT * FROM rulesets")
        d.addCallback(self._storeDbRulesets, result)
        d.addErrback(result.errback)
        return result

    def _storeDbRulesets(self, rulesets, result):
        """The second step in the refresh() process, loading the retrieved list of rulesets"""
        result.callback(self._iterRulesetValues(rulesets))
        return rulesets

    def _iterRulesetValues(self, rulesets):
        for uri, ruleset in rulesets:
            yield ruleset

    def dbStore(self, ruleset):
        """Store a ruleset persistently in our SQL database"""
        import Database

        # Delete the old ruleset, if there was one
        result = defer.Deferred()
        d = Database.pool.runOperation("DELETE FROM rulesets WHERE uri = %s" % Database.quote(ruleset.uri, 'text'))

        # If we need to insert a new ruleset, do that after the delete finishes
        if ruleset.isEmpty():
            d.addCallback(result.callback)
        else:
            d.addCallback(self._insertRuleset, result, ruleset)
        d.addErrback(result.errback)
        return result

    def _insertRuleset(self, none, result, ruleset):
        """Callback used by store() to insert a new or modified ruleset into the SQL database"""
        import Database
        d = Database.pool.runOperation("INSERT INTO rulesets (uri, xml) values(%s, %s)" % (
            Database.quote(ruleset.uri, 'text'), Database.quote(XML.toString(ruleset.xml), 'text')))
        d.addCallback(result.callback)
        d.addErrback(result.errback)


class FileRulesetStorage(RulesetStorage):
    """A simple RulesetStorage that uses a simple flat text file"""
    def __init__(self, hub, uriRegistry, path):
        self.path = path
        RulesetStorage.__init__(self, hub, uriRegistry)

    def dbIter(self):
        if os.path.isfile(self.path):
            dom = XML.parseStream(open(self.path))
            for element in XML.getChildElements(dom.documentElement):
                if element.nodeName == "ruleset":
                    yield element
        else:
            log.msg("The file %r does not exist, loading no rulesets" % self.path)

    def dbStore(self, ruleset=None):
        """Write all rulesets to disk in one big XML file"""
        doc = XML.parseString("<rulesets>\n"
                              "<!--\n"
                              "This is a ruleset storage for CIA. It tells the CIA server\n"
                              "how to deliver messages. Don't edit it by hand while the server\n"
                              "is running, use tools/ruleset_editor.py\n"
                              "-->\n"
                              "\n"
                              "</rulesets>")
        root = doc.documentElement
        for ruleset in self.flatten():
            root.appendChild(XML.Domlette.ConvertDocument(ruleset.xml).documentElement)
            root.appendChild(doc.createTextNode("\n\n"))

        f = open(self.path, "w")
        XML.Domlette.Print(doc, f)
        f.write("\n")


### The End ###
