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
import Ruleset, Message, Rack
import string, os
import time, datetime, calendar


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
        recentMessages = self.storage.getPathTarget(path).recentMessages
        if recentMessages:
            return recentMessages.getLatest(limit)
        else:
            return []

    def xmlrpc_getCounterValues(self, path, name):
        """Returns a dictionary with current values for the given counter,
           in the same format as would be returned by the counter's getValues()
           member. Note that times are returned as UNIX-style seconds since
           the epoch in UTC.
           """
        counters = self.storage.getPathTarget(path).counters
        if counters:
            counter = counters.getCounter(name)
            if counter:
                return counter.getValues()
        return {}

    def xmlrpc_getMetadata(self, path):
        """Return a dictionary holding all metadata for a particular stats path"""
        target = self.storage.getPathTarget(path)
        if target.metadata:
            return target.metadata.dict
        else:
            return {}

    def xmlrpc_updateMetadata(self, path, d, key):
        """Merge the given dictionary into a path's metadata.
           This requires one of the usual capability keys for everything, this
           module, or this function, but it can also be accessed using a capability
           key that unlocks only a particular path.
           """
        self.caps.faultIfMissing(key, 'universe', 'stats', 'stats.metadata',
                                 ('stats.path', path))
        target = self.storage.getPathTarget(path)
        target.metadata.update(d)
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
        return self.getTarget(list(path.split("/")) + list(extraSegments))


class StatsTarget(object):
    """Encapsulates all the stats-logging features used for one particular
       target. This can be one project, one class of messages, etc.
       It is constructed around a Rack namespace that stores the various
       resources this stats target owns.
       """
    def __init__(self, storage, pathSegments):
        self.storage = storage
        self.pathSegments = pathSegments
        self.rack = storage.rack.namespace(pathSegments)

        self.counters = Counters(self.rack.namespace('counters'))
        self.recentMessages = RingBuffer(self.rack.namespace('recentMessages'))
        self.metadata = self.rack.namespace('metadata')

    def deliver(self, message):
        """A message has been received which should be logged by this stats target"""
        self.counters.increment()
        self.recentMessages.push(str(message))

    def catalog(self):
        """Return a list of subdirectories of this stats target"""
        return 'not', 'here', 'yet', 'FIXME'

    def child(self, name):
        """Return the StatsTarget for the given sub-target name under this one"""
        return StatsTarget(self.storage, tuple(self.pathSegments) + (name,))


class TimeInterval(object):
    """Represents some interval of time, like 'yesterday' or 'this week'.
       Provides functions for returning the bounds of the
       interval and testing whether a particular time is within it.
       Represents time using datetime objects. By default, the interval
       is created relative to the current date and time in UTC. To override
       this, a datetime object can be passed to the constructor.

       TimeInterval is constructed with the name of the interval
       it should represent.

       >>> now = datetime.datetime(2003, 12, 19, 2, 19, 39, 50279)

       >>> TimeInterval('today', now)
       <TimeInterval from 2003-12-19 00:00:00 to 2003-12-20 00:00:00>

       >>> TimeInterval('yesterday', now)
       <TimeInterval from 2003-12-18 00:00:00 to 2003-12-19 00:00:00>

       >>> TimeInterval('thisWeek', now)
       <TimeInterval from 2003-12-15 00:00:00 to 2003-12-22 00:00:00>

       >>> TimeInterval('lastWeek', now)
       <TimeInterval from 2003-12-08 00:00:00 to 2003-12-15 00:00:00>

       >>> TimeInterval('thisMonth', now)
       <TimeInterval from 2003-12-01 00:00:00 to 2004-01-01 00:00:00>

       >>> TimeInterval('lastMonth', now)
       <TimeInterval from 2003-11-01 00:00:00 to 2003-12-01 00:00:00>
       """
    def __init__(self, name, now=None):
        if not now:
            now = datetime.datetime.utcnow()
        self.range = getattr(self, name)(now)

    def __repr__(self):
        return "<TimeInterval from %s to %s>" % self.range

    def __contains__(self, dt):
        return dt >= self.range[0] and dt < self.range[1]

    def today(self, now):
        midnightToday = now.replace(hour=0, minute=0, second=0, microsecond=0)
        midnightTomorrow = midnightToday + datetime.timedelta(days=1)
        return (midnightToday, midnightTomorrow)

    def yesterday(self, now):
        midnightToday = now.replace(hour=0, minute=0, second=0, microsecond=0)
        midnightYesterday = midnightToday - datetime.timedelta(days=1)
        return (midnightYesterday, midnightToday)

    def thisWeek(self, now):
        midnightToday = now.replace(hour=0, minute=0, second=0, microsecond=0)
        beginning = midnightToday - datetime.timedelta(days=calendar.weekday(now.year, now.month, now.day))
        end = beginning + datetime.timedelta(weeks=1)
        return (beginning, end)

    def lastWeek(self, now):
        thisWeek = self.thisWeek(now)
        return (thisWeek[0] - datetime.timedelta(weeks=1), thisWeek[0])

    def thisMonth(self, now):
        beginning = now.replace(hour=0, minute=0, second=0, microsecond=0, day=1)
        try:
            end = beginning.replace(month=now.month+1)
        except ValueError:
            # Next year
            end = beginning.replace(month=1, year=now.year+1)
        return (beginning, end)

    def lastMonth(self, now):
        end = now.replace(hour=0, minute=0, second=0, microsecond=0, day=1)
        try:
            beginning = end.replace(month=now.month-1)
        except ValueError:
            # Last year
            beginning = end.replace(month=12, year=now.year-1)
        return (beginning, end)


class RingBuffer(object):
    """A FIFO buffer for the last n objects delivered to it, based on Rack.
       The rack includes keys for the numbers 0 through n-1, plus the following
       strings:

          head  : The key to place the next node at
          count : Total number of nodes in the buffer
          size  : Size of this buffer, can't be changed after it's created.
                  The size specified to the constructor is only
                  used when a new database has been created.

       >>> d = RingBuffer(Rack.open('/tmp/ringbuffer_test.db', 'n'))
       >>> d.getLatest()
       []
       >>> d.push('foo')
       >>> d.push('bar')
       >>> d.push((42, None))
       >>> d.getLatest()
       ['foo', 'bar', (42, None)]
       >>> for i in xrange(5000):
       ...    d.push(str(i))
       >>> len(d.getLatest())
       1024
       >>> len(d.getLatest(10000))
       1024
       >>> len(d.getLatest(100))
       100
       >>> d.getLatest(10)
       ['4990', '4991', '4992', '4993', '4994', '4995', '4996', '4997', '4998', '4999']
       """
    def __init__(self, rack, size=1024):
        self.rack = rack

        if not self.rack.has_key('size'):
            # It's a new database, initialize the special keys
            self.rack['head'] = 0
            self.rack['count'] = 0
            self.rack['size'] = size

        # Cache the special keys
        self.head = self.rack['head']
        self.count = self.rack['count']
        self.size = self.rack['size']

    def push(self, node):
        """Add the given node to the FIFO, overwriting
           the oldest entries if the buffer is full.
           """
        # Stow the new node at our head and increment it
        self.rack[self.head] = node
        self.head = self.head + 1
        if self.head >= self.size:
            self.head -= self.size
        self.rack['head'] = self.head

        # If we haven't just also pushed out an old item,
        # increment the count of items in our rack.
        if self.count < self.size:
            self.count += 1
            self.rack['count'] = self.count

    def getLatest(self, n=None):
        """Returns up to the latest 'n' items. If n is None,
           returns the entire contents of the FIFO.
           Returns a list, oldest items first.
           """
        # Figure out how many items we can actually extract
        if n is None or n > self.count:
            n = self.count

        # Find the key holding the oldest item we want to return,
        # and start pulling items out from there
        key = self.head - n
        if key < 0:
            key += self.size
        results = []
        while n > 0:
            results.append(self.rack[key])
            key += 1
            if key >= self.size:
                key -= self.size
            n -= 1
        return results


class Counters(object):
    """A set of counters which are used together to track events
       occurring over several TimeIntervals. Stored in a Rack.
       """
    def __init__(self, rack):
        self.rack = rack

    def getCounter(self, name):
        """Return the Rack associated with a counter. The Rack
           is a subclass of the dictionary type, and can
           include the following keys:

           firstEventTime : The time, in UTC seconds since the epoch, when the first event occurred
           lastEventTime  : The time when the most recent event occurred
           eventCount     : The number of events that have occurred
           """
        return self.rack.namespace(name)

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
            if not datetime.datetime.utcfromtimestamp(cTime) in TimeInterval(current):
                # Our current timer is old, copy it to the previous timer and delete it
                p.clear()
                p.update(c)
                c.clear()

        if pTime is not None:
            if not datetime.datetime.utcfromtimestamp(pTime) in TimeInterval(previous):
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
