""" LibCIA.Stats.Target

Implements the core objects for representing and
interacting with stats targets.
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

import os, string
from twisted.internet import defer
from twisted.python import log
from LibCIA import Ruleset, Database, TimeUtil, Files
import time, posixpath, sys, cPickle, random
from LibCIA.Stats.Metadata import Metadata
from LibCIA.Stats.Messages import MessageBuffer


class StatsTarget(object):
    """Encapsulates all the stats-logging features used for one particular
       target. This can be one project, one class of messages, etc.
       Every StatsTarget is identified by a UNIX-style pathname.
       The root stats target's path is the empty string.

       This object doesn't store any of the actual data, it's just a way to
       access the persistent data stored in our global SQL database.
       """
    def __init__(self, path=''):
        self.setPath(path)
        self._messages = None
        self._counters = None
        self._metadata = None

    def _getMessages(self):
        if self._messages is None:
            self._messages = MessageBuffer(self.getDiskPath())
        return self._messages
    messages = property(_getMessages)

    def _getCounters(self):
        if self._counters is None:
            self._counters = Counters(self)
        return self._counters
    counters = property(_getCounters)

    def _getMetadata(self):
        if self._metadata is None:
            self._metadata = Metadata(self)
        return self._metadata
    metadata = property(_getMetadata)

    def setPath(self, path):
        # Remove leading and trailing slashes, remove duplicate
        # slashes, process '.' and '..' directories. We don't
        # allow paths beginning with '.' or '_'.
        self.pathSegments = []
        for segment in path.split('/'):
            if segment == '..':
                if self.pathSegments:
                    del self.pathSegments[-1]
            elif segment in ('.', ''):
                pass
            elif segment[0] in ('.', '_'):
                raise ValueError("Stats path segment %r begins with a reserved character"
                                 % segment)
            else:
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

    def getDiskPath(self):
        """Every target gets a directory on disk. This returns it, without any
           guarantee that it exists yet.
           """
        return Files.tryGetDir(Files.dbDir, 'stats', *map(string.lower, self.pathSegments))

    def deliver(self, message=None):
        """An event has occurred which should be logged by this stats target"""
        if message:
            self.messages.push(unicode(message).encode('utf-8'))
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

    def getMTime(self):
        """Get the modification time of this stats target, in seconds since the epoch.
           Currently this ignores metadata changes and reports the lastEventTime of the
           'forever' event counter. The result will be delivered via a Deferred.
           """
        result = defer.Deferred()
        self.counters.getCounter('forever').addCallback(
            self._getMTime, result
            ).addErrback(result.errback)
        return result

    def _getMTime(self, counter, result):
        if counter:
            result.callback(counter['lastEventTime'])
        else:
            result.callback(None)

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


class SubscriptionDelivery:
    """The object responsible for actually notifiying entities that
       have subscribed to a stats target.
       """
    def __init__(self, target):
        self.target = target

    def notify(self, scope):
        """Notify all subscribers to this stats target of a change in 'scope'"""
        # Get a list of applicable triggers from the database
        Database.pool.runQuery("SELECT id, `trigger` FROM stats_subscriptions "
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


class Counters:
    """A set of counters which are used together to track how many
       events occur and how frequently in each of several time intervals.
       """
    def __init__(self, target):
        self.target = target
        self.cache = None

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
        # Invalidate the cache for this counter if we have one
        self.cache = None

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
        if self.cache is not None:
            result = defer.Deferred()
            result.callback(self.cache.get(name))
            return result
        else:
            return Database.pool.runInteraction(self._getCounter, name)

    def dict(self):
        """Return a Deferred that eventually results in a dictionary mapping
           counter name to the dictionary that would be returned by getCounter.
           """
        if self.cache is not None:
            # This is the cache itself
            result = defer.Deferred()
            result.callback(self.cache)
            return result
        else:
            return Database.pool.runInteraction(self._dict)

        return Database.pool.runInteraction(self._dict)

    def _updateCache(self, cursor):
        """Database interaction to update our counter cache"""
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
        self.cache = results

    def _getCounter(self, cursor, name):
        """Database interaction implementing getCounter"""
        self._updateCache(cursor)
        return self.cache.get(name)

    def _dict(self, cursor):
        """Database interaction implementing _getCounterDict"""
        self._updateCache(cursor)
        return self.cache

    def clear(self):
        """Delete all counters for this target. Returns a Deferred"""
        self.cache = {}
        return Database.pool.runOperation("DELETE FROM stats_counters WHERE target_path = %s" %
                                          Database.quote(self.target.path, 'varchar'))


class Maintenance:
    """This class performs periodic maintenance of the stats database, including
       counter rollover and removing old messages.
       """
    def __init__(self):
        self.targetQueue = []

    def run(self):
        """Performs one stats maintenance cycle, returning a Deferred that
           yields None when the maintenance is complete.
           """
        return Database.pool.runInteraction(self._run)

    def _run(self, cursor):
        """Database interaction implementing the maintenance cycle"""
        self.checkRollovers(cursor)
        self.pruneSubscriptions(cursor)

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

    def pruneSubscriptions(self, cursor, maxFailures=3):
        """Delete subscriptions that have expired"""
        cursor.execute("DELETE FROM stats_subscriptions WHERE expiration < %s" %
                       Database.quote(int(time.time()), 'bigint'))

### The End ###
