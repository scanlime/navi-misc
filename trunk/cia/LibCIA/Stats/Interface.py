""" LibCIA.Stats.Handler

The stats:// URI handler, the root of the stats XML-RPC interface,
and the maintenance system. All the usual entry points for the stats system.
"""
#
# CIA open source notification system
# Copyright (C) 2003-2005 Micah Dowty <micah@navi.cx>
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
from twisted.internet import defer
from twisted.web import xmlrpc
from LibCIA import RpcServer, Database, XML
from Target import StatsTarget
import cPickle, urlparse, time


class StatsInterface(RpcServer.Interface):
    """An XML-RPC interface used to query stats"""
    def __init__(self):
        RpcServer.Interface.__init__(self)
        self.putSubHandler('metadata', MetadataInterface())
        self.putSubHandler('subscribe', SubscriptionInterface())

    def xmlrpc_catalog(self, path=''):
        """Return a list of subdirectories within this stats path"""
        result = defer.Deferred()
        d = StatsTarget(path).catalog()
        d.addErrback(result.errback)
        d.addCallback(self._catalog, result)
        return result

    def _catalog(self, items, result):
        """Convert the returned catalog into target names and return them via
           the provided Deferred instance.
           """
        result.callback([target.name for target in items])

    def xmlrpc_getLatestMessages(self, path, limit=None):
        """Return 'limit' latest messages delivered to this stats target,
           or all available recent messages if 'limit' isn't specified.
           """
        return [(id, XML.toString(doc)) for id, doc in
                StatsTarget(path).messages.getLatest(limit)]

    def xmlrpc_getCounterValues(self, path, name):
        """Returns a dictionary with current values for the given counter.
           Note that times are returned as UNIX-style seconds since
           the epoch in UTC.
           """
        return StatsTarget(path).counters.getCounter(name)

    def protected_clearTarget(self, path):
        """Deletes any data stored at a given stats target or in any of its subtargets"""
        log.msg("Clearing stats path %r" % path)
        return StatsTarget(path).clear()

    def caps_clearTarget(self, rpcPath, statsPath):
        """In addition to the usual capabilities, allow ('stats.path', path)"""
        return self.makeDefaultCaps(rpcPath) + [('stats.path', statsPath)]


class MetadataInterface(RpcServer.Interface):
    """An XML-RPC interface for querying and modifying stats metadata"""
    def wrapTuple(self, t):
        """Wrap the value in a (value, type) tuple in an xmlrpc.Binary
           if the type doesn't start with text/.
           """
        if t and not t[1].startswith("text/"):
            return (xmlrpc.Binary(t[0]), t[1])
        else:
            return t

    def xmlrpc_get(self, path, name, default=False):
        """Get a (value, type) tuple for the metadata key with the given
           name, returning 'default' if it isn't found
           """
        result = defer.Deferred()
        StatsTarget(path).metadata.get(name, default).addCallback(
            self._get, result).addErrback(result.errback)
        return result

    def _get(self, t, result):
        """Backend for get() that ensures the results are serializable"""
        result.callback(self.wrapTuple(t))

    def xmlrpc_dict(self, path):
        """Return a mapping of names to (value, type) tuples for the given path"""
        result = defer.Deferred()
        StatsTarget(path).metadata.dict().addCallback(
            self._dict, result).addErrback(result.errback)
        return result

    def _dict(self, original, result):
        """Backend for dict() that ensures the results are serializable"""
        d = {}
        for name, t in original.iteritems():
            d[name] = self.wrapTuple(t)
        result.callback(d)

    def caps_set(self, rpcPath, statsPath, name, value, mimeType=None):
        """In addition to the usual capabilities, allow ('stats.path', path),
           ('stats.metadata.path', path), and ('stats.metadata.key', path, name)
           for setting metadata.
           """
        return self.makeDefaultCaps(rpcPath) + [
            ('stats.path', statsPath),
            ('stats.metadata.path', statsPath),
            ('stats.metadata.key', statsPath, name),
            ]

    def caps_clear(self, rpcPath, statsPath):
        return self.makeDefaultCaps(rpcPath) + [
            ('stats.path', statsPath),
            ('stats.metadata.path', statsPath),
            ]

    def caps_remove(self, rpcPath, statsPath, name):
        return self.makeDefaultCaps(rpcPath) + [
            ('stats.path', statsPath),
            ('stats.metadata.path', statsPath),
            ('stats.metadata.key', statsPath, name),
            ]

    def protected_set(self, path, name, value, mimeType='text/plain'):
        """Set a metadata key's value and MIME type"""
        return StatsTarget(path).metadata.set(name, str(value), mimeType)

    def protected_clear(self, path):
        """Remove all metadata for one target"""
        return StatsTarget(path).metadata.clear()

    def protected_remove(self, path, name):
        """Remove one metadata key for this target, if it exists"""
        return StatsTarget(path).metadata.remove(name)


class SubscriptionInterface(RpcServer.Interface):
    """An XML-RPC interface for subscribing to be notified when changes
       occur to a stats target. This provides multiple ways of subscribing,
       for compatibility with multiple existing standards.
       """
    def _unsubscribe(self, cursor, client):
        """Remove all subscriptions for the given client.
           This is intended to be run inside a database interaction.
           """
        cursor.execute("DELETE FROM stats_subscriptions WHERE client = %s" %
                       Database.quote(client, 'varchar'))

    def makeTrigger(self, triggerFunc, *triggerArgs, **triggerKwargs):
        """Given a trigger function and the arguments to pass it,
           returns a pickled representation of the trigger.
           """
        return cPickle.dumps((triggerFunc, triggerArgs, triggerKwargs))

    def _subscribe(self, cursor, target, client, trigger, scope=None, ttl=25*60*60):
        """A database interaction for adding subscriptions.
           'target' must be the StatsTarget object this subscription is for.
           'client' is the IP address of the client requesting this subscription.
           'trigger' is a trigger pickle, as returned by makeTrigger
           'scope' refers to a part of the stats target this refers to. By default, all of it.
           'ttl' is the time to live for this subscription, 25 hours by default.
           """
        cursor.execute("INSERT INTO stats_subscriptions "
                       "(target_path, expiration, scope, client, trigger) "
                       "VALUES (%s, %s, %s, %s, '%s')" %
                       (Database.quote(target.path, 'varchar'),
                        Database.quote(int(time.time() + ttl), 'bigint'),
                        Database.quote(scope, 'varchar'),
                        Database.quote(client, 'varchar'),
                        Database.quoteBlob(trigger)))

    def xmlrpc_rss2(self, procedureName, clientPort, responderPath, protocol, urls, request=None):
        """This is the flavor of subscription required for the RSS 2.0 <cloud>
           tag. The client IP should be determined from this request. 'urls'
           is a list of URLs the client is interested in monitoring- we have
           to convert those into stats targets.
           """
        log.msg("Received an RSS 2.0 subscription request for %r reporting via %r at %r on port %r to %r" %
                (urls, protocol, responderPath, clientPort, procedureName))

        # Reject protocols we can't handle
        if protocol != 'xml-rpc':
            log.msg("Rejecting request: unsupported protocol")
            return False

        # Poing off into a database interaction for most of this...
        return Database.pool.runInteraction(self._rss2, procedureName, clientPort,
                                            responderPath, urls, request.getClientIP())

    def _rss2(self, cursor, procedureName, clientPort, responderPath, urls, clientIP):
        """The database interaction implementing RSS 2.0 subscriptions"""
        # Unsubscribe all of this client's previous requests
        self._unsubscribe(cursor, clientIP)

        # Generate a new subscription for each of its new URLs
        for url in urls:
            # Figure out the associated stats target for this URL
            target = self.getTargetFromURL(url)
            if not target:
                log.msg("Ignoring URL %r which doesn't appear to be a stats target" % url)
                continue

            # Make a trigger that notifies the client as requested.
            xmlrpcUrl = "http://%s:%s%s" % (clientIP, clientPort, responderPath)
            trigger = self.makeTrigger(xmlrpc.Proxy(xmlrpcUrl).callRemote,
                                       procedureName, url)

            # Finally jam the trigger in our database
            self._subscribe(cursor, target, clientIP, trigger)

        # According to the RSS 2.0 spec, return True to indicate success
        return True

    def getTargetFromURL(self, url):
        """Given a URL that presumably refers to our HTTP server, extract a stats
           target from it. The URL should be to an RSS feed, but we won't be too
           picky. If we can't find a related stats target, return None.
           """
        # We ignore everything except the path, assuming the URL actually
        # refers to this host. There's no reason for someone to ask us to
        # notify them if another server's page changes, and it would take
        # more effort than it's worth to accurately determine if the given
        # host refers to this CIA server, with all the proxying and DNS
        # multiplicity that could be going on.
        path = urlparse.urlparse(url)[2]

        # FIXME: really cheesy hack!
        if not path.startswith("/stats/"):
            return None
        return StatsTarget(path[7:].split("/.",1)[0])

### The End ###
