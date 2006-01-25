""" LibCIA.TimeUtil

Date/time utilities. This includes a class for representing common
time intervals like 'today' and 'last week', and methods for formatting
dates and durations.
"""
#
# CIA open source notification system
# Copyright (C) 2003-2005 Micah Dowty <micah@navi.cx>
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

import Units
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

    def getFirstTimestamp(self):
        """Return the first timestamp value within this range"""
        return datetimeToTimestamp(self.range[0])

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


formatDuration = Units.TimeUnits().format


def formatDate(t):
    """Format a date, in UTC seconds since the epoch.
       This should use a format that doesn't necessarily adhere to
       any standard, but is easy to read and fairly universal.
       """
    return time.strftime("%H:%M on %b %d, %Y", time.gmtime(t))

def formatRelativeDate(t):
    """Like formatDate, this is an arbitrary format chosen to be
       easily human-readable, but this function is allowed to
       render it relative to the current time.
       """
    now = time.time()
    delta = int(now - t)

    if delta < 0:
        return "%ss in the future" % -delta
    if delta < 1:
        return "< 1 sec ago"
    if delta < 60:
        return "%d sec ago" % delta

    delta /= 60
    if delta < 60:
        return "%d min ago" % delta

    tmNow = time.gmtime(now)
    tm = time.gmtime(t)
    if tm.tm_year == tmNow.tm_year:

        if tm.tm_yday == tmNow.tm_yday:
            return time.strftime("%H:%M today", tm)

        if tm.tm_yday == tmNow.tm_yday - 1:
            return time.strftime("%H:%M yesterday", tm)

        if tmNow.tm_yday - tm.tm_yday < 7:
            return time.strftime("%H:%M %A", tm)

        return time.strftime("%H:%M on %b %d", tm)

    return time.strftime("%H:%M on %b %d, %Y", tm)
    

def formatLogDate(t):
    """Format a date in the format they should be in our log file.
       This should look just like the date Twisted puts in automatically,
       in the local time zone.
       """
    y,mon,d,h,min, iigg,nnoo,rree,daylight = time.localtime(t)
    return "%0.4d/%0.2d/%0.2d %0.2d:%0.2d %s" % (
        y, mon, d, h, min, time.tzname[daylight])

def formatDateRFC822(t):
    """Format a date, in UTC seconds since the epoch, using RFC822 formatting"""
    return time.strftime("%a, %d %b %Y %H:%M:%S +0000", time.gmtime(t))


def formatDateISO8601(t):
    """Format a date, in UTC seconds since the epoch, using ISO 8601 format"""
    return time.strftime("%Y-%m-%dT%H:%M:%SZ", time.gmtime(t))


def mktime_utc(data):
    t = time.mktime(data[:8] + (0,))
    return t - time.timezone

def datetimeToTimestamp(datetime):
    """Convert a UTC datetime object to a UTC timestamp"""
    return mktime_utc(datetime.utctimetuple())

### The End ###
