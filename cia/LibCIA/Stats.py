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

import Ruleset, XML, Message
import string, os, time, datetime, calendar


class StatsURIHandler(Ruleset.RegexURIHandler):
    """Handles stats:// URIs. The message passed to a stats:// URI is
       URI-encoded and added to the end of the stats:// URI to form
       a path identifying a class of messages that stats are collected for.
       """
    scheme = 'stats'
    regex = r"^stats://(?P<path>[a-zA-Z0-9_-]+(/[a-zA-Z0-9_-]+)*)$"

    def __init__(self, hub, statsDirectory):
        self.statsDirectory = statsDirectory
        Ruleset.RegexURIHandler.__init__(self)
        self.addClients(hub)

    def addClients(self, hub):
        """Add our own clients to the Message.Hub. We use this to listen
           for queryStats messages. The extra level of indirection here helps
           rebuild() work its magic without breaking.
           """
        hub.addClient(lambda msg: self.queryStats(msg),
                      Message.Filter('<find path="/message/body/queryStats">'))

    def queryStats(self, message):
        """Handle <queryStats> messages"""
        return "moo"

    def message(self, uri, message, content):
        # Stick the URI's path and the content together into a stats
        # path, URI-encoding the content first. This combined with
        # our statsDirectory gives us the local path for a stats target.
        StatsTarget(os.path.join(self.statsDirectory, self.parseURI(uri)['path'],
                                 uriencode(content))).deliver(message)


def uriencode(s, allowedChars = string.ascii_letters + string.digits + "-_"):
    """Return a URI-encoded version of 's', all characters not in the
       given list will be replaced with their hexadecimal value prefixed
       with '%'.
       """
    chars = []
    for char in s:
        if char in allowedChars:
            chars.append(char)
        else:
            chars.append("%%%02X" % ord(char))
    return "".join(chars)


class StatsTarget(object):
    """Encapsulates all the stats-logging features used for one particular
       target. This can be one project, one class of messages, etc.
       It is constructed with the path of a directory in the filesystem
       which represents this target.
       """
    def __init__(self, path):
        self.path = path
        self.createIfNecessary(path)
        self.counters = IntervalCounters(os.path.join(self.path, 'counters.xml'))
        self.recentMessages = MessageBuffer(os.path.join(self.path, 'recent_messages.xml'))

    def createIfNecessary(self, path):
        try:
            os.makedirs(path)
        except OSError:
            pass

    def deliver(self, message):
        """A message has been received which should be logged by this stats target"""
        self.counters.increment()
        self.recentMessages.push(message)


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
        XML.XMLStorage.__init__(self, fileName, 'counters')

    def emptyStorage(self):
        # Maps counter name to Counter instance
        self.counters = {}

    def store(self, xml):
        c = Counter(xml)
        self.counters[c.name] = c

    def flatten(self):
        return self.counters.values()

    def getCounter(self, name):
        """Get a counter with the given name, creating it if it doesn't exist"""
        if not self.counters.has_key(name):
            self.counters[name] = Counter(name=name)
        return self.counters[name]


class MessageBuffer(XML.XMLStorage):
    """A FIFO holding the last n messages delivered to it"""
    def __init__(self, fileName, size=100):
        self.size = size
        XML.XMLStorage.__init__(self, fileName, 'messages')

    def emptyStorage(self):
        self.buffer = []

    def store(self, xml):
        self.buffer.append(Message.Message(xml))

    def flatten(self):
        return self.buffer

    def push(self, message):
        self.buffer.append(message)
        while len(self.buffer) > self.size:
            del self.buffer[0]
        self.save()


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

    def _getValue(self, type, name, default=None, castTo=None):
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

    def _setValue(self, type, name, value):
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
        return self._getValue('time', 'creation', castTo=int)

    def getCreationDatetime(self):
        """Like getCreationTime, but cast the result to a UTC datetime object"""
        return datetime.datetime.utcfromtimestamp(self.getCreationTime())

    def setCreationTime(self, value):
        self._setValue('time', 'creation', value)

    def getFirstEventTime(self):
        return self._getValue('time', 'firstEvent', castTo=int)

    def setFirstEventTime(self, value):
        self._setValue('time', 'firstEvent', value)

    def getLastEventTime(self):
        return self._getValue('time', 'lastEvent', castTo=int)

    def setLastEventTime(self, value):
        self._setValue('time', 'lastEvent', value)

    def getEventCount(self):
        return self._getValue('count', 'events', 0, castTo=int)

    def incrementEventCount(self):
        self._setValue('count', 'events', self.getEventCount() + 1)

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
