""" LibCIA.Web.StatsBrowser

A web interface, built using nevow, for CIA's stats:// namespace
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

import Base
import time
from LibCIA import TimeUtil


class Clock(Base.Section):
    """Display the current time in UTC"""
    def getTime(self):
        return TimeUtil.formatDate(time.time())

    title = "UTC Clock"
    body = getTime


class StatsPage(Base.Template):
    def __init__(self, caps, storage):
        self.caps = caps
        self.storage = storage
        Base.Template.__init__(self)

    def getDynamicChild(self, name, request):
        return self

    def render_leftColumn(self, context, data):
        s = Base.SectionSerializer(Base.Section("Squirrels", "moose bites?"))
        from twisted.python import components
        from nevow.iwoven import ISerializable
        print "squidgey", components.implements(s, ISerializable)
        return [
            s
            ]

### The End ###
