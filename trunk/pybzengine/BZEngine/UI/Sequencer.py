""" BZEngine.UI.Sequencer

Objects to represent interactive animations abstractly and sequence
their activity, optionally with timers and transitions.

A basic animation is represented as a Page, several of which can be
stored in a Book. There are special Pages that wrap other pages and
define transitions or other modifiers.
"""
#
# Python BZEngine Package
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

from BZEngine import Event, Animated


class Page:
    """A collection of scene objects, event handlers, and other objects forming an
       'interactive video clip' with a definite beginning and end. These objects
       are put into a Book along with other Pages.

       A page is active as long as it exists in memory- between when its constructor
       finishes and when its finalize() member is called.

       The Book manages instantiating and deleting Pages. A page signals that it's
       job is finished by triggering its onFinish event.
       """
    def __init__(self, view):
        Event.attach(self, "onFinish")
        self.viewport = view.viewport
        self.view = view
        self.time = Animated.Timekeeper()

    def finalize(self):
        """Optional hook to perform any deinitialization the Page needs"""
        pass


class Book(Page):
    """A collection of Pages. A Book is responsible for controlling the overal flow
       of control in a program built from multiple Pages. The page at the beginning
       of the Book is always the active page. Pages are removed from
       the Book when their onFinish event is triggered. Pages can be added either at
       the beginning or at the end of the book- pages added at the beginning are made
       active immediately, and the formerly active page resumes when that page finishes.
       Pages added at the end are activated after all other pages have finished.

       Note that since pages are only instantiated when active, pages specified for
       insertion into the Book are given as a (class, parameters) tuple. The 'view'
       parameter is assumed, and is not given in the parameters list. If no parameters
       are needed, the class can be given bare rather than as part of a tuple.

       Also note that a book is, itself, a page. This is a convenient way to group
       pages hierarchially.
       """
    def __init__(self, view, initialPages):
        Page.__init__(self, view)
        self.pages = initialPages
        self.activeInstance = None
        self.activeSpec = None
        self.evaluatePages()

    def pushFront(self, pages):
        """Add the given list of pages to the front of the book. The first page
           given will become active.
           """
        self.pages = pages + self.pages
        self.evaluatePages()

    def pushBack(self, pages):
        """Add the given list of pages to the back of the book"""
        self.pages = self.pages + pages
        self.evaluatePages()

    def popFront(self):
        """Remove the currently active page. This is called automatically
           by a page's onFinish handler.
           """
        del self.pages[0]
        self.evaluatePages()

    def evaluatePages(self):
        """Used internally, this looks at the list of pages and determines if the active
           page needs to be changed. If so, the new active page is instantiated and the
           reference to the old active page is lost, terminating it.
           """
        if self.pages:
            # If the currently active page wasn't created from the spec at the front of the page list,
            # it should no longer be active. Make it so.
            if self.activeSpec is not self.pages[0]:
                if self.activeInstance:
                    self.activeInstance.finalize()
                self.activeSpec = None
                self.activeInstance = None

                # Transform the item at the front of self.pages into a pageClass and pageParameters.
                # This accounts for the format described in this class's doc string- a page specification
                # may be a bare class or it may be a (class, parameters) tuple.
                try:
                    pageClass, pageParameters = self.pages[0]
                except TypeError:
                    pageClass = self.pages[0]
                    pageParameters = ()

                # Create the new active page instance
                self.activeSpec = self.pages[0]
                self.activeInstance = pageClass(self.view, *pageParameters)

                # Set up an observer on the page's onFinish handler that removes it from the book
                self.activeInstance.onFinish.observe(self.popFront)
        else:
            # No more pages, disable any currently active page and call our onFinish event
            if self.activeInstance:
                self.activeInstance.finalize()
            self.activeSpec = None
            self.activeInstance = None
            self.onFinish()

### The End ###
