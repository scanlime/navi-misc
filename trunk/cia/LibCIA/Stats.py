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

import Ruleset, XML
import re, string, os, time


class StatsURIHandler(Ruleset.RegexURIHandler):
    """Handles stats:// URIs. The message passed to a stats:// URI is
       URI-encoded and added to the end of the stats:// URI to form
       a path identifying a class of messages that stats are collected for.
       """
    scheme = 'stats'
    regex = r"^stats://(?P<path>[a-zA-Z0-9_-]+(/[a-zA-Z0-9_-]+)*)$"

    def __init__(self, statsDirectory):
        self.statsDirectory = statsDirectory
        Ruleset.RegexURIHandler.__init__(self)

    def message(self, uri, message, content):
        # Stick the URI's path and the content together into a stats
        # path, URL-encoding the content first. This combined with
        # our statsDirectory gives us the local path for a stats target.
        t = StatsTarget(os.path.join(self.statsDirectory, self.parseURI(uri)['path'], urlencode(content)))

        # Count this freshly received messages among our stats
        t.increment()


def urlencode(s, allowedChars = string.ascii_letters + string.digits + "-_"):
    """Return a URL-encoded version of 's', all characters not in the
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
        self.counters = CounterList(os.path.join(self.path, 'counters.xml'))

    def createIfNecessary(self, path):
        try:
            os.makedirs(path)
        except OSError:
            pass

    def increment(self):
        """An event has occurred which should be logged in this stats target"""
        self.counters.getCounter("foo").increment()
        self.counters.save()


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

### The End ###
