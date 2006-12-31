""" LibCIA.IRC.Handler

The irc:// URI handler, acting as a frontend to our network of bots
"""
#
# CIA open source notification system
# Copyright (C) 2003-2006 Micah Dowty <micah@navi.cx>
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

from twisted.spread import pb
from twisted.internet import defer, reactor
from twisted.python import log
from LibCIA import Ruleset


class IrcURIHandler(Ruleset.RegexURIHandler):
    """Handles irc:// URIs in rulesets. This creates a message queue
       for each URI, and delivers formatted messages to the proper
       message queue instance.

       This follows a subset of the internet draft at:

          http://www.w3.org/Addressing/draft-mirashi-url-irc-01.txt

       In short, the following sorts of URIs are valid:

          irc://irc.foo.net/boing
            Refers to the channel #boing at irc.foo.net on the default port

          irc://irc.foo.net:1234/boing
            Refers to the channel #boing at irc.foo.net, on port 1234

          irc://irc.foo.net/muffin,isnick
            Refers to a users with the nick 'muffin' on irc.foo.net's default port

       The full hostname may also be replaced with a network name, as defined
       in Network.py. This implies a random choice from multiple servers, and possibly
       other network-specific behaviour.

       This is starting to be updated to a newer irc:// draft that recommends
       including the #, but still supports the above URIs.
       """
    scheme = 'irc'
    regex = r"""
       ^irc://(?P<host>(  ([a-zA-Z]([a-zA-Z0-9.-]*[a-zA-Z0-9])?) |
                          ([0-9]+(\.[0-9]+){3})
                       ))
       (:(?P<port>[0-9]+))?/(?P<target>[^\s,]+)(?P<isnick>,isnick)?$
       """

    def __init__(self, remoteBots):
        # A map from URI to message queue
        self.queueMap = {}
        self.remoteBots = remoteBots
        Ruleset.RegexURIHandler.__init__(self)

    def createQueueFromURI(self, uri):
        """Convert a URI to a new message queue instance"""
        d = self.parseURI(uri)

        if d['isnick']:
            # This refers to a nickname- deliver private messages to that user
            return PrivateMessageQueue(self.remoteBots, d['host'], d['port'], d['target'])
        else:
            # It's a channel. Add the # if necesary.
            channel = d['target']
            if channel[0] != '#':
                channel = '#' + channel
            return ChannelMessageQueue(self.remoteBots, d['host'], d['port'], channel)

    def assigned(self, uri, newRuleset):
        # If this URI is new, create a new message queue for it
        if not uri in self.queueMap:
            q = self.createQueueFromURI(uri)
            self.queueMap[uri] = q

    def unassigned(self, uri):
        self.queueMap[uri].cancel()
        del self.queueMap[uri]

    def message(self, uri, message, content):
        self.queueMap[uri].send(unicode(content).encode('utf-8'))

    def rulesetsRefreshed(self):
        """Synchronize our requests with the server's only once rulesets have
           refreshed, so we don't inadvertently delete a request that hasn't
           finished loading yet.
           """
        self.remoteBots.allowSync = True
        self.remoteBots.syncRequests()


class RequestIdentity:
    """An object that can represent the identity of a Request remotely"""
    __slots__ = ["host", "port", "channel"]
    def __init__(self, host, port, channel):
        self.__dict__.update(dict(
            host = host,
            port = port,
            channel = channel,
            ))

    def __hash__(self):
        return hash((self.host, self.port, self.channel))

    def __cmp__(self, other):
        return cmp((self.host, self.port, self.channel),
                   (other.host, other.port, other.channel))

    def __setattr__(self, name, value):
        raise TypeError("RequestIdentity instances are immuatble")


class ReconnectingPBClient(pb.PBClientFactory):
    """A PBClientFactory that automatically tries to reconnect
       using a supplied method if the connection fails.
       """
    def __init__(self, reconnector, delay=4):
        self.reconnector = reconnector
        self.delay = delay
        pb.PBClientFactory.__init__(self)

    def clientConnectionFailed(self, connector, reason):
        reactor.callLater(self.delay, self.reconnector)

    def clientConnectionLost(self, connector, reason):
        reactor.callLater(self.delay, self.reconnector)


class RemoteBots:
    """Represents a bot server running elsewhere, that we connect to
       using Perspective Broker. This caches references to currently
       registered requests, and ensures that the remote request list
       matches the local one even when the bot server restarts.
       """
    def __init__(self, socketName):
        self.socketName = socketName
        self.botNet = None
        self.factory = ReconnectingPBClient(self.connect)
        self.allowSync = False

        # Maps RequestIdentity instances to remote requests. Requests
        # that should exist but don't yet are mapped to None.
        self.requestMap = {}

        self.connect()

    def connect(self):
        log.msg("Trying to connect to bots on socket %r..." % self.socketName)
        self.botNet = None
        reactor.connectUNIX(self.socketName, self.factory)
        if not self.factory.rootObjectRequests:
            self.factory.getRootObject().addCallback(
                self._gotRootObject)

    def _gotRootObject(self, root):
        log.msg("Connected to bot server")
        self.botNet = root

        if self.allowSync:
            # Sync on connect only if we can be sure we've loaded all requests
            self.syncRequests()

    def findRequest(self, requestId):
        """Find a remote Request reference matching the given request ID,
           creating it if necessary. Returns the remote reference via
           a Deferred. Returns None if the request doesn't exist yet
           and can't be created (for example, if the bot server is down).
           Even if the bot server is unavailable, this request is added
           to our local list.
           """
        result = defer.Deferred()

        if self.requestMap.get(requestId) and self.botNet:
            # This request already exists
            result.callback(self.requestMap[requestId])
        else:
            # It doesn't exist. Mark that it should
            self.requestMap[requestId] = None
            if self.botNet:
                # If we have a connection to the bot net, try to add the request
                self.botNet.callRemote("findRequest", requestId.host, requestId.port,
                                       requestId.channel).addCallback(
                    self._findRequest, requestId, result).addErrback(
                    result.errback)
            else:
                # Nope, give up for now
                result.callback(None)

        return result

    def _findRequest(self, requestRef, requestId, result):
        self.requestMap[requestId] = requestRef
        result.callback(requestRef)

    def removeRequest(self, requestId):
        """Ask the bot server to remove a request, given a RequestIdentity.
           Returns, via a Deferred, True if the request could be removed
           or False if we can't currently contact the bot server. In either
           case, the request is removed from our local list.
           """
        result = defer.Deferred()

        if requestId in self.requestMap:
            if self.botNet:
                # We already have a reference to this request and our server is up.
                # Send the request a message to cancel itself.
                self.requestMap[requestId].callRemote("cancel").addCallback(
                    self._cancelledRequest, result).addErrback(
                    result.errback)
            else:
                result.callback(False)

            # Whether the server is up or not, remove it from our local list
            del self.requestMap[requestId]

        else:
            # We don't have a reference to it. See if it exists server-side
            if self.botNet:
                self.botNet.callRemote("findRequest", requestId.host, requestId.port,
                                       requestId.channel, False).addCallback(
                    self._removeFoundRequest, result).addErrback(
                    result.errback)
            else:
                result.callback(False)

        return result

    def _cancelledRequest(self, completed, result):
        # Our request was successfully cancelled
        result.callback(True)

    def _removeFoundRequest(self, ref, result):
        if ref:
            # There is indeed an instance of this request on the server. Cancel it.
            ref.callRemote("cancel").addCallback(
                self._cancelledRequest, result).addErrback(
                result.errback)
        else:
            # Nothing to do
            result.callback(True)

    def _removeRequest(self, requestRef, requestId, result):
        self.requestMap[requestId] = requestRef
        result.callback(requestRef)

    def syncRequests(self):
        """Flush our local references to remote request objects. If a server
           connection has been established, this retrieves new request references
           and adds and removes requests as necessary.
           """
        log.msg("Synchronizing bot requests")
        for id in self.requestMap.keys():
            self.requestMap[id] = None

        if self.botNet:
            # Get the server's request list
            self.botNet.callRemote("getRequests").addCallback(
                self._syncRequests)

    def _syncRequests(self, serverRequests):
        for (host, port, channel), ref in serverRequests.iteritems():
            id = RequestIdentity(host, port, channel)

            if id in self.requestMap:
                # Good, this is a request that we need and we have. Save the reference
                self.requestMap[id] = ref

            else:
                # This is a request that the server has but we don't need. Cancel it
                ref.callRemote("cancel")

        # Start trying to reestablish all remaining requests with no reference
        for id, ref in self.requestMap.iteritems():
            if not ref:
                self.findRequest(id)


class MessageQueue:
    """Abstract base class for a queue we can deliver IRC messages to"""
    def __init__(self, remoteBots, host, port, channel, target):
        self.target = target
        self.remoteBots = remoteBots
        self.requestId = RequestIdentity(host, port, channel)

        self.remoteBots.findRequest(self.requestId)

    def cancel(self):
        """Cancel the request associated with this message queue"""
        self.remoteBots.removeRequest(self.requestId)

    def send(self, message):
        """Split up a message into lines and queue it for transmission"""
        self.remoteBots.findRequest(self.requestId).addCallback(self._send, message)

    def _send(self, request, message):
        """Once we have a reference to the remote request, send our message"""
        # If the request doesn't exist yet on the server, ignore this.
        if request:
            request.callRemote("msgList", self.target, message.split('\n'))


class PrivateMessageQueue(MessageQueue):
    """Send private messages to a particular user, using one bot"""
    def __init__(self, remoteBots, host, port, nick):
        MessageQueue.__init__(self, remoteBots, host, port, None, nick)


class ChannelMessageQueue(MessageQueue):
    """Send messages to a channel, using multiple bots if necessary"""
    def __init__(self, remoteBots, host, port, channel):
        MessageQueue.__init__(self, remoteBots, host, port, channel, channel)

### The End ###
