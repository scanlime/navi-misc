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

from __future__ import division
from BZEngine import Event, Animated
from BZEngine.UI import GLOrtho


class Page:
    """A collection of scene objects, event handlers, and other objects forming an
       'interactive video clip' with a definite beginning and end. These objects
       are put into a Book along with other Pages.

       A page is active as long as it exists in memory- between when its constructor
       finishes and when its finalize() member is called.

       The Book manages instantiating and deleting Pages. A page signals that it's
       job is finished by triggering its onFinish event.
       """
    def __init__(self, book):
        Event.attach(self, "onFinish")
        self.viewport = book.viewport
        self.view = book.view
        self.time = Animated.Timekeeper()

    def finalize(self):
        """Optional hook to perform any deinitialization the Page needs"""
        pass


class PageWrapper(Page):
    """A page that acts as a wrapper around another, adding extra functionality"""
    def __init__(self, book, subpage):
        Page.__init__(self, book)
        self.subpage = subpage

        # Link the subpage's onFinish to our onFinish such that if it quits, we do too
        self.subpage.onFinish.observe(self.onFinish)

    def finalize(self):
        """Give our subpage a chance to clean up"""
        self.subpage.finalize()


class Book:
    """A collection of Pages. A Book is responsible for controlling the overal flow
       of control in a program built from multiple Pages. The page at the beginning
       of the Book is always the active page. Pages are removed from
       the Book when their onFinish event is triggered. Pages can be added either at
       the beginning or at the end of the book- pages added at the beginning are made
       active immediately, and the formerly active page resumes when that page finishes.
       Pages added at the end are activated after all other pages have finished.

       Note that since pages are only instantiated when active, pages specified for
       insertion into the Book are given as a callable with a single 'book' parameter.
       If the class doesn't take any extra parameters, this can simply be a class.
       Otherwise, it is convenient for this to be a lambda expression to instantiate
       the class properly.
       """
    def __init__(self, view, initialPages):
        Event.attach(self, "onFinish")
        self.view = view
        self.viewport = view.viewport
        self.pages = initialPages
        self.activeInstance = None
        self.activeClass = None
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
            # If the currently active page wasn't created from the class at the front of the page list,
            # it should no longer be active. Make it so.
            if self.activeClass is not self.pages[0]:
                if self.activeInstance:
                    self.activeInstance.finalize()
                self.activeClass = None
                self.activeInstance = None

                # Create the new active page instance
                self.activeClass = self.pages[0]
                self.activeInstance = self.activeClass(self)

                # Set up an observer on the page's onFinish handler that removes it from the book
                self.activeInstance.onFinish.observe(self.popFront)
        else:
            # No more pages, disable any currently active page and call our onFinish event
            if self.activeInstance:
                self.activeInstance.finalize()
            self.activeClass = None
            self.activeInstance = None
            self.onFinish()


class PageTimerWrapper(PageWrapper):
    """A page wrapper that calls onFinish after the page has been active for a
       fixed amount of time.
       """
    def __init__(self, book, subpage, duration):
        PageWrapper.__init__(self, book, subpage)
        self.duration = duration
        self.viewport.onSetupFrame.observe(self.setupFrame)

    def setupFrame(self):
        # There are other methods we could use to decide when to quit, but this
        # one is most robust when nonstandard Timekeepers are involved, as is
        # the case when recording movies.
        self.duration -= self.time.step()
        if self.duration <= 0:
            self.onFinish()


def PageTimer(duration, page):
    """Given a page class or a callable, return a callable for creating that page
       with a PageTimerWrapper around it.
       """
    return lambda book: PageTimerWrapper(book, page(book), duration)


class FadeInWrapper(PageWrapper):
    """A page wrapper that fades in any page from a given solid color over the given duration."""
    def __init__(self, book, subpage, duration, color):
        PageWrapper.__init__(self, book, subpage)
        self.rate = 1/duration
        self.color = list(color[:3]) + [1]
        self.overlay = self.viewport.region(self.viewport.rect)
        self.overlay.fov = None
        self.overlay.onDrawFrame.observe(self.drawFrame)

    def drawFrame(self):
        # Draw a rectangle of the color 'self.color' over the entire screen
        GLOrtho.setup()
        GLOrtho.setColor(*self.color)
        GLOrtho.filledRect(self.overlay.size)

        # Fade out the overlay's alpha. When it hits zero, delete our overlay viewport,
        # causing this function to not be called any longer.
        self.color[3] -= self.rate * self.time.step()
        if self.color[3] <= 0:
            self.overlay = None


def FadeIn(duration, color, page):
    """Given a page class or a callable, return a callable for creating that page
       with a FadeInWrapper around it.
       """
    return lambda book: FadeInWrapper(book, page(book), duration, color)


class FadeOutWrapper(PageWrapper):
    """A page wrapper that fades in any page to a solid color over the given
       duration after that page's onFinish has been called.
       """
    def __init__(self, book, subpage, duration, color):
        PageWrapper.__init__(self, book, subpage)
        self.rate = 1/duration
        self.color = list(color[:3]) + [0]
        self.overlay = None
        subpage.onFinish.replace(self.start)

    def start(self):
        if not self.overlay:
            self.overlay = self.viewport.region(self.viewport.rect)
            self.overlay.fov = None
            self.overlay.onDrawFrame.observe(self.drawFrame)

    def drawFrame(self):
        # Draw a rectangle of the color 'self.color' over the entire screen
        GLOrtho.setup()
        GLOrtho.setColor(*self.color)
        GLOrtho.filledRect(self.overlay.size)

        # Fade in the overlay's alpha. When it hits one, delete our overlay viewport,
        # causing this function to not be called any longer, and call our own onFinish.
        self.color[3] += self.rate * self.time.step()
        if self.color[3] > 1:
            self.overlay = None
            self.onFinish()


def FadeOut(duration, color, page):
    """Given a page class or a callable, return a callable for creating that page
       with a FadeOutWrapper around it.
       """
    return lambda book: FadeOutWrapper(book, page(book), duration, color)


def PageInterrupter(events, page):
    """Returns a factory function that creates the given page then attaches
       its onFinish event to all events in the given list. This makes it easy
       to specify a list of events that will automatically terminate the given
       page if it's active. All parameters given by the events are ignored.
       """
    def factory(book):
        # Actually instantiate the page
        p = page(book)

        # Wrapper to ignore all arguments from the event
        def finish(*args, **kw):
            p.onFinish()

            # We're using strong references so we need to explicitly unobserve
            for event in events:
                event.unobserve(finish)

        # Set up all observers. Use strong references, so they won't be
        # garbage collected as soon as this function returns.
        for event in events:
            event.strongObserve(finish)
        return p
    return factory


def UserPageInterrupter(page):
    """Wraps a page such that it can be interrupted by user interaction
       (a key or mouse button press)
       """
    def factory(book):
        events = [
            book.viewport.onKeyDown,
            book.viewport.onMouseButtonDown,
            ]
        return PageInterrupter(events, page)(book)
    return factory

### The End ###
