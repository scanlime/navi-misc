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
from twisted.enterprise.util import quote as quoteSQL
from twisted.internet import defer
import Ruleset, Message, TimeUtil, RPC, Database
import string, os, time, posixpath, sys


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


class StatsInterface(RPC.Interface):
    """An XML-RPC interface used to query stats"""
    def __init__(self):
        RPC.Interface.__init__(self)
        self.putSubHandler('metadata', MetadataInterface())

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


class MetadataInterface(RPC.Interface):
    """An XML-RPC interface for querying and modifying stats metadata"""
    def xmlrpc_get(self, path, name, default=False):
        """Get a (value, type) tuple for the metadata key with the given
           name, returning 'default' if it isn't found
           """
        return StatsTarget(path).metadata.get(name, default)

    def xmlrpc_dict(self, path):
        """Return a mapping of names to (value, type) tuples for the given path"""
        return StatsTarget(path).metadata.dict()

    def protected_set(self, path, name, value, mimeType='text/plain'):
        """Set a metadata key's value and MIME type"""
        return StatsTarget(path).metadata.set(name, value, mimeType)


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
           """
        title = self.metadata.get('title')
        if title:
            return title
        if self.name:
            return self.name
        return 'Stats'

    def clear(self):
        """Delete everything associated with this stats target. Returns a Deferred
           indicating the completion of this operation.
           """
        # Delete the item in stats_target- the other tables will be
        # deleted due to cascading foreign keys
        return Database.pool.runOperation("DELETE FROM stats_catalog WHERE target_path = %s" %
                                          quoteSQL(self.path, 'varchar'))

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
                                        (quoteSQL(parent.path, 'varchar'),
                                         quoteSQL(self.path, 'varchar')))
        else:
            # This is the root node. We still need to insert a parent to keep the
            # table consistent, but our parent in this case is NULL.
            cursor.execute("INSERT IGNORE INTO stats_catalog (target_path) VALUES(%s)" %
                           quoteSQL(self.path, 'varchar'))

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
                            quoteSQL(self.path, 'varchar'))
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
            timestamp = quoteSQL(str(message.xml.timestamp), 'bigint')
        else:
            timestamp = NULL

        self.target._autoCreateTargetFor(cursor, cursor.execute,
                                         "INSERT INTO stats_messages (target_path, xml, timestamp)"
                                         " VALUES(%s, %s, %s)" %
                                         (quoteSQL(self.target.path, 'varchar'),
                                          quoteSQL(message, 'text'),
                                          timestamp))

    def getLatest(self, limit=None):
        """Return the most recent messages as XML strings, optionally up to a provided
           maximum value. The messages are returned in reverse chronological order.
           """
        return Database.pool.runInteraction(self._getLatest, limit)

    def _getLatest(self, cursor, limit):
        if limit is None:
            limitClause = ''
        else:
            limitClause = " LIMIT %d" % limit
        cursor.execute("SELECT xml FROM stats_messages WHERE target_path = %s ORDER BY id DESC%s" %
                            (quoteSQL(self.target.path, 'varchar'),
                             limitClause))
        results = []
        while True:
            row = cursor.fetchone()
            if row is None:
                break
            results.append(row[0])
        return results


class Metadata:
    """An abstraction for the metadata that may be stored for any stats target.
       Metadata objects consist of a name, a MIME type, and a value. The value
       can be any binary or text object stored as a string. Metadata keys
       are stored base64-encoded in a LONGBLOB, as it's probably faster than
       string quoting.
       """
    def __init__(self, target):
        self.target = target

    def get(self, name, default=None):
        """Return a Deferred that results in the (value, type) tuple for the the
           given metadata key, or 'default' if a result can't be found.
           """
        return Database.pool.runInteraction(self._get, name, default)

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
                                          quoteSQL(self.target.path, 'varchar'))

    def remove(self, name):
        """Remove one metadata key, with the given name"""
        return Database.pool.runOperation("DELETE FROM stats_metadata WHERE target_path = %s AND name = %s" %
                                          (quoteSQL(self.target.path, 'varchar'),
                                           quoteSQL(name, 'varchar')))

    def _get(self, cursor, name, default):
        """Database interaction to return the value and type for a particular key"""
        cursor.execute("SELECT value, mime_type FROM stats_metadata WHERE target_path = %s AND name = %s" %
                       (quoteSQL(self.target.path, 'varchar'),
                        quoteSQL(name, 'varchar')))
        return cursor.fetchone() or default

    def _dict(self, cursor):
        """Database interaction to return to implement dict()"""
        cursor.execute("SELECT name, value, mime_type FROM stats_metadata WHERE target_path = %s" %
                       quoteSQL(self.target.path, 'varchar'))
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
        # Just to make sure our row exists...
        cursor.execute("INSERT IGNORE INTO stats_metadata (target_path, name) VALUES(%s, %s)" %
                       (quoteSQL(self.target.path, 'varchar'),
                        quoteSQL(name, 'varchar')))

        # Quote the value using the '_mysql' module directly, since twisted's quote() doesn't
        # understand the way MySQL requires BLOBs to be quoted.
        import _mysql
        qValue = _mysql.escape_string(str(value))

        # Now actually set the value
        cursor.execute("UPDATE stats_metadata SET mime_type = %s, value = '%s' WHERE target_path = %s AND name = %s" %
                       (quoteSQL(mimeType, 'varchar'),
                        qValue,
                        quoteSQL(self.target.path, 'varchar'),
                        quoteSQL(name, 'varchar')))

    def _keys(self, cursor):
        """Database interaction implementing keys()"""
        cursor.execute("SELECT DISTINCT name FROM stats_metadata WHERE target_path = %s" %
                       (quoteSQL(self.target.path, 'varchar')))
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
                           (quoteSQL(self.target.path, 'varchar'),
                            quoteSQL(name, 'varchar')))
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
                       (quoteSQL(self.target.path, 'varchar'),
                        quoteSQL(name, 'varchar'),
                        quoteSQL(now, 'bigint')))

        # Increment the counter and update its timestamp
        cursor.execute("UPDATE stats_counters SET "
                       "event_count = event_count + 1,"
                       "last_time = %s "
                       "WHERE target_path = %s AND name = %s" %
                       (quoteSQL(now, 'bigint'),
                        quoteSQL(self.target.path, 'varchar'),
                        quoteSQL(name, 'varchar')))

    def getCounter(self, name):
        """Return a Deferred that eventually results in a dictionary,
           including the following keys:

           firstEventTime : The time, in UTC seconds since the epoch, when the first event occurred
           lastEventTime  : The time when the most recent event occurred
           eventCount     : The number of events that have occurred
           """
        return Database.pool.runInteraction(self._getCounter, name)

    def _getCounter(self, cursor, name):
        """Database interaction implementing _getCounter"""
        cursor.execute("SELECT first_time, last_time, event_count FROM stats_counters WHERE"
                       " target_path = %s AND name = %s" %
                       (quoteSQL(self.target.path, 'varchar'),
                        quoteSQL(name, 'varchar')))
        row = cursor.fetchone()
        return {
            'firstEventTime': row[0],
            'lastEventTime':  row[1],
            'eventCount':     row[2],
            }

    def clear(self):
        """Delete all counters for this target. Returns a Deferred"""
        return Database.pool.runOperation("DELETE FROM stats_counters WHERE target_path = %s" %
                                          quoteSQL(self.target.path, 'varchar'))

###### Rollovers are still broke

#     def checkOneRollover(self, previous, current):
#         """Check for rollovers in one pair of consecutive time intervals,
#            like yesterday/today or lastMonth/thisMonth.
#            """
#         p = self.getCounter(previous)
#         c = self.getCounter(current)
#         pTime = p.get('firstEventTime', None)
#         cTime = c.get('firstEventTime', None)

#         if cTime is not None:
#             if not cTime in TimeUtil.Interval(current):
#                 # Our current timer is old, copy it to the previous timer and delete it
#                 p.clear()
#                 p.update(c)
#                 c.clear()

#         if pTime is not None:
#             if not pTime in TimeUtil.Interval(previous):
#                 # The previous timer is old. Either the current timer rolled over first
#                 # and it was really old, or no events occurred in the first timer
#                 # (cTime is None) but some had in the previous timer and it's old.
#                 p.clear()

#     def checkRollovers(self):
#         """If any of the counters were created outside the interval
#            they apply to, transfer its value to another counter
#            if we need to and reset it.
#            """
#         self.checkOneRollover('yesterday', 'today')
#         self.checkOneRollover('lastWeek', 'thisWeek')
#         self.checkOneRollover('lastMonth', 'thisMonth')


### The End ###
