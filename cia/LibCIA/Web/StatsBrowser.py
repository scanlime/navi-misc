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

from twisted.web import static
from twisted.web.woven import page, widgets
import os, urllib


def pathSplit(s):
    """Given a path as a string, return it split into directories,
       ignoring leading and trailing slashes or multiple slashes.
       """
    return [i for i in s.split('/') if i]


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
        print selfPath, upLevels, down
        while selfPath and down and selfPath[0] == down[0]:
            del selfPath[0]
            del down[0]
            upLevels -= 1
        return '/'.join(['..'] * upLevels + down + [''])

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

    def wvfactory_statsLink(self, request, node, data):
        """Create a widget for viewing a StatsPage instance as a hyperlink"""
        a = widgets.Anchor()
        a.setLink(self.getPathTo(request, data))
        a.setText(data.wmfactory_title(request))
        return a

### The End ###
