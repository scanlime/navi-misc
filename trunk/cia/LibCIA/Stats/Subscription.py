""" LibCIA.Stats.Subscription

Implements a generic publish/subscribe system for receiving notification
about changes in stats targets. On top of this generic mechanism, interfaces
compatible with various existing standards are constructed.
"""
#
# CIA open source notification system
# Copyright (C) 2003-2004 Micah Dowty <micahjd@users.sourceforge.net>
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
from twisted.web import xmlrpc
from twisted.internet import defer
from urlparse import urlparse
from LibCIA import RpcServer, Database
import time, cPickle


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
        path = urlparse(url)[2]

        # FIXME: really cheesy hack!
        if not path.startswith("/stats/"):
            return None
        return StatsTarget(path[7:].split("/.",1)[0])


class SubscriptionDelivery:
    """The object responsible for actually notifiying entities that
       have subscribed to a stats target.
       """
    def __init__(self, target):
        self.target = target

    def notify(self, scope):
        """Notify all subscribers to this stats target of a change in 'scope'"""
        # Get a list of applicable triggers from the database
        Database.pool.runQuery("SELECT id, trigger FROM stats_subscriptions "
                               "WHERE target_path = %s "
                               "AND (scope is NULL or scope = %s)" %
                               (Database.quote(self.target.path, 'varchar'),
                                Database.quote(scope, 'varchar'))).addCallback(self.runTriggers)

    def runTriggers(self, rows):
        """After retrieving a list of applicable triggers, this calls them.
           'rows' should be a sequence of (id, trigger) tuples.
           """
        if rows:
            log.msg("Notifying %d subscribers for %r" % (len(rows), self.target))
        for id, trigger in rows:
            f, args, kwargs = cPickle.loads(trigger)
            defer.maybeDeferred(f, *args, **kwargs).addCallback(
                self.triggerSuccess, id).addErrback(
                self.triggerFailure, id)

    def triggerSuccess(self, result, id):
        """Record a successful trigger run for the given subscription id"""
        # Zero the consecutive failure count
        Database.pool.runOperation("UPDATE stats_subscriptions SET failures = 0 WHERE id = %s" %
                                   Database.quote(id, 'bigint'))

    def triggerFailure(self, failure, id):
        """Record an unsuccessful trigger run for the given subscription id"""
        Database.pool.runInteraction(self._triggerFailure, failure, id)

    def _triggerFailure(self, cursor, failure, id, maxFailures=3):
        # Increment the consecutive failure count
        log.msg("Failed to notify subscriber %d for %r: %r" % (id, self.target, failure))
        cursor.execute("UPDATE stats_subscriptions SET failures = failures + 1 WHERE id = %s" %
                       Database.quote(id, 'bigint'))

        # Cancel the subscription if we've had too many failures
        cursor.execute("DELETE FROM stats_subscriptions WHERE id = %s AND failures > %s" %
                       (Database.quote(id, 'bigint'),
                        Database.quote(maxFailures, 'int')))
        if cursor.rowcount:
            log.msg("Unsubscribing subscriber %d for %r, more than %d consecutive failures" %
                    (id, self.target, maxFailures))

### The End ###
