""" LibCIA.Stats

Defines the stats:// URI for rulesets to target. The URI is of
the form stats://[optional/path/prefix]
The message passed to the URI from a ruleset is then used as the
est of the stats:// path. This makes it easy to create multiple
namespaces for which stats are collected, and generate the actual
stats target using part of the message.
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
from twisted.internet import defer, reactor
from urlparse import urlparse
import Ruleset, Message, TimeUtil, RpcServer, Database
import string, os, time, posixpath, sys, cPickle


class StatsURIHandler(Ruleset.RegexURIHandler):
    """Handles stats:// URIs. A stats target is chosen,
       and the message is delivered to it.
       """
    scheme = 'stats'
    regex = r"^stats://(?P<path>([a-zA-Z0-9_-]+(/[a-zA-Z0-9_-]+)*)?)$"

    def message(self, uri, message, content):
        """Appends 'content' to the path represented by the given URI
           and delivers a message to its associated stats target.
           """
        path = posixpath.join(self.parseURI(uri)['path'], content)
        StatsTarget(path).deliver(message)


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
        return StatsTarget(path).messages.getLatest(limit)

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
        Database.pool.runQuery("SELECT trigger FROM stats_subscriptions "
                               "WHERE target_path = %s "
                               "AND (scope is NULL or scope = %s)" %
                               (Database.quote(self.path, 'varchar'),
                                Database.quote(scope, 'varchar'))).addCallback(self.runTriggers)

    def runTriggers(self, triggers):
        """After retrieving a list of applicable triggers, this calls them"""
        for trigger in triggers:
            f, args, kwargs = cPickle.loads(trigger[0])
            defer.maybeDeferred(f, *args, **kwargs).addCallback(
                self.triggerSuccess).addErrback(self.triggerFailure)


class StatsTarget:
    """Encapsulates all the stats-logging features used for one particular
       target. This can be one project, one class of messages, etc.
       Every StatsTarget is identified by a UNIX-style pathname.
       The root stats target's path is the empty string.

       This object doesn't store any of the actual data, it's just a way to
       access the persistent data stored in our global SQL database.
       """
    def __init__(self, path=''):
        self.setPath(path)
        self.messages = Messages(self)
        self.counters = Counters(self)
        self.metadata = Metadata(self)

    def setPath(self, path):
        # Remove leading and trailing slashes, remove duplicate
        # slashes, process '.' and '..' directories.
        self.pathSegments = []
        for segment in path.split('/'):
            if segment == '..':
                if self.pathSegments:
                    del self.pathSegments[-1]
            elif segment and segment != '.':
                self.pathSegments.append(segment)
        self.path = '/'.join(self.pathSegments)

        # Our database uses VARCHAR(128), make sure this fits
        if len(self.path) > 128:
            raise Ruleset.InvalidURIException("Stats paths are currently limited to 128 characters")

        # Our name is the last path segment, or None if we're the root
        if self.pathSegments:
            self.name = self.pathSegments[-1]
        else:
            self.name = None

    def deliver(self, message=None):
        """An event has occurred which should be logged by this stats target"""
        if message:
            self.messages.push(message)
        self.counters.increment()
        SubscriptionDelivery(self).notify('messages')

    def child(self, name):
        """Return the StatsTarget for the given sub-target name under this one"""
        return StatsTarget(posixpath.join(self.path, name))

    def parent(self):
        """Return the parent StatsTarget of this one, or None if we're the root"""
        if self.path:
            return self.child('..')

    def catalog(self):
        """Return a list of StatsTargets instances representing all children of this target"""
        return Database.pool.runInteraction(self._catalog)

    def getTitle(self):
        """Return the human-readable title of this stats target. In
           decreasing order of preference, this is:
             - our 'title' metadata key
             - self.name, the last segment of our path
             - 'Stats'
           The result will always be a Deferred.
           """
        result = defer.Deferred()
        self.metadata.getValue('title').addCallback(self._getTitle, result).addErrback(result.errback)
        return result

    def _getTitle(self, metadataTitle, result):
        if metadataTitle is not None:
            result.callback(metadataTitle)
        elif self.name:
            result.callback(self.name)
        else:
            result.callback('Stats')

    def clear(self):
        """Delete everything associated with this stats target. Returns a Deferred
           indicating the completion of this operation.
           """
        # Delete the item in stats_target- the other tables will be
        # deleted due to cascading foreign keys
        return Database.pool.runOperation("DELETE FROM stats_catalog WHERE target_path = %s" %
                                          Database.quote(self.path, 'varchar'))

    def __repr__(self):
        return "<StatsTarget at %r>" % self.path

    def _create(self, cursor):
        """Internal function to create a new stats target, meant to be run from
           inside a database interaction. This is actually a recursive operation
           that tries to create parent stats targets if necessary.

           NOTE: this -must- ignore duplicate keys to avoid a race condition in which
                 one thread, in _autoCreateTargetFor, decides to create a new target
                 but before that target is fully created another thread also decides
                 it needs a new target.
           """
        parent = self.parent()
        if parent:
            # If we have a parent, we have to worry about creating it
            # if it doesn't exist and generating the proper parent path.
            parent._autoCreateTargetFor(cursor, cursor.execute,
                                        "INSERT IGNORE INTO stats_catalog (parent_path, target_path) VALUES(%s, %s)" %
                                        (Database.quote(parent.path, 'varchar'),
                                         Database.quote(self.path, 'varchar')))
        else:
            # This is the root node. We still need to insert a parent to keep the
            # table consistent, but our parent in this case is NULL.
            cursor.execute("INSERT IGNORE INTO stats_catalog (target_path) VALUES(%s)" %
                           Database.quote(self.path, 'varchar'))

    def _autoCreateTargetFor(self, cursor, func, *args, **kwargs):
        """Run the given function. If an exception occurs that looks like a violated
           foreign key constraint, add our path to the database and try
           again (without attempting to catch any exceptions).
           This is fast way to create stats targets that don't exist without
           a noticeable performance penalty when executing operations on
           existing stats targets.

           NOTE: This is meant to be run inside a database interaction, hence
                 a cursor is required. This cursor will be used to
                 create the new stats target if one is required.
           """
        try:
            func(*args, **kwargs)
        except:
            # Cheesy way to detect foreign key errors without being too DBMS-specific
            if str(sys.exc_info()[1]).find("foreign key") >= 0:
                self._create(cursor)
                func(*args, **kwargs)
            else:
                raise

    def _catalog(self, cursor):
        """Database interaction representing the internals of catalog()"""
        cursor.execute("SELECT target_path FROM stats_catalog WHERE parent_path = %s" %
                            Database.quote(self.path, 'varchar'))
        results = []
        while True:
            row = cursor.fetchone()
            if row is None:
                break
            results.append(StatsTarget(row[0]))
        return results


class Messages(object):
    """Represents the set of stored messages associated with one stats target"""
    def __init__(self, target):
        self.target = target

    def push(self, message):
        """Store a new message for this stats target"""
        # This must be done inside a database interaction, since we may need
        # to create the target's catalog entry if it doesn't exist.
        return Database.pool.runInteraction(self._push, message)

    def _push(self, cursor, message):
        # Does this message have a timestamp?
        if message.xml.timestamp:
            timestamp = Database.quote(str(message.xml.timestamp), 'bigint')
        else:
            # Our message really should have had a timestamp.. don't
            # store it, because without a timestamp it will be immortal.
            return

        self.target._autoCreateTargetFor(cursor, cursor.execute,
                                         "INSERT INTO stats_messages (target_path, xml, timestamp)"
                                         " VALUES(%s, %s, %s)" %
                                         (Database.quote(self.target.path, 'varchar'),
                                          Database.quote(message, 'text'),
                                          timestamp))

    def getLatest(self, limit=None):
        """Return the most recent messages as (id, xml) tuples, optionally up to a provided
           maximum value. The messages are returned in reverse chronological order.
           """
        return Database.pool.runInteraction(self._getLatest, limit)

    def _getLatest(self, cursor, limit):
        if limit is None:
            limitClause = ''
        else:
            limitClause = " LIMIT %d" % limit
        cursor.execute("SELECT id, xml FROM stats_messages WHERE target_path = %s ORDER BY id DESC%s" %
                            (Database.quote(self.target.path, 'varchar'),
                             limitClause))
        return cursor.fetchall()

    def getMessageById(self, id):
        """Return a single message from this stats target's archive, by ID.
           Returns None if the message isn't in our database.
           """
        return Database.pool.runInteraction(self._getMessageById, id)

    def _getMessageById(self, cursor, id):
        cursor.execute("SELECT xml FROM stats_messages WHERE target_path = %s AND id = %s" %
                       (Database.quote(self.target.path, 'varchar'),
                        Database.quote(id, 'bigint')))
        row = cursor.fetchone()
        if row:
            return row[0]


class Metadata:
    """An abstraction for the metadata that may be stored for any stats target.
       Metadata objects consist of a name, a MIME type, and a value. The value
       can be any binary or text object stored as a string.
       """
    def __init__(self, target):
        self.target = target

    def get(self, name, default=None):
        """Return a Deferred that results in the (value, type) tuple for the the
           given metadata key, or 'default' if a result can't be found.
           """
        return Database.pool.runInteraction(self._get, name, default)

    def getValue(self, name, default=None, typePrefix='text/'):
        """Like get(), but only returns the value. Ensures the MIME type
           begins with typePrefix. If it doesn't, a TypeError is raised.
           """
        return Database.pool.runInteraction(self._getValue, name, default, typePrefix)

    def set(self, name, value, mimeType='text/plain'):
        """Set a metadata key, creating it if it doesn't exist"""
        return Database.pool.runInteraction(self._set, name, value, mimeType)

    def keys(self):
        """Return (via a Deferred) a list of all valid metadata key names"""
        return Database.pool.runInteraction(self._keys)

    def dict(self):
        """Return (via a Deferred) a mapping from names to (value, type) tuples"""
        return Database.pool.runInteraction(self._dict)

    def clear(self):
        """Delete all metadata for this target. Returns a Deferred"""
        return Database.pool.runOperation("DELETE FROM stats_metadata WHERE target_path = %s" %
                                          Database.quote(self.target.path, 'varchar'))

    def remove(self, name):
        """Remove one metadata key, with the given name"""
        return Database.pool.runOperation("DELETE FROM stats_metadata WHERE target_path = %s AND name = %s" %
                                          (Database.quote(self.target.path, 'varchar'),
                                           Database.quote(name, 'varchar')))

    def has_key(self, name):
        """Returs True (via a Deferred) if the given key name exists for this target"""
        return Database.pool.runInteraction(self._has_key, name)

    def _has_key(self, cursor, name):
        """Database interaction implemented has_key()"""
        cursor.execute("SELECT COUNT(*) FROM stats_metadata WHERE target_path = %s AND name = %s" %
                       (Database.quote(self.target.path, 'varchar'),
                        Database.quote(name, 'varchar')))
        return bool(cursor.fetchone()[0])

    def _get(self, cursor, name, default):
        """Database interaction to return the value and type for a particular key"""
        cursor.execute("SELECT value, mime_type FROM stats_metadata WHERE target_path = %s AND name = %s" %
                       (Database.quote(self.target.path, 'varchar'),
                        Database.quote(name, 'varchar')))
        return cursor.fetchone() or default

    def _getValue(self, cursor, name, default, typePrefix):
        result = self._get(cursor, name, None)
        if result is None:
            return default
        value, mimeType = result
        if not mimeType.startswith(typePrefix):
            raise TypeError("A metadata key of type %s was found where %s* was expected" %
                            (mimeType, typePrefix))
        return value

    def _dict(self, cursor):
        """Database interaction to return to implement dict()"""
        cursor.execute("SELECT name, value, mime_type FROM stats_metadata WHERE target_path = %s" %
                       Database.quote(self.target.path, 'varchar'))
        results = {}
        while True:
            row = cursor.fetchone()
            if row is None:
                break
            results[row[0]] = (row[1], row[2])
        return results

    def _set(self, cursor, name, value, mimeType):
        """Database interaction implementing set(). This first runs a dummy
           'insert ignore' to ensure that the row exists in our table, then
           runs an update to change its value.
           """
        # Make sure our row exists. This is wrapped in an autoCreateTargetFor
        # so that if the stats target doesn't exist, it is also automatically created.
        self.target._autoCreateTargetFor(cursor, cursor.execute,
                                         "INSERT IGNORE INTO stats_metadata (target_path, name) VALUES(%s, %s)" %
                                         (Database.quote(self.target.path, 'varchar'),
                                          Database.quote(name, 'varchar')))

        # Now actually set the value
        cursor.execute("UPDATE stats_metadata SET mime_type = %s, mtime = %s, value = '%s' "
                       "WHERE target_path = %s AND name = %s" %
                       (Database.quote(mimeType, 'varchar'),
                        Database.quote(time.time(), 'bigint'),
                        Database.quoteBlob(value),
                        Database.quote(self.target.path, 'varchar'),
                        Database.quote(name, 'varchar')))

    def _keys(self, cursor):
        """Database interaction implementing keys()"""
        cursor.execute("SELECT DISTINCT name FROM stats_metadata WHERE target_path = %s" %
                       (Database.quote(self.target.path, 'varchar')))
        results = []
        while True:
            row = cursor.fetchone()
            if row is None:
                break
            results.append(row[0])
        return results


class Counters:
    """A set of counters which are used together to track events
       occurring over several TimeUtil.Intervals. Stored in a Rack.
       """
    def __init__(self, target):
        self.target = target

    def increment(self):
        """Increment all applicable counters, signaling the arrival of a new event"""
        # Automatically create the stats target if it doesn't exist
        return Database.pool.runInteraction(self._incrementWrapper)

    def _incrementWrapper(self, cursor):
        """Database interaction implementing increment(). Ensures
           the stats target exists while calling _increment().
           """
        self.target._autoCreateTargetFor(cursor, self._increment, cursor)

    def _createCounter(self, cursor, name):
        """Internal function to create one blank counter if it doesn't exist."""
        try:
            cursor.execute("INSERT INTO stats_counters (target_path, name) VALUES(%s, %s)" %
                           (Database.quote(self.target.path, 'varchar'),
                            Database.quote(name, 'varchar')))
        except:
            # Ignore duplicate key errors
            if str(sys.exc_info()[1]).find("duplicate key") < 0:
                raise

    def _increment(self, cursor):
        """Internal function, run within a database interaction, that ensures
           all required counters exist then updates them all.
           """
        self._incrementCounter(cursor, 'forever')
        self._incrementCounter(cursor, 'today')
        self._incrementCounter(cursor, 'thisWeek')
        self._incrementCounter(cursor, 'thisMonth')

    def _incrementCounter(self, cursor, name):
        """Increment one counter, creating it if necessary"""
        now = int(time.time())

        # Insert a default value, which will be ignored if the counter already exists
        cursor.execute("INSERT IGNORE INTO stats_counters (target_path, name, first_time) VALUES(%s, %s, %s)" %
                       (Database.quote(self.target.path, 'varchar'),
                        Database.quote(name, 'varchar'),
                        Database.quote(now, 'bigint')))

        # Increment the counter and update its timestamp
        cursor.execute("UPDATE stats_counters SET "
                       "event_count = event_count + 1,"
                       "last_time = %s "
                       "WHERE target_path = %s AND name = %s" %
                       (Database.quote(now, 'bigint'),
                        Database.quote(self.target.path, 'varchar'),
                        Database.quote(name, 'varchar')))

    def getCounter(self, name):
        """Return a Deferred that eventually results in a dictionary,
           including the following keys:

           firstEventTime : The time, in UTC seconds since the epoch, when the first event occurred
           lastEventTime  : The time when the most recent event occurred
           eventCount     : The number of events that have occurred
           """
        return Database.pool.runInteraction(self._getCounter, name)

    def dict(self):
        """Return a Deferred that eventually results in a dictionary mapping
           counter name to the dictionary that would be returned by getCounter.
           """
        return Database.pool.runInteraction(self._dict)

    def _getCounter(self, cursor, name):
        """Database interaction implementing getCounter"""
        cursor.execute("SELECT first_time, last_time, event_count FROM stats_counters WHERE"
                       " target_path = %s AND name = %s" %
                       (Database.quote(self.target.path, 'varchar'),
                        Database.quote(name, 'varchar')))
        row = cursor.fetchone()
        if row is not None:
            return {
                'firstEventTime': row[0],
                'lastEventTime':  row[1],
                'eventCount':     row[2],
                }

    def _dict(self, cursor):
        """Database interaction implementing _getCounterDict"""
        cursor.execute("SELECT name, first_time, last_time, event_count FROM stats_counters WHERE"
                       " target_path = %s" %
                       Database.quote(self.target.path, 'varchar'))
        results = {}
        while True:
            row = cursor.fetchone()
            if row is None:
                break
            results[row[0]] = {
                'firstEventTime': row[1],
                'lastEventTime':  row[2],
                'eventCount':     row[3],
                }
        return results

    def clear(self):
        """Delete all counters for this target. Returns a Deferred"""
        return Database.pool.runOperation("DELETE FROM stats_counters WHERE target_path = %s" %
                                          Database.quote(self.target.path, 'varchar'))


class Maintenance:
    """This class performs periodic maintenance of the stats database, including
       counter rollover and removing old messages.
       """
    # Maximum number of messages to keep around for each stats target
    maxTargetMessages = 200

    def run(self):
        """Performs one stats maintenance cycle, returning a Deferred that
           yields None when the maintenance is complete.
           """
        return Database.pool.runInteraction(self._run)

    def _run(self, cursor):
        """Database interaction implementing the maintenance cycle"""
        self.checkRollovers(cursor)
        self.pruneSubscriptions(cursor)
        self.pruneTargets(cursor)

    def checkOneRollover(self, cursor, previous, current):
        """Check for rollovers in one pair of consecutive time intervals,
           like yesterday/today or lastMonth/thisMonth. This is meant to
           be run inside a database interaction.
           """
        # Delete counters that are too old to bother keeping at all
        cursor.execute("DELETE FROM stats_counters "
                       "WHERE (name = %s OR name = %s) "
                       "AND first_time < %s" %
                       (Database.quote(previous, 'varchar'),
                        Database.quote(current, 'varchar'),
                        Database.quote(long(TimeUtil.Interval(previous).getFirstTimestamp()), 'bigint')))

        # Roll over remaining counters that are too old for current
        # but still within the range of previous. Note that there is a
        # race condition in which this update will fail because a timer
        # has been incremented between it and the above DELETE. It could
        # be prevented by locking the table, but it's probably not worth
        # it. If the rollover fails this time, it will get another chance.
        cursor.execute("UPDATE stats_counters SET name = %s "
                       "WHERE name = %s "
                       "AND first_time < %s" %
                       (Database.quote(previous, 'varchar'),
                        Database.quote(current, 'varchar'),
                        Database.quote(long(TimeUtil.Interval(current).getFirstTimestamp()), 'bigint')))

    def checkRollovers(self, cursor):
        """Check all applicable counters for rollovers. This should
           be executed inside a database interaction.
           """
        self.checkOneRollover(cursor, 'yesterday', 'today')
        self.checkOneRollover(cursor, 'lastWeek', 'thisWeek')
        self.checkOneRollover(cursor, 'lastMonth', 'thisMonth')

    def pruneTargets(self, cursor):
        """Find all stats targets, running pruneTarget on each"""
        cursor.execute("SELECT target_path FROM stats_catalog")
        for row in cursor.fetchall():
            self.pruneTarget(cursor, row[0])
        cursor.execute("OPTIMIZE TABLE stats_messages")

    def pruneTarget(self, cursor, path):
        """Delete messages that are too old, from one particular stats target"""
        # Find the ID of the oldest message we want to keep
        cursor.execute("SELECT id FROM stats_messages WHERE target_path = %s ORDER BY id DESC LIMIT 1 OFFSET %d" %
                       (Database.quote(path, 'varchar'),
                        self.maxTargetMessages))
        row = cursor.fetchone()
        if row:
            id = row[0]
            cursor.execute("DELETE FROM stats_messages WHERE target_path = %s AND id < %d" %
                           (Database.quote(path, 'varchar'), id))

    def pruneSubscriptions(self, cursor):
        """Delete subscriptions that have expired"""
        now = int(time.time())
        cursor.execute("DELETE FROM stats_subscriptions WHERE expiration < %d" % now)

### The End ###
