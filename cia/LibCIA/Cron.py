""" LibCIA.Cron

A simple scheduler for repeated events. Each event is represented
by a frequency in seconds, and a callable that handles the event.

Trigger times are always an integer multiple of the period, so
any period that one day can be evenly divided into will always run
at the same time each day. An event with a period of an hour will
always run at the top of the hour, and an event that runs
every 15 minutes will always run at :00, :15, :30, and :45.

Events are guaranteed not to be triggered too early, but they might
be late if the system is heavily loaded.

Normally all triggers are run once when they are added, since
presumably if the server is just starting it will have missed a lot
of opportunities to trigger events. This makes sense for most
maintenance tasks. If it doesn't, you can disable that by adding the
event with runNow=False.
"""
#
# CIA open source notification system
# Copyright (C) 2003-2007 Micah Dowty <micah@navi.cx>
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

import time
import TimeUtil
from twisted.internet import reactor, defer
from twisted.python import log

# Just some convenience definitions you can use for event periods
hourly = 60 * 60
daily = hourly * 24


class Event:
    """An event that can be scheduled by Cron. A callable is run when
       the event needs to be triggered. Given the current time, an
       event can report when it needs to run next.

       If the callable isn't expected to complete its tasks right away,
       it should return a Deferred.
       """
    def __init__(self, period, callable, name=None, runNow=False):
        if not name:
            name = repr(callable)

        self.period = period
        self.callable = callable
        self.name = name

        if runNow:
            self.triggerTime = 0
        else:
            self.triggerTime = self.findNextTriggerTime()

    def findNextTriggerTime(self, now=None):
        """Find the next time (in seconds since the epoch) that this
           event should be triggered, after the current time.
           """
        if now is None:
            now = time.time()
        return (now // self.period + 1) * self.period

    def trigger(self):
        """Trigger this event, and update the time at which
           the next triggering should occur.
           """
        self.triggerTime = self.findNextTriggerTime()
        log.msg("Cron: running %s, next run at %s" %
                (self.name, TimeUtil.formatLogDate(self.triggerTime)))
        defer.maybeDeferred(self.callable).addCallback(self.triggerFinished)

    def triggerFinished(self, result):
        log.msg("Cron: finished %s" % self.name)


class Scheduler:
    """Holds events, triggering them when they want to be triggered"""
    def __init__(self, *events):
        # Maps from event instance to the DelayedCall that schedules it
        self.events = {}
        for event in events:
            self.schedule(event)

    def cancel(self, event):
        """Stop triggering the given event"""
        if event in self.events:
            if self.events[event].active():
                self.events[event].cancel()
            del self.events[event]

    def schedule(self, event):
        """Delete any stale DelayedCalls we may have for this event, then
           create one that will trigger our event at the correct time.
           """
        self.cancel(event)
        delay = event.triggerTime - time.time()
        if delay > 0:
            self.events[event] = reactor.callLater(event.triggerTime - time.time(), self.trigger, event)
        else:
            self.trigger(event)

    def trigger(self, event):
        """Trigger the given event then reschedule it"""
        event.trigger()
        self.schedule(event)

### The End ###
