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
import os, time


class CountersModel(model.MethodModel):
    """A Model representing a set of event counters, constructed
       around a Stats.Counters
       """
    def submodelCheck(self, request, name):
        return True

    def submodelFactory(self, request, name):
        return CounterModel(self.original.getCounter(name))


class CounterModel(model.MethodModel):
    """A Model representing one counter, wrapping a dictionary
       of that counter's values.
       """
    def submodelCheck(self, request, name):
        return True

    def submodelFactory(self, request, name):
        return 'foo'
        return self.original.get(name, 0)


class RecentMessagesModel(model.MethodModel):
    """A Model wrapping a StatsTarget's recentMesssages attribute
       (a LogDB instance). Submodels are integers representing how many
       messages to list. Submodels of those integers are lists of
       messages in XML.
       """
    
    def submodelCheck(self, request, name):
        return True

    def submodelFactory(self, request, name):
        if self.original:
            return model.ListModel(self.original.getLatest(int(name)))
        return model.ListModel([])


class StatsChildrenModel(model.MethodModel):
    """Wraps a StatsPage, providing access to its child pages"""
    def submodelCheck(self, request, name):
        return True

    def submodelFactory(self, request, name):
        return self.original.getDynamicChild(name)


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
    """A Woven view representing one StatsTarget"""

    templateFile = "stats_browser.xhtml"
    templateDirectory = os.path.split(os.path.abspath(__file__))[0]

    def initialize(self, caps=None, target=None, storage=None):
        self.caps = caps
        self.target = target
        if storage:
            self.target = storage.getRoot()
        self.storage = self.target.storage

    def getDynamicChild(self, name, request=None):
        return StatsPage(caps   = self.caps,
                         target = self.target.child(name))

    def submodelCheck(self, request, name):
        """The default implementation of this chokes when name is None"""
        return name and hasattr(self, "wmfactory_"+name)

    def getPathTo(self, request, destination):
        """Assuming this is the page requested, return a relative URI to the given page"""
        return 'boing'

    def getNodeModel(self, request, node, submodel):
        """Override the default getNodeModel so that nodes we can't find
           turn into None rather than causing an exception. This makes
           our 'conditional' widget work.
           """
        try:
            page.Page.getNodeModel(self, request, node, submodel)
        except:
            return None


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
        return self.target.metadata

    def wmfactory_title(self, requeset):
        """Return the human-readable title of this stats target. This
           is loaded from the 'title' metadata item if that exists, otherwise
           it's an un-URI-encoded version of the last item in our path.
           """
        return self.target.getTitle()

    def wmfactory_counters(self, request):
        """Return a CountersModel instance that can be used to access
           the event counters stored at this stats path.
           """
        return CountersModel(self.target.counters)

    def wmfactory_recentMessages(self, request):
        """Returns a model that can be used to return the 'n' most recent
           messages delivered to this stats target.
           """
        return RecentMessagesModel(self.target.recentMessages)

    def wmfactory_currentTime(self, request):
        """Always returns the current time"""
        return time.time()

    def wmfactory_root(self, request):
        """Returns the root StatsPage"""
        return StatsPage(caps    = self.caps,
                         storage = self.storage)

    def wmfactory_child(self, request):
        """Returns a model representing the children of this StatsPage.
           Each submodel of the returned model looks for a child with that name.
           """
        return StatsChildrenModel(self)


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
        return Conditional(condition = data and data.original)

    def wvfactory_ifNot(self, request, node, data):
        """Hide the children of this widget if our model evaluates to True"""
        return Conditional(condition = not (data and data.original))

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
