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

import time
import Template
from LibCIA import TimeUtil
from Base import tag, place


class Clock(Template.Section):
    title = "UTC Clock"

    def render_rows(self, context):
        return [TimeUtil.formatDate(time.time())]


class Counters(Template.Section):
    """A Section displaying the counters from a StatsTarget"""
    title = "event counters"

    def __init__(self, target):
        self.target = target

    def render_relativeDate(self, context, counterName, valueName):
        return self.target.counters.getCounter(counterName).get(valueName)

    rows = [
               ['The last message was received ',
                tag('b')[ place('relativeDate', 'forever', 'lastEventTime') ]]
           ]


class StatsLink(tag):
    """An anchor tag linking to the given stats target"""
    def __init__(self, target, **kwargs):
        url = "boing"
        tag.__init__(self, 'a', href=url, **kwargs)
        self.content = target.getTitle()


class Catalog(Template.Section):
    """A Section displaying links to all children of a StatsTarget"""
    title = "catalog"
    rows = [tag('ul', _class="catalog")[ place('items') ]]

    def __init__(self, target):
        self.target = target

    def render_items(self, context):
        return [tag('li')[StatsLink(self.target.child(name))] for name in self.target.catalog()]


class StatsPage(Template.Page):
    """A web page providing an interface to one StatsTarget"""
    def __init__(self, caps, target):
        self.caps = caps
        self.target = target

    def getChildWithDefault(self, name, request):
        """Part of IResource, called by twisted.web to retrieve pages for URIs
           below this one. This just creates a StatsPage instance for our StatsTarget's child.
           """
        if name:
            return StatsPage(self.caps, self.target.child(name))
        else:
            # Ignore empty path sections
            return self

    def render_mainTitle(self, context):
        return self.target.getTitle()

    def render_mainColumn(self, context):
        return [
            Counters(self.target),
            Catalog(self.target),
            ]

    leftColumn = [
        Clock()
        ]

### The End ###
