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

class File(static.File):
    """A local subclass of static.File that overrides the default directory
       listing behavior.
    """
    def listNames(self):
        """Override listNames to hide hidden files, like .svn and .xvpics"""
        listing = static.File.listNames(self)
            return [item for item in listing if not item.startswith(".")]

    def directoryListing(self):
        """Use our own directory lister rather than relying on Twisted's default.
           This lets us keep the site's look more consistent, and doesn't pull
           in all of Woven just for a silly listing page.
        """
        return ServerPages.DirectoryListing(self)

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

    def __contains__(self, page):
        """Subclasses must implement this to test whether a page
           belongs to this component.
        """
        return False

class Site (server.Site):
    """A twisted.web.server.Site subclass, to use our modified Request class"""
    requestFactory = Request

    def __init__ (self, resource):
        self.components = []
        server.Site.__init__ (self, resource)

    def putComponent (self, childName, component):
        """Install the given component instance at 'childName'"""
        component.url = '/' + childName
        self.resource.putChild(childName, component.resource)
        self.components.append(component)
