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

from twisted.web import xmlrpc
from twisted.python import log
import Ruleset, Message, Rack, TimeUtil
import string, os, time


class StatsURIHandler(Ruleset.RegexURIHandler):
    """Handles stats:// URIs. The message passed to a stats:// URI is
       URI-encoded and added to the end of the stats:// URI to form
       a path identifying a class of messages that stats are collected for.
       """
    scheme = 'stats'
    regex = r"^stats://(?P<path>([a-zA-Z0-9_-]+(/[a-zA-Z0-9_-]+)*)?)$"

    def __init__(self, storage):
        self.storage = storage
        Ruleset.RegexURIHandler.__init__(self)

    def message(self, uri, message, content):
        """Appends 'content' to the path represented by the given URI
           and delivers a message to its associated stats target.
           """
        target = self.storage.getPathTarget(self.parseURI(uri)['path'], content)
        target.deliver(message)


class StatsInterface(xmlrpc.XMLRPC):
    """An XML-RPC interface used to query stats"""
    def __init__(self, caps, storage):
        xmlrpc.XMLRPC.__init__(self)
        self.caps = caps
        self.storage = storage

    def xmlrpc_catalog(self, path=''):
        """Return a list of subdirectories within this stats path.
           Defaults to the root of the stats:// namespace if 'path'
           isn't specified.
           """
        return self.storage.getPathTarget(path).catalog()

    def xmlrpc_getLatestMessages(self, path, limit=None):
        """Return 'limit' latest messages delivered to this stats target,
           or all available recent messages if 'limit' isn't specified.
           """
        return self.storage.getPathTarget(path).recentMessages.getLatest(limit)

    def xmlrpc_getCounterValues(self, path, name):
        """Returns a dictionary with current values for the given counter.
           Note that times are returned as UNIX-style seconds since
           the epoch in UTC.
           """
        return dict(self.storage.getPathTarget(path).counters.getCounter(name))

    def xmlrpc_getMetadata(self, path):
        """Return a dictionary holding all metadata for a particular stats path"""
        return dict(self.storage.getPathTarget(path).metadata)

    def xmlrpc_updateMetadata(self, path, d, key):
        """Merge the given dictionary into a path's metadata.
           This requires one of the usual capability keys for everything, this
           module, or this function, but it can also be accessed using a capability
           key that unlocks only a particular path.
           """
        self.caps.faultIfMissing(key, 'universe', 'stats', 'stats.metadata',
                                 ('stats.path', path))
        self.storage.getPathTarget(path).metadata.update(d)
        log.msg("Updating metadata for stats path %r\n%r" % (path, d))
        return True

    def xmlrpc_setMetadata(self, path, d, key):
        """Replace a path's metadata with the given dictionary"""
        self.caps.faultIfMissing(key, 'universe', 'stats', 'stats.metadata',
                                 ('stats.path', path))
        metadata = self.storage.getPathTarget(path).metadata
        metadata.clear()
        metadata.update(d)
        log.msg("Replacing metadata for stats path %r\n%r" % (path, d))
        return True


class StatsStorage(object):
    """A filesystem-like system for storing stats, based on Rack.
       Every path corresponds with a StatsTarget and a Rack namespace.
       """
    def __init__(self, fileName):
        self.rack = Rack.open(fileName)

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
        self.recentMessages = Rack.RingBuffer(self.rack.unlistedChild(('recentMessages',)), 500)
        self.metadata = self.rack.child(('metadata',))

    def deliver(self, message):
        """A message has been received which should be logged by this stats target"""
        self.counters.increment()
        self.recentMessages.push(str(message))

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


def _test():
    import doctest, Stats
    return doctest.testmod(Stats)

if __name__ == "__main__":
    _test()

### The End ###
