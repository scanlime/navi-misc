""" BZEngine.UI.Viewport.Base

An abstract base class defining the Viewport interface
"""
#
# Python BZEngine Package
# Copyright (C) 2003-2004 Micah Dowty <micahjd@users.sourceforge.net>
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

from BZEngine import Event
import copy
from weakref import ReferenceError

__all__ = ('Viewport', 'RegionMixin')


class Viewport(object):
    """Abstract base class for a viewport. Defines events and methods
       that all viewports must support.
       """
    def __init__(self, eventLoop, size=None):
        self.eventLoop = eventLoop
        Event.attach(self, 'onFrame', 'onResize',
                     'onSetupFrame', 'onDrawFrame', 'onFinishFrame',
                     'onRenderChild_before', 'onRenderChild_after')

        # This makes it easier for RegionMixin to override the parent and rootView
        # while still having it set correctly for show() below.
        if not hasattr(self, 'parent'):
            self.parent = None
            self.rootView = self

        self.rect = None
        self.rectExp = None
        self.size = None
        if size is not None:
            self.setRect((0,0) + size)

        self.renderSequence = [
            self.onRenderChild_before,
            self.onSetupFrame,
            self.onDrawFrame,
            self.onRenderChild_after,
            self.onFinishFrame,
            ]

        self.visible = False
        self.show()

    def initRegion(self):
        """An initialization hook that gets run for viewport regions.
           Note that initialization added to __init__ does not get run for regions.
           """
        pass

    def show(self):
        """Make this viewport visible. the 'visible' attribute must be treated as
           read-only. This implementation simply sets it, but viewport regions have
           a much more complex procedure.
           """
        self.visible = True

    def hide(self):
        self.visible = False

    def render(self):
        if self.visible and self.size[0] > 0 and self.size[1] > 0:
            for f in self.renderSequence:
                try:
                    f()
                except ReferenceError:
                    # Ignore parts of our renderSequence that were referenced weakly and garbage collected
                    pass

    def setRect(self, rect):
        """Set this viewport's size and position rectangle.
           The provided rectangle can optionally be a function called to
           calculate the viewport's rectangle any time updateRect is called.
           """
        if callable(rect):
            self.rectExp = rect
        else:
            self.rectExp = lambda: rect
        self.updateRect()

    def updateRect(self):
        """Actually resize the viewport, calculating a rectangle using the
           expression stored by setRect().
           """
        newRect = self.rectExp()
        if newRect != self.rect:
            self.rect = newRect
            if self.parent:
                self.rect = (self.rect[0] + self.parent.rect[0],
                             self.rect[1] + self.parent.rect[1],
                             self.rect[2],
                             self.rect[3])
            self.size = self.rect[2:]
            self.onResize()

    def setCaption(self, title):
        """Set the window caption on the viewport, if applicable"""
        pass

    def region(self, rect, renderLink='after'):
        """Return a new Viewport that identifies the given rectangular subviewport.
           As in setRect, the rectangle can also be a function that returns a viewport,
           to be reevaluated when a parent viewport changes size.

           renderLink controls how this region is linked into its parent's rendering
           sequence. 'after' inserts it after all other entries, 'before' before all
           others, None doesn't insert it.
           """
        class ViewportRegion(RegionMixin, self.__class__):
            pass
        return ViewportRegion(self, rect, renderLink)


class RegionMixin(Viewport):
    """A mix-in class that implements sub-viewports created with the region() method.
       This class may perform extra implementation-specific per-region setup.

       parent     - The viewport who had its region() called to create this region
       rect       - A 4-tuple specifying the x, y, width, and height coordinates of this region,
                    relative to the parent.
       renderLink - Specifies how this viewport is inserted into its parent's rendering sequence.
                    'after' inserts it at the end of the rendering sequence, 'before' inserts it
                    at the beginning, None doesn't insert it at all. These names correspond with
                    the onRenderChild_* events.
       """
    def __init__(self, parent, rect, renderLink):
        self.renderLink = renderLink
        self.parent = parent
        self.rootView = parent.rootView

        # The render event we're currently connected to, so we can hide() properly
        self.currentRenderEvent = None

        # Note that we call Viewport's init, rather than our direct subclass.
        # We don't want to initialize the viewport's underlying video device again.
        Viewport.__init__(self, parent.eventLoop)
        self.setRect(rect)

        # Allow region-specific setup from the viewport subclasses
        self.initRegion()

    def setCaption(self, title):
        """Viewport regions have nowhere to show a caption"""
        pass

    def show(self):
        """For viewport regions, this attaches the viewport into the parent's render cycle."""
        if self.visible:
            return
        self.visible = True

        # Update our rectangle when the parent's changes
        self.parent.onResize.observe(self.updateRect)

        # Attach ourselves to the proper onRenderChild handler
        if self.renderLink:
            self.currentRenderEvent = getattr(self.parent, 'onRenderChild_%s' % self.renderLink)
            self.currentRenderEvent.observe(self.render)

    def hide(self):
        """Make the viewport invisible. Invisible viewport regions don't take any time
           during the render cycle, whereas visible viewports always have some overhead
           even if they have no handlers attached.
           """
        if not self.visible:
            return
        self.visible = False

        # Stop updating our parent rectangle
        self.parent.onResize.unobserve(self.updateRect)

        # Remove us from the parent's render sequence
        if self.currentRenderEvent:
            self.currentRenderEvent.unobserve(self.render)
            self.currentRenderEvent = None

### The End ###
