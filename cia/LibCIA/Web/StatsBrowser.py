""" LibCIA.StatsBrowser

A web interface using Woven for browsing CIA's stats:// namespace
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

from twisted.web import static, domhelpers
from twisted.web.woven import page, widgets, model
from LibCIA import Message
import os, urllib, time


def pathSplit(s):
    """Given a path as a string, return it split into directories,
       ignoring leading and trailing slashes or multiple slashes.
       """
    return [i for i in s.split('/') if i]


class CounterListModel(model.Wrapper):
    """A Model representing a set of event counters, constructed
       around a Stats.CounterList. Submodels are dictionaries as
       returned by the counter's getValues() member.
       """
    def getSubmodel(self, request, name):
        if self.original:
            counter = self.original.getCounter(name, create=False)
            return CounterModel(counter)
        return CounterModel(None)


class RecentMessagesModel(model.Wrapper):
    """A Model wrapping a StatsTarget's recentMesssages attribute
       (a LogDB instance). Submodels are integers representing how many
       messages to list. Submodels of those integers are lists of
       messages in XML.
       """
    def getSubmodel(self, request, name):
        if self.original:
            return model.ListModel(self.original.getLatest(int(name)))
        return model.ListModel([])


class CounterModel(model.MethodModel):
    """A Model representing a single event counter, providing submodels
       for retrieving the total event count, creation date, most recent
       event date, and mean time between events. This may be constructed
       with None to stand in for a nonexistent counter, returning default
       values.

       Times are returned in UNIX-style seconds since the epoch, in UTC.
       The meanPeriod is in seconds.
       """
    def wmfactory_eventCount(self, request):
        if self.original:
            return self.original.getEventCount()
        else:
            return 0

    def wmfactory_firstEventTime(self, request):
        if self.original:
            return self.original.getFirstEventTime()
        return ''

    def wmfactory_lastEventTime(self, request):
        if self.original:
            return self.original.getLastEventTime()
        return ''

    def wmfactory_creationTime(self, request):
        if self.original:
            return self.original.getCreationTime()
        return ''

    def wmfactory_meanPeriod(self, request):
        if self.original:
            lastTime = self.original.getLastEventTime()
            firstTime = self.original.getFirstEventTime()
            count = self.original.getEventCount()
            if lastTime and firstTime and count > 1:
                return float(lastTime - firstTime) / count
        return ''


class Conditional(widgets.Widget):
    """A widget that hides its children if it is constructed with a value
       that doesn't evaluate to True.
       """
    def initialize(self, condition=True):
        self.condition = condition

    def generateDOM(self, request, node):
        node = widgets.Widget.generateDOM(self, request, node)
        if not self.condition:
            domhelpers.clearNode(node)
        return node


class StatsPage(page.Page):
    """A Woven view representing one stats:// path"""

    templateFile = "stats_browser.xhtml"
    templateDirectory = os.path.split(os.path.abspath(__file__))[0]

    def initialize(self, caps=None, storage=None, path=''):
        self.caps = caps
        self.storage = storage
        self.path = path
        self.target = self.storage.getPathTarget(path)

    def getDynamicChild(self, name, request):
        if self.path == '' or self.path[-1] == '/':
            newPath = self.path + name
        else:
            newPath = self.path + '/' + name
        return StatsPage(caps = self.caps,
                         storage = self.storage,
                         path = newPath)

    def submodelCheck(self, request, name):
        """The default implementation of this chokes when name is None"""
        return name and hasattr(self, "wmfactory_"+name)

    def getPathTo(self, request, destination):
        """Assuming this is the page requested, return a relative URI to the given page"""
        # Figure out how many levels deep we are in the stats path
        # and what levels there are in the destination, and start
        # cancelling out what we can to create an optimized relative path.
        selfPath = pathSplit(self.path)
        upLevels = len(selfPath)
        down = pathSplit(destination.path)
        while selfPath and down and selfPath[0] == down[0]:
            del selfPath[0]
            del down[0]
            upLevels -= 1
        return '/'.join(['..'] * upLevels + down + [''])

    def getNodeModel(self, request, node, submodel):
        """Override the default getNodeModel so that nodes we can't find
           turn into None rather than causing an exception. This makes
           our 'conditional' widget work.
           """
        try:
            page.Page.getNodeModel(self, request, node, submodel)
        except:
            return None

    def addTimeUnits(self, original):
        """Convert a time in seconds to a time in some other appropriate units"""
        # A table of various units, listed in decreasing order.
        # We convert to the first unit in which the given value would
        # be greater than some threshold
        threshold = 0.8
        units = (
            ('years',   365 * 24 * 60 * 60),
            ('months',  30 * 24 * 60 * 60),
            ('weeks',   7 * 24 * 60 * 60),
            ('days',    24 * 60 * 60),
            ('hours',   60 * 60),
            ('minutes', 60),
            ('seconds', 1),
            )
        for name, seconds in units:
            converted = original / seconds
            if converted > threshold:
                break
        return "%.02f %s" % (converted, name)


    ######################################### Submodel Factories

    def wmfactory_uri(self, request):
        return "stats://" + self.path

    def wmfactory_path(self, request):
        return self.path

    def wmfactory_catalog(self, request):
        """Returns a list of all pages below this one, as
           StatsPage instances, sorted case-insentitively by title.
           """
        cat = [self.getDynamicChild(name, request) for name in self.target.catalog()]
        cat.sort(lambda a, b: cmp(a.wmfactory_title(request).lower(),
                                  b.wmfactory_title(request).lower()))
        return cat

    def wmfactory_metadata(self, request):
        """Return a dictionary of all metadata for this stats target"""
        if self.target.metadata:
            return self.target.metadata.dict
        return {}

    def wmfactory_title(self, requeset):
        """Return the human-readable title of this stats target. This
           is loaded from the 'title' metadata item if that exists, otherwise
           it's an un-URI-encoded version of the last item in our path.
           """
        # First try to return the 'title' metadata key
        if self.target.metadata:
            try:
                title = self.target.metadata.dict['title']
                if title:
                    return title
            except KeyError:
                pass

        # Now try the path
        title = urllib.unquote(self.path.split('/')[-1])
        if title:
            return title

        # If that failed, we're at the root- make up a default root title
        return "Stats"

    def wmfactory_counters(self, request):
        """Return a CountersModel instance that can be used to access
           the event counters stored at this stats path.
           """
        return CounterListModel(self.target.counters)

    def wmfactory_recentMessages(self, request):
        """Returns a model that can be used to return the 'n' most recent
           messages delivered to this stats target.
           """
        return RecentMessagesModel(self.target.recentMessages)


    ######################################### Widget Factories

    def wvfactory_statsLink(self, request, node, data):
        """Create a widget for viewing a StatsPage instance as a hyperlink"""
        a = widgets.Anchor()
        a.setLink(self.getPathTo(request, data))
        a.setText(data.wmfactory_title(request))
        return a

    def wvfactory_duration(self, request, node, data):
        """Given a duration in seconds, convert it to more appropriate units"""
        return widgets.Text(self.addTimeUnits(data.original))

    def wvfactory_if(self, request, node, data):
        """Hide the children of this widget if our model evaluates to False"""
        return Conditional(condition = data.original)

    def wvfactory_ifNot(self, request, node, data):
        """Hide the children of this widget if our model evaluates to True"""
        return Conditional(condition = not data.original)

    def wvfactory_fullDate(self, request, node, data):
        """Convert UNIX time in UTC to a complete date and time"""
        return widgets.Text(time.strftime("%c", time.gmtime(data.original)))

    def wvfactory_relativeDate(self, request, node, data):
        """Convert a UTC UNIX time in the past to an indication of how long ago it was"""
        return widgets.Text(self.addTimeUnits(time.time() - data.original))

    def wvfactory_message(self, request, node, data):
        """Use AutoFormatter to display a message in XHTML"""
        html = Message.AutoFormatter('xhtml').format(Message.Message(data.original))
        return widgets.RawText(html)


### The End ###
