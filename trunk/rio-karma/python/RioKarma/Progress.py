""" RioKarma.Progress

This module implements a simple system for reporting progress updates,
and showing those progress updates in various ways. This progress
reporting system extends Twisted's Deferred type to include 'statusback'
and 'addStatusback', analagous to callback/errback and
addCallback/addErrback. Progress.Deferred's can be aggregated to
provide the progress of one large operation that consists of several
separate pieces.

updates. Like a Deferred's callback/errback, a Progress.Deferred's
statusback

"""
#
# Copyright (C) 2005 Micah Dowty <micah@navi.cx>
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

from __future__ import division
import sys
from cStringIO import StringIO
from twisted.internet import defer


class StatusMixin:
    """This is a mix-in class that implements statusback functionality.
       Status callbacks have four parameters:

       completed:  The amount completed so far, in 'units'

       total:      The total amount of work to do, in 'units'

       units:      The name of the units used for completed/total.
                   Could be None if they are arbitrary, or a string.

       name:       The name of the operation being performed, or
                   None if it is unknown. As the status bubbles up
                   the chain toward a UI of some sort, its name will
                   be refined. The name may also change to indicate
                   what exactly we're doing in an operation- for example
                   if several status deferreds are combined.
       """
    statusbacks = None

    def addStatusback(self, sb):
        if self.statusbacks is None:
            self.statusbacks = []
        self.statusbacks.append(sb)
        return self

    def statusback(self, completed, total, units=None, name=None):
        if self.statusbacks:
            for sb in self.statusbacks:
                sb(completed, total, units, name)

    def chainStatus(self, d, renameTo=None):
        if renameTo is None:
            self.addStatusback(d.statusback)
        else:
            self.addStatusback(lambda completed, total, units, name:
                               d.statusback(completed, total, units, renameTo))


class Deferred(StatusMixin, defer.Deferred):
    pass


class DeferredList(StatusMixin, defer.DeferredList):
    def __init__(self, deferredList, **kwargs):
        defer.DeferredList.__init__(self, deferredList, **kwargs)
        index = 0
        for deferred in deferredList:
            deferred.addStatusback(self._sbDeferred, index)
            index += 1
        self._sbResults = [None] * index

    # FIXME: This won't work, needs a statusback() implementation
    #        that will aggregate results properly.

def gatherResults(deferredList):
    d = DeferredList(deferredList, fireOnOneErrback=1)
    d.addCallback(defer._parseDListResult)
    return d


class Reporter:
    """This is an abstract base class for objects that can report
       progress updates to an end user.
       """
    def statusback(self, completed, total, units=None, name=None):
        """This is our interface to the Progress.Deferred,
           defined here in the subclass to avoid exposing a
           relatively fragile interface to each UI.
           """
        self.completed = completed
        self.total = total
        self.units = units
        self.name = name

        # Extra calculations based on the Deferred-provided raw data
        self.fraction = max(0, min(1, self.completed / self.total))
        self.percent = "%3.0f%%" % (self.fraction * 100)

        # Now get the UI to update itself
        self.update()

    def update(self):
        """Subclasses must define this to actually deliver
           the news to a user somehow.
           """
        pass


class ConsoleReporter(Reporter):
    """A simple Progress.Reporter implementation for command line use.
       In order to maintain the console's state cleanly, this can intercept
       normal output to stdout/stderr and places it above the progress
       bar.
       """
    currentMessage = None

    def __init__(self,
                 barWidth      = 40,
                 completedChar = '#',
                 remainingChar = '-',
                 spinner       = r"-\|/",
                 install       = True
                 ):
        self.barWidth = barWidth
        self.completedChar = completedChar
        self.remainingChar = remainingChar
        self.spinner = spinner

        if install:
            self.install()

    def install(self):
        """Set up this ConsoleReporter to intercept normal stdout/stderr text"""
        self._savedStdout = sys.stdout
        self._savedStderr = sys.stderr
        sys.stdout = self
        sys.stderr = self

    def uninstall(self):
        """Reverse the effects of install()"""
        sys.stdout = self._savedStdout
        sys.stderr = self._savedStderr

    def write(self, text):
        """This method receives normal text written to stdout/stderr while
           this console reporter is installed.
           """
        buffer = StringIO()

        if self.currentMessage is not None:
            buffer.write('\b' * len(self.currentMessage))
            buffer.write(' ' * len(self.currentMessage))
            buffer.write('\b' * len(self.currentMessage))

        buffer.write(text)

        if self.currentMessage is not None:
            buffer.write(self.currentMessage)

        self._savedStderr.write(buffer.getvalue())

    def setMessage(self, message):
        """Replace the currently displayed progress message,
           erasing the old one first. The message may be
           None to clear the old without writing a new one.
           """
        buffer = StringIO()

        if self.currentMessage is not None:
            buffer.write('\b' * len(self.currentMessage))
            buffer.write(' ' * len(self.currentMessage))
            buffer.write('\b' * len(self.currentMessage))

        self.currentMessage = message
        if self.currentMessage is not None:
            buffer.write(self.currentMessage)

        self._savedStderr.write(buffer.getvalue())

    def update(self):
        completed = int(self.fraction * self.barWidth + 0.5)
        if completed < self.barWidth:
            spinnerChar = self.spinner[0]
            self.spinner = self.spinner[1:] + spinnerChar
        else:
            spinnerChar = ''

        self.setMessage("%s %s%s%s %s " % (
            self.percent,
            self.completedChar * completed,
            spinnerChar,
            self.remainingChar * (self.barWidth - completed - 1),
            self.name))

### The End ###
