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

from twisted.internet import defer

def StatusMixin:
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

    def statusback(self, completed

def gatherResults(deferredList):
    d = DeferredList(deferredList, fireOnOneErrback=1)
    d.addCallback(defer._parseDListResult)
    return d

### The End ###
