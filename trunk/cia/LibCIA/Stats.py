""" LibCIA.Stats

Defines the stats:// URI for rulesets to target. The URI is of
the form stats://[optional/path/prefix]
The message passed to the URI from a ruleset is then URI encoded
and used as the rest of the stats:// path. This makes it easy to
create multiple namespaces for which stats are collected, and generate
the actual stats target using part of the message.
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
import Ruleset, Message, TimeUtil, RPC
import string, os, time, types, posixpath


class StatsURIHandler(Ruleset.RegexURIHandler):
    """Handles stats:// URIs. The message passed to a stats:// URI is
       URI-encoded and added to the end of the stats:// URI to form
       a path identifying a class of messages that stats are collected for.
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
        return StatsTarget(path).catalog()

    def xmlrpc_getLatestMessages(self, path, limit=None):
        """Return 'limit' latest messages delivered to this stats target,
           or all available recent messages if 'limit' isn't specified.
           """
        return StatsTarget(path).getLatestMessages(limit)

    def xmlrpc_getCounterValues(self, path, name):
        """Returns a dictionary with current values for the given counter.
           Note that times are returned as UNIX-style seconds since
           the epoch in UTC.
           """
        try:
            return dict(self.storage.getPathTarget(path).counters.getCounter(name))
        except:
            Debug.catchFault()

    def xmlrpc_clearTarget(self, path, key):
        """Deletes any data stored at the stats target with the given path.
           This is not recursive.
           """
        self.caps.faultIfMissing(key, 'universe', 'stats', 'stats.clearTarget', ('stats.path', path))
        try:
            self.storage.getPathTarget(path).clear()
            self.storage.sync()
            log.msg("Clearing stats path %r" % path)
            return True
        except:
            Debug.catchFault()


class MetadataInterface(xmlrpc.XMLRPC):
    """An XML-RPC interface for querying and modifying stats metadata"""
    def __init__(self, caps, storage):
        xmlrpc.XMLRPC.__init__(self)
        self.caps = caps
        self.storage = storage

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


class StatsStorage(object):
    """A filesystem-like system for storing stats, based on Rack.
       Every path corresponds with a StatsTarget and a Rack namespace.
       """
    def __init__(self, fileName):
        self.rack = Rack.open(fileName)

    def sync(self):
        """Write out any pending changes to disk"""
        self.rack.sync()

    def getTarget(self, pathSegments):
        """Return a StatsTarget representing the stats stored at the given path,
           represented as one or more nested directories.
           """
        return StatsTarget(self, pathSegments)

    def getPathTarget(self, path, *extraSegments):
        """Like getTarget, but split up 'path' into segments and optionally append extraSegments first"""
        # Strip empty strings, so that "/" and "" give us (), and paths like "a/////b" turn into ('a', 'b')
        segments = path.split("/") + list(extraSegments)
        return self.getTarget([segment for segment in segments if segment])

    def getRoot(self):
        return StatsTarget(self)


class MetadataRack:
    """A Rack subclass that only stores strings, but allows retrieval
       and specification of meta-metadata such as a value's MIME type.
       """
    def __setitem__(self, key, value):
        """Wrap the usual __setitem__ to enforce that keys must be strings"""
        if type(key) not in types.StringTypes:
            raise TypeError("MetadataRack's keys must be strings")
        Rack.Rack.__setitem__(self, key, value)

    def setType(self, key, mimeType):
        """Set a key's MIME type. None can be used to restore the default."""
        # This stores the type at (key, 'type'), which can't normally be reached
        # from outside because our __setitem__ only allows strings.
        if mimeType is None:
            try:
                del self[(key, 'type')]
            except KeyError:
                pass
        else:
            Rack.Rack.__setitem__(self, (key, 'type'), mimeType)

    def getType(self, key, default="text/plain"):
        """Get a key's MIME type, returning 'default' if it hasn't been set"""
        return self.get((key, 'type'), default)

    def __iter__(self):
        """Return only the string items, so when iterating the metadata our
           caller doesn't see the internal keys we use to store MIME types.
           """
        for item in Rack.Rack.__iter__(self):
            if type(item) in types.StringTypes:
                yield item

    def __delitem__(self, key):
        # After deleting an item, unset its MIME type
        Rack.Rack.__delitem__(self, key)
        self.setType(key, None)


class StatsTarget(object):
    """Encapsulates all the stats-logging features used for one particular
       target. This can be one project, one class of messages, etc.
       It is constructed around a Rack namespace that stores the various
       resources this stats target owns.

       Each path segment corresponds to one namespace in the Rack. String
       rack namespaces are always path segments, other data types like
       tuples are used for particular stats resources.
       """
    def __init__(self, storage, pathSegments=()):
        self.storage = storage
        self.pathSegments = pathSegments
        self.rack = storage.rack.child(*map(str, pathSegments))

        # Child namespaces that are tuples rather than strings are used
        # for our actual resource storage.
        self.counters = Counters(self.rack.child(('counters',)))
        self.recentMessages = Rack.RingBuffer(self.rack.child(('recentMessages',), cls=Rack.UnlistedRack), 500)
        self.metadata = self.rack.child(('metadata',), cls=MetadataRack)

    def deliver(self, message):
        """A message has been received which should be logged by this stats target"""
        self.counters.increment()
        self.recentMessages.push(str(message))
        self.storage.sync()

    def catalog(self):
        """Return a list of subdirectories of this stats target"""
        # This is just a list of strings from the rack's subnamespace catalog
        return [item for item in self.rack.catalog() if type(item) == str]

    def child(self, name):
        """Return the StatsTarget for the given sub-target name under this one"""
        return StatsTarget(self.storage, tuple(self.pathSegments) + (str(name),))

    def parent(self):
        """Return the parent StatsTarget of this one, or None if we're the root"""
        if self.pathSegments:
            return StatsTarget(self.storage, self.pathSegments[:-1])

    def getTitle(self):
        """Return the human-readable title of this stats target-
           the 'title' metadata key if we have one, otherwise the last
           section of our path. The root stats target has the special
           default title 'Stats', since it has no last path segment.
           """
        title = self.metadata.get('title')
        if title:
            return title
        if self.pathSegments:
            return self.pathSegments[-1]
        return 'Stats'

    def clear(self):
        """Effectively delete this stats target- delete all of its counters,
           clear its recent messages, clear all of its metadata.
           """
        self.counters.clear()
        self.recentMessages.clear()
        self.metadata.clear()


class Counters(object):
    """A set of counters which are used together to track events
       occurring over several TimeUtil.Intervals. Stored in a Rack.
       """
    def __init__(self, rack):
        self.rack = rack
        self.checkRollovers()

    def getCounter(self, name):
        """Return the Rack associated with a counter. The Rack
           is a subclass of the dictionary type, and can
           include the following keys:

           firstEventTime : The time, in UTC seconds since the epoch, when the first event occurred
           lastEventTime  : The time when the most recent event occurred
           eventCount     : The number of events that have occurred
           """
        return self.rack.child(name)

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
