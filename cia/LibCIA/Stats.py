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
import string, os, time, types, posixpath, sys


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
        return StatsTarget(path).getCounterValues(name)

    def protected_clearTarget(self, path):
        """Deletes any data stored at a given stats target or in any of its subtargets"""
        log.msg("Clearing stats path %r" % path)
        return StatsTarget(path).clear()

    def caps_clearTarget(self, rpcPath, statsPath):
        """In addition to the usual capabilities, allow ('stats.path', path)"""
        return self.makeDefaultCaps(rpcPath) + [('stats.path', statsPath)]


class MetadataInterface(RPC.Interface):
    """An XML-RPC interface for querying and modifying stats metadata"""
    def getKeyValue(self, metadata, key):
        """Return the value of a particular metadata key, enclosing it
           in a Binary object if its mime type isn't in text/*.
           """
        if metadata.getType(key).startswith("text/"):
            return metadata[key]
        else:
            return xmlrpc.Binary(metadata[key])

    def xmlrpc_getKeyList(self, path):
        """Return a list of metadata keys for a particular stats path"""
        try:
            return self.storage.getPathTarget(path).metadata.keys()
        except:
            Debug.catchFault()

    def xmlrpc_getKeyValues(self, path, keys=None):
        """Return a mapping from key to value. If a key list is specified,
           only keys in that list are returned- otherwise all available keys
           are returned.
           """
        try:
            metadata = self.storage.getPathTarget(path).metadata
            results = {}
            for key in metadata:
                if keys is None or key in keys:
                    results[key] = self.getKeyValue(metadata, key)
            return results
        except:
            Debug.catchFault()

    def xmlrpc_getKeyTypes(self, path, keys=None):
        """Return a mapping from key to MIME type. If a key list is specified,
           only keys in that list are returned- otherwise all available keys
           are returned.
           """
        try:
            metadata = self.storage.getPathTarget(path).metadata
            results = {}
            for key in metadata:
                if keys is None or key in keys:
                    results[key] = metadata.getType(key)
            return results
        except:
            Debug.catchFault()

    def xmlrpc_getKeys(self, path, keys=None):
        """Return a mapping from key to a mapping of all information available
           for that key. The mapping includes 'value' and 'type' currently.
           If a key list is specified, only keys in that list are returned-
           otherwise all available keys are returned.
           """
        try:
            metadata = self.storage.getPathTarget(path).metadata
            results = {}
            for key in metadata:
                if keys is None or key in keys:
                    results[key] = {
                        'type': metadata.getType(key),
                        'value': self.getKeyValue(metadata, key),
                        }
            return results
        except:
            Debug.catchFault()

    def xmlrpc_setKeyValues(self, path, keyMap, key):
        """For each key/value pair in the given map, set the corresponding
           key in the given path's metadata. Requires a capability key
           for this module or for the particular path in question.
           """
        self.caps.faultIfMissing(key, 'universe', 'stats', 'stats.metadata',
                                 ('stats.path', path))
        try:
            self.storage.getPathTarget(path).metadata.update(keyMap)
            self.storage.sync()
            log.msg("Updating metadata values for stats path %r\n%r" % (path, keyMap))
            return True
        except:
            Debug.catchFault()

    def xmlrpc_setKeyTypes(self, path, keyMap, key):
        """For each key/value pair in the given map, set the corresponding
           MIME type in the given path's metadata. Requires a capability key
           for this module or for the particular path in question.
           """
        self.caps.faultIfMissing(key, 'universe', 'stats', 'stats.metadata',
                                 ('stats.path', path))
        try:
            metadata = self.storage.getPathTarget(path).metadata
            for mdKey, mdType in keyMap.iteritems():
                metadata.setType(mdKey, mdType)
            self.storage.sync()
            log.msg("Updating metadata types for stats path %r\n%r" % (path, keyMap))
            return True
        except:
            Debug.catchFault()

    def xmlrpc_delKeys(self, path, metadataKeys, key):
        """Delete zero or more metadata keys from the given list.
           Requires a capability key for this module or for the particular
           path in question.
           """
        self.caps.faultIfMissing(key, 'universe', 'stats', 'stats.metadata',
                                 ('stats.path', path))
        try:
            metadata = self.storage.getPathTarget(path).metadata
            for mdKey in metadataKeys:
                del metadata[mdKey]
                log.msg("Deleted metadata key %r for stats path %r" % (mdKey, path))
            self.storage.sync()
            return True
        except:
            Debug.catchFault()


class StatsPathException(Exception):
    pass


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
            raise StatsPathException("Stats paths are currently limited to 128 characters")

        # Our name is the last path segment, or None if we're the root
        if self.pathSegments:
            self.name = self.pathSegments[-1]
        else:
            self.name = None

    def deliver(self, message=None):
        """An event has occurred which should be logged by this stats target"""
        if message:
            self.messages.push(message)

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
        try:
            if parent:
                # If we have a parent, we have to worry about creating it
                # if it doesn't exist and generating the proper parent path.
                parent._autoCreateTargetFor(cursor, cursor.execute,
                                            "INSERT INTO stats_catalog (parent_path, target_path) VALUES(%s, %s)" %
                                            (quoteSQL(parent.path, 'varchar'),
                                             quoteSQL(self.path, 'varchar')))
            else:
                # This is the root node. We still need to insert a parent to keep the
                # table consistent, but our parent in this case is NULL.
                cursor.execute("INSERT INTO stats_catalog (target_path) VALUES(%s)" %
                                    quoteSQL(self.path, 'varchar'))
        except:
            # Ignore duplicate key errors
            if str(sys.exc_info()[1]).find("duplicate key") < 0:
                raise

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
       can be any binary or text object stored as a string.
       """
    def __init__(self, target):
        self.target = target

    def get(self, name, default=None):
        return default

    def __contains__(self, name):
        return False


class Counters:
    """A set of counters which are used together to track events
       occurring over several TimeUtil.Intervals. Stored in a Rack.
       """
    def __init__(self, target):
        self.target = target

    def getCounter(self, name):
        """Return the Rack associated with a counter. The Rack
           is a subclass of the dictionary type, and can
           include the following keys:

           firstEventTime : The time, in UTC seconds since the epoch, when the first event occurred
           lastEventTime  : The time when the most recent event occurred
           eventCount     : The number of events that have occurred
           """
        return {}
        #return self.rack.child(name)

    def incrementCounter(self, name, evTime=None):
        """Increment the counter with the given name.
           If evTime is provided, that is used as the
           time at which this event was received. Otherwise,
           the current time is used.
           """
        c = self.getCounter(name)
        if evTime is None:
            evTime = int(time.time())

            c.setdefault('firstEventTime', evTime)
        c['lastEventTime'] = evTime
        c['eventCount'] = c.setdefault('eventCount', 0) + 1

    def checkOneRollover(self, previous, current):
        """Check for rollovers in one pair of consecutive time intervals,
           like yesterday/today or lastMonth/thisMonth.
           """
        p = self.getCounter(previous)
        c = self.getCounter(current)
        pTime = p.get('firstEventTime', None)
        cTime = c.get('firstEventTime', None)

        if cTime is not None:
            if not cTime in TimeUtil.Interval(current):
                # Our current timer is old, copy it to the previous timer and delete it
                p.clear()
                p.update(c)
                c.clear()

        if pTime is not None:
            if not pTime in TimeUtil.Interval(previous):
                # The previous timer is old. Either the current timer rolled over first
                # and it was really old, or no events occurred in the first timer
                # (cTime is None) but some had in the previous timer and it's old.
                p.clear()

    def checkRollovers(self):
        """If any of the counters were created outside the interval
           they apply to, transfer its value to another counter
           if we need to and reset it.
           """
        self.checkOneRollover('yesterday', 'today')
        self.checkOneRollover('lastWeek', 'thisWeek')
        self.checkOneRollover('lastMonth', 'thisMonth')

    def increment(self):
        """Increments all applicable counters in this list
           and saves them, after checking for rollovers.
           """
        self.checkRollovers()
        self.incrementCounter('today')
        self.incrementCounter('thisWeek')
        self.incrementCounter('thisMonth')
        self.incrementCounter('forever')

    def clear(self):
        """Delete all counters"""
        self.rack.clear()
        for child in self.rack.catalog():
            self.rack.child(child).clear()


def _test():
    import doctest, Stats
    return doctest.testmod(Stats)

if __name__ == "__main__":
    _test()

### The End ###
