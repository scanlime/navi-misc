""" LibCIA.TimeUtil

Date/time utilities. This includes a class for representing common
time intervals like 'today' and 'last week', and methods for formatting
dates and durations.
"""
#
# CIA open source notification system
# Copyright (C) 2003-2004 Micah Dowty <micahjd@users.sourceforge.net>
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

from __future__ import division
import time, datetime, calendar


class Interval(object):
    """Represents some interval of time, like 'yesterday' or 'this week'.
       Provides functions for returning the bounds of the
       interval and testing whether a particular time is within it.
       Represents time using datetime objects. By default, the interval
       is created relative to the current date and time in UTC. To override
       this, a datetime object can be passed to the constructor.

       Interval is constructed with the name of the interval
       it should represent.

       >>> now = datetime.datetime(2003, 12, 19, 2, 19, 39, 50279)

       >>> Interval('today', now)
       <Interval from 2003-12-19 00:00:00 to 2003-12-20 00:00:00>

       >>> Interval('yesterday', now)
       <Interval from 2003-12-18 00:00:00 to 2003-12-19 00:00:00>

       >>> Interval('thisWeek', now)
       <Interval from 2003-12-15 00:00:00 to 2003-12-22 00:00:00>

       >>> Interval('lastWeek', now)
       <Interval from 2003-12-08 00:00:00 to 2003-12-15 00:00:00>

       >>> Interval('thisMonth', now)
       <Interval from 2003-12-01 00:00:00 to 2004-01-01 00:00:00>

       >>> Interval('lastMonth', now)
       <Interval from 2003-11-01 00:00:00 to 2003-12-01 00:00:00>
       """
    def __init__(self, name, now=None):
        if not now:
            now = datetime.datetime.utcnow()
        self.range = getattr(self, name)(now)

    def __repr__(self):
        return "<Interval from %s to %s>" % self.range

    def __contains__(self, dt):
        if not isinstance(dt, datetime.datetime):
            dt = datetime.datetime.utcfromtimestamp(dt)

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


# A table of time units, represented as a list of (name, seconds) tuples
units = [
    ('years',   365 * 24 * 60 * 60),
    ('months',  30 * 24 * 60 * 60),
    ('weeks',   7 * 24 * 60 * 60),
    ('days',    24 * 60 * 60),
    ('hours',   60 * 60),
    ('minutes', 60),
    ('seconds', 1),
    ('milliseconds', 0.001),
    ('microseconds', 0.000001),
    ]


def formatDuration(dt, threshold=0.8, format="%.02f %s"):
    """Given a duration in seconds, picks more appropriate units
       and accuracy to represent it with and returns a string.
       We convert to the first unit in which the given value would
       be greater than the given threshold.

       >>> formatDuration(3236523)
       '1.25 months'

       >>> formatDuration(40)
       '40.00 seconds'

       >>> formatDuration(50)
       '0.83 minutes'
       """
    for name, seconds in units:
        converted = dt / seconds
        if converted > threshold:
            break
    return format % (converted, name)


def formatDate(t):
    """Format a date, in UTC seconds since the epoch"""
    return time.strftime("%H:%M on %b %d, %Y", time.gmtime(t))


def _test():
    import doctest, TimeUtil
    return doctest.testmod(TimeUtil)

if __name__ == "__main__":
    _test()

### The End ###
