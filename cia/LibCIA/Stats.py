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
import Ruleset, XML, Message
import string, os
import time, datetime, calendar
import anydbm


class StatsStorage(object):
    """A thin abstraction around the directory that acts as a root for all stats:// URIs"""
    def __init__(self, path):
        self.path = path

    def getTarget(self, pathSegments):
        """Return a StatsTarget representing the stats stored at the given
           path, represented as one or more nested directories. Disallowed
           characters in each path segment are URI-encoded.
           """
        return StatsTarget(os.path.join(self.path, *map(self.uriencode, pathSegments)))

    def getPathTarget(self, path, *extraSegments):
        """Like getTarget, but split up 'path' into segments and optionall append extraSegments first"""
        return self.getTarget(list(path.split("/")) + list(extraSegments))

    def uriencode(self, s, allowedChars = string.ascii_letters + string.digits + "-_"):
        """Return a URI-encoded version of 's', all characters not in the
           given list will be replaced with their hexadecimal value prefixed
           with '%'.
           This is like urllib.quote(), but we can't use that since we don't assume
           the '.' character is safe- it can be used to create hidden files and special
           directory names, so it's easier to just quote it rather than detecting
           those special cases.
        """
        chars = []
        for char in s:
            if char in allowedChars:
                chars.append(char)
            else:
                chars.append("%%%02X" % ord(char))
        return "".join(chars)


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
        self.caps = caps
        self.storage = storage

    def xmlrpc_isPathValid(self, path):
        """Returns 'true' if the given stats path exists at all"""
        return self.storage.getPathTarget(path).exists()

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

    def xmlrpc_getCounters(self, path):
        """Return counters for the given path as XML"""
        return self.storage.getPathTarget(path).countersToXml()

    def xmlrpc_catalogCounters(self, path):
        """Runs 'catalog' on the given path to determine it's subdirectories,
           then creates a mapping from subdirectory name to getCounters results
           and returns that.
           """
        return self.storage.getPathTarget(path).catalogCounters()


class StatsTarget(object):
    """Encapsulates all the stats-logging features used for one particular
       target. This can be one project, one class of messages, etc.
       It is constructed with the path of a directory in the filesystem
       which represents this target.

       On message delivery, the path and all files within will be created
       automatically. Until then though, the path may not exist and the
       'counters' and 'recentMessages' attributes may be None.
       """
    def __init__(self, path):
        self.path = path
        self.open(False)

    def exists(self):
        """Returns True if this stats target is valid"""
        return os.path.isdir(self.path)

    def open(self, createIfNecessary=True):
        """Open all resources in this stats target. If createIfNecessary is True,
           the resources will be created if they don't already exist.
           """
        # If we're in the business of creating things that don't exist,
        # make sure our stats path exists.
        if createIfNecessary:
            try:
                os.makedirs(self.path)
            except OSError:
                pass

        for attrName, cls, filename in [
            ('counters', IntervalCounters, os.path.join(self.path, 'counters.xml')),
            ('recentMessages', LogDB, os.path.join(self.path, 'recent_messages.db')),
            ]:
            if hasattr(self, attrName) and getattr(self, attrName):
                # Already open, skip it
                continue
            if createIfNecessary or os.path.isfile(filename):
                obj = cls(filename)
            else:
                obj = None
            setattr(self, attrName, obj)

    def deliver(self, message):
        """A message has been received which should be logged by this stats target"""
        self.open()
        self.counters.increment()
        self.recentMessages.push(str(message))

    def catalog(self):
        """Return a list of subdirectories of this stats target"""
        try:
            return [dir for dir in os.listdir(self.path) if os.path.isdir(os.path.join(self.path, dir))]
        except OSError:
            # Nonexistant stats targets have no subdirectories
            return []

    def countersToXml(self):
        """Return all counters as raw XML"""
        if self.counters:
            return self.counters.toXml()
        else:
            return ''

    def child(self, name):
        """Return the StatsTarget for the given subdirectory name under this one"""
        return StatsTarget(os.path.join(self.path, name))

    def catalogCounters(self):
        """Return a mapping from subdirectory name to XML counters"""
        results = {}
        for child in self.catalog():
            results[child] = self.child(child).countersToXml()
        return results


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


class CounterList(XML.XMLStorage):
    """Several named Counters stored in one file"""
    def __init__(self, fileName):
        # Use lazy loading to speed things up if we need to refer to
        # a counter list without necessarily being able to read its contents.
        XML.XMLStorage.__init__(self, fileName, 'counters', lazyLoad=True)

    def emptyStorage(self):
        # Maps counter name to Counter instance
        self.counters = {}

    def store(self, xml):
        c = Counter(xml)
        self.counters[c.name] = c

    def flatten(self):
        return self.counters.values()

    def getNames(self):
        """Return all currently valid counter names"""
        self.loadIfNecessary()
        return self.counters.keys()

    def getCounter(self, name, create=True):
        """Get a counter with the given name. If 'create' is True, it
           will be created if necessary. If 'create' is False and the
           counter doesn't exist, None will be returned.
           """
        self.loadIfNecessary()
        if not self.counters.has_key(name):
            if create:
                self.counters[name] = Counter(name=name)
            else:
                return None
        return self.counters[name]


class LogDB(object):
    """A database (accessed via anydbm) used as a FIFO for the
       last n objects delivered to it. The objects in this
       case are always strings. The database is organized as a
       ring buffer. Keys include the numbers 0 through n-1,
       plus the following special keys:

          head  : The key to place the next node at
          count : Total number of nodes in the buffer
          size  : Size of this buffer, can't be changed after it's created.
                  The size specified to the constructor is only
                  used when a new database has been created.

       >>> d = LogDB('/tmp/example.db', 'n')
       >>> d.getLatest()
       []
       >>> d.push('foo')
       >>> d.push('bar')
       >>> d.push('boing')
       >>> d.getLatest()
       ['foo', 'bar', 'boing']
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
    def __init__(self, fileName, flags='c', size=1024):
        self.db = anydbm.open(fileName, flags)
        if not self.db.has_key('size'):
            # It's a new database, initialize the special keys
            self.db['head'] = '0'
            self.db['count'] = '0'
            self.db['size'] = str(size)
        self.size = int(self.db['size'])
        self.head = int(self.db['head'])
        self.count = int(self.db['count'])

    def close(self):
        self.db.close()

    def push(self, node):
        """Add the given node to the FIFO, overwriting
           the oldest entries if the buffer is full.
           'node' must be a string.
           """
        # Stow the new node at our head and increment it
        self.db[str(self.head)] = node
        self.head = self.head + 1
        if self.head >= self.size:
            self.head -= self.size
        self.db['head'] = str(self.head)

        # If we haven't just also pushed out an old item,
        # increment the count of items in our db.
        if self.count < self.size:
            self.count += 1
            self.db['count'] = str(self.count)

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
            results.append(self.db[str(key)])
            key += 1
            if key >= self.size:
                key -= self.size
            n -= 1
        return results


class IntervalCounters(CounterList):
    """A set of counters which are used together to track events
       occurring over several TimeIntervals.
       """
    def checkOneRollover(self, previous, current):
        """Check for rollovers in one pair of consecutive time intervals,
           like yesterday/today or lastMonth/thisMonth.
           """
        # Is our current counter still current?
        if not self.getCounter(current).getCreationDatetime() in TimeInterval(current):
            # Nope, it's not. Is it within the previous interval?
            if self.getCounter(current).getCreationDatetime() in TimeInterval(previous):
                # Yes, roll over the current counter into previous
                self.getCounter(previous).copyFrom(self.getCounter(current))
            else:
                # No, it's really old... reset the previous counter
                self.getCounter(previous).reset()
            # Either way we need to reset the current counter
            self.getCounter(current).reset()

    def checkRollovers(self):
        """If any of the counters were created outside the interval
           they apply to, transfer its value to another counter
           if we need to and reset it.
           """
        self.checkOneRollover('yesterday', 'today')
        self.checkOneRollover('lastWeek', 'thisWeek')
        self.checkOneRollover('lastMonth', 'thisMonth')

    def load(self):
        """Check for rollovers on load"""
        CounterList.load(self)
        self.checkRollovers()

    def increment(self):
        """Increments all applicable counters in this list
           and saves them, after checking for rollovers.
           """
        self.checkRollovers()
        self.getCounter('today').increment()
        self.getCounter('thisWeek').increment()
        self.getCounter('thisMonth').increment()
        self.getCounter('forever').increment()
        self.save()


class Counter(XML.XMLObject):
    """Represents a tally of events and event frequency in a particular
       time period. Each counter has a permenently-assigned name which is
       used by the CounterSchedule to roll over counters when appropriate.
       Each counter stores timestamps for its creation, the first event
       counted, and the last event counted, as well as storing the total
       number of events counted.
       """
    def __init__(self, xml=None, name=None):
        if xml is None:
            xml = "<counter>\n</counter>"
        XML.XMLObject.__init__(self, xml)
        if name is not None:
            self.xml['name'] = name
            self.name = name

    def reset(self):
        """Delete all values in this counter, restoring their defaults"""
        self.xml.children = ["\n"]
        self.preprocess()

    def preprocess(self):
        # Make sure that we have a creation time, adding it if we don't
        if self.getCreationTime() is None:
            self.setCreationTime(int(time.time()))
        # Store the name
        self.name = self.xml.getAttribute('name')

    def getValue(self, type, name, default=None, castTo=None):
        """Return the value in this counter stored as the
           given data type using 'name'. If it can't be
           found, the provided default will be returned.
           The value is always returned as a string.
           """
        for child in self.xml.children:
            if not isinstance(child, XML.domish.Element):
                continue
            if child.name != type:
                continue
            if child.getAttribute('name') != name:
                continue
            value = str(child).strip()
            if castTo is not None:
                value = castTo(value)
            return value
        return default

    def setValue(self, type, name, value):
        """Set the value in this counter with the given type
           and name, overwriting an old value if there was one.
           """
        # First try to find an old value
        for child in self.xml.children:
            if not isinstance(child, XML.domish.Element):
                continue
            if child.name != type:
                continue
            if child.getAttribute('name') != name:
                continue

            # Erase any existing content for this tag and set our new content
            child.children = [str(value)]
            return

        # No old value, make a new tag
        self.xml.addElement(type, content=str(value))['name'] = name
        self.xml.children.append("\n")

    def getCreationTime(self):
        return self.getValue('time', 'creation', castTo=int)

    def getCreationDatetime(self):
        """Like getCreationTime, but cast the result to a UTC datetime object"""
        return datetime.datetime.utcfromtimestamp(self.getCreationTime())

    def setCreationTime(self, value):
        self.setValue('time', 'creation', value)

    def getFirstEventTime(self):
        return self.getValue('time', 'firstEvent', castTo=int)

    def setFirstEventTime(self, value):
        self.setValue('time', 'firstEvent', value)

    def getLastEventTime(self):
        return self.getValue('time', 'lastEvent', castTo=int)

    def setLastEventTime(self, value):
        self.setValue('time', 'lastEvent', value)

    def getEventCount(self):
        return self.getValue('count', 'events', 0, castTo=int)

    def incrementEventCount(self):
        self.setValue('count', 'events', self.getEventCount() + 1)

    def increment(self, evTime=None):
        """Count an incoming event. This increments our total event count,
           and sets what timers need setting. A timestamp for the event
           can optionally be provided, if it isn't the current time will
           be used.
           """
        if evTime is None:
            evTime = int(time.time())
        if self.getFirstEventTime() is None:
            self.setFirstEventTime(evTime)
        self.setLastEventTime(evTime)
        self.incrementEventCount()

    def copyFrom(self, other):
        """Copy all content from the other counter into this counter
           excepting the name. This includes all timestamps.
           """
        # Kinda kludgey, but copy.deepcopy() chokes on something in domish
        savedName = self.name
        self.loadFromString(other.xml.toXml())
        self.xml['name'] = savedName
        self.name = savedName


def _test():
    import doctest, Stats
    return doctest.testmod(Stats)

if __name__ == "__main__":
    _test()

### The End ###
