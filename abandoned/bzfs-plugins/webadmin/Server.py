"""Server

General classes for the web server. Muchly borrowed from Micah Dowty's CIA code
"""
#
# Copyright (C) 2005 David Trowbridge
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License
# as published by the Free Software Foundation; either version 2
# of the License, or (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
#

from twisted.web import server, static
from twisted.python import log

class File (static.File):
    """A local subclass of static.File that overrides the default directory
       listing behavior.
    """
    def listNames (self):
        """Override listNames to hide hidden files, like .svn and .xvpics"""
        listing = static.File.listNames(self)
        return [item for item in listing if not item.startswith (".")]

    def directoryListing (self):
        """Use our own directory lister rather than relying on Twisted's default.
           This lets us keep the site's look more consistent, and doesn't pull
           in all of Woven just for a silly listing page.
        """
        return ServerPages.DirectoryListing (self)

class Component:
    """A component is some top-level area of the site that is explicitly
       assigned a URL and may be visible to the user in some sort of
       site-wide navigation system. Every component must have a root URL
       and a root resource- indeed, the major reason Components exist is
       to give a subsystem a way to bind itself to a subset of a site's URL
       space.
    """
    # The component's URL, will be set by the Site
    url = None

    # The component's resource, will be set by the component's constructor.
    resource = None

    # The component's user-visible name, if it has one
    name = None

    def __contains__ (self, page):
        """Subclasses must implement this to test whether a page
           belongs to this component.
        """
        return False

class StaticJoiner (File):
    """This web page acts mostly like a static.File, and all children
       are files under the given directory- however this page itself
       renders the provided 'index' page. This can be used to create
       a dynamically generated front page that references static pages
       or images as its children.
    """
    def __init__ (self, path, indexPage,
                  defaultType = "text/plain",
                  ignoredExts = (),
                  registry    = None,
                  allowExt    = 0):
        self.indexPage = indexPage
        static.File.__init__ (self, path, defaultType, ignoredExts, registry, allowExt)

    def getChild (self, path, request):
        if path:
            return static.File.getChild (self, path, request)
        else:
            return self

    def render (self, request):
        return self.indexPage.render (request)

    def createSimilarFile (self, path):
        f = File (path, self.defaultType, self.ignoredExts, self.registry)
        f.processors = self.processors
        f.indexNames = self.indexNames[:]
        return f

class Site (server.Site):
    """A twisted.web.server.Site subclass, that might go away"""

    def __init__ (self, resource):
        self.components = []
        server.Site.__init__ (self, resource)

    def putComponent (self, childName, component):
        """Install the given component instance at 'childName'"""
        component.url = '/' + childName
        self.resource.putChild (childName, component.resource)
        self.components.append (component)
