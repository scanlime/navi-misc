""" vfdwidgets

This is a simple text layout system for small character cell displays
like VFDs and LCDs. It's part of the MI6K project, but it's a self-contained
module that could be useful elsewhere.

The two fundamental entities in this layout system are Widgets and Surfaces.
A Widget displays some bit of information. It may have a fixed size, or it may
grow or shrink to fit its environment. A Surface is a fixed-size area widgets
can be displayed in.

Widgets have a 'gravity' which controls which side of the Surface they 'stick'
to, and a priority that controls which widgets are hidden if space becomes scarce.
"""
#
# Media Infrawidget 6000
# Copyright (C) 2004 Micah Dowty <micah@navi.cx>
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
import time

__all__ = ["Widget", "Surface", "Text", "LoopingScroller"]


class Widget(object):
    """Abstract base class for all widgets. Subclasses must override
       at least 'draw', and many of this class' attributes should be
       modified for useful layout behaviour.
       """

    gravity = (0, 0)
    """The quadrant of this vector determines which side or corner of the
       surface this widget sticks to. The magnitude in each axis controls
       the placement of this widget relative to others.

       A widget with gravity (0, 1) will stick to the bottom of the surface weakly,
       a widget with gravity (-100, 100) will have a very strong attraction to the
       bottom-left corner.

       Vertical gravity always takes precedence over horizontal gravity. A
       widget with gravity (-100, 1) will always be at least a line below a widget
       with gravity (0, 0). Widgets with different Y gravity will always be on different
       lines, whereas widgets with the same Y gravity won't necessarily be on the same
       line.
       """

    visible = True
    height = 1
    minWidth = 0
    sizeWeight = 1

    priority = 1

    def draw(self, width, height):
        """Return an array of 'height' strings, each 'width' characters long.
           The widget is responsible for adjusting itself to the given size
           in the best way possible.
           """
        raise NotImplementedError

    def update(self, dt):
        """Animated widgets override this to update their state. 'dt' is the
           delta time since the last update, in seconds.
           """
        pass


class Text(Widget):
    """A fixed-size text widget with no animation, aligned somehow
       in its allocated rectangle. The default is for it to be centered.
       """
    def __init__(self,
                 text       = '',
                 gravity    = (-1, -1),
                 align      = (0.5, 0.5),
                 priority   = 1,
                 background = ' ',
                 ellipses   = '',
                 visible    = True,
                 ):
        Widget.__init__(self)
        self.text = text
        self.align = align
        self.gravity = gravity
        self.priority = priority
        self.background = background
        self.ellipses = ellipses
        self.visible = visible
        self.offset = (0, 0)

    def setText(self, text):
        self._text = text
        self.textChanged(text.split('\n'))

    def getText(self):
        return self._text

    text = property(getText, setText)

    def textChanged(self, lines):
        self._lines = lines
        w = 0
        for line in lines:
            w = max(w, len(line))
        self.minWidth = w
        self.height = len(lines)

    def getDrawableLines(self, width, height):
        """Return the actual line buffer used in draw(). Subclasses may override
           this to do layout specific to the actual size we're drawing at.
           """
        return self._lines

    def draw(self, width, height):
        buffer = []
        lines = self.getDrawableLines(width, height)

        # Offset our text according to its alignment and our current scrolling offset
        x = max(0, int((width - self.minWidth) * self.align[0] + 0.5)) + self.offset[0]
        y = max(0, int((height - self.height) * self.align[1] + 0.5)) + self.offset[1]

        # Pad the top
        if y > 0:
            buffer.extend([self.background*width] * y)
        for line in lines[:height]:
            # Truncate the top
            if y < 0:
                y += 1
                continue

            # Apply the horizontal offset
            if x > 0:
                line = self.background*x + line
            elif x < 0:
                line = self.ellipses + line[len(self.ellipses) - x:]

            # Truncate or pad each line horizontally
            if len(line) > width:
                line = line[:width - len(self.ellipses)] + self.ellipses
                buffer.append(line)
            else:
                buffer.append(line + self.background*(width-len(line)))
	    assert len(buffer[-1]) == width

        # Truncate the bottom
        buffer = buffer[:height]

        # Pad the bottom
        while len(buffer) < height:
            buffer.append(self.background*width)

	assert len(buffer) == height
        return buffer


class LoopingScroller(Text):
    """Text which scrolls forward automatically, looping around on itself"""
    def __init__(self,
                 text          = '',
                 gravity       = (0, -1),
                 align         = (0,0),
                 priority      = 1,
                 background    = ' ',
                 padding       = ' '*5,
                 pauseDuration = 1,
                 scrollRate    = 10,
                 visible       = True,
                 ):
        Text.__init__(self, text, gravity, align, priority,
                      background, visible=visible)
        self.padding = padding
        self.pauseDuration = pauseDuration
        self.scrollRate = scrollRate

        self.offset = (0,0)
        self.scroll = 0
        self.pauseRemaining = self.pauseDuration

    def textChanged(self, lines):
        Text.textChanged(self, lines)
        self.fullWidth = self.minWidth
        self.minWidth = 0

    def getDrawableLines(self, width, height):
        if self.fullWidth > width:
            self.offset = (int(-self.scroll), 0)
            return [line + self.padding + line for line in self._lines]
        else:
            self.offset = (0,0)
            return self._lines

    def update(self, dt):
        self.pauseRemaining -= dt
        if self.pauseRemaining > 0:
            return
        dt = -self.pauseRemaining
        self.pauseRemaining = 0

        self.scroll += dt * self.scrollRate
        wrapPoint = self.fullWidth + len(self.padding)
        if self.scroll >= wrapPoint:
            self.scroll -= wrapPoint
            self.pauseRemaining += self.pauseDuration


class Rect:
    """Represents a rectangle by the location of its top-left corner and its size"""
    def __init__(self, x, y, width, height):
        self.x = x
        self.y = y
        self.width = width
        self.height = height

    def __repr__(self):
        return "<Rect %r %r %r %r>" % (self.x, self.y, self.width, self.height)

    def splitLeft(self, amount):
        """Split a rectangle off of the left side of this one,
           returning a (left, remaining) tuple.
           """
        return (Rect(self.x, self.y, amount, self.height),
                Rect(self.x + amount, self.y, self.width - amount, self.height))

    def splitRight(self, amount):
        """Split a rectangle off of the right side of this one,
           returning a (right, remaining) tuple.
           """
        return (Rect(self.x + self.width - amount, self.y, amount, self.height),
                Rect(self.x, self.y, self.width - amount, self.height))

    def splitTop(self, amount):
        """Split a rectangle off of the top side of this one,
           returning a (top, remaining) tuple.
           """
        return (Rect(self.x, self.y, self.width, amount),
                Rect(self.x, self.y + amount, self.width, self.height - amount))

    def splitBottom(self, amount):
        """Split a rectangle off of the bottom side of this one,
           returning a (bottom, remaining) tuple.
           """
        return (Rect(self.x, self.y + self.height - amount, self.width, amount),
                Rect(self.x, self.y, self.width, self.height - amount))


class LayoutLine:
    """Contains a number of widgets that all share the same Y gravity.
       This line collectively has a height and Y padding determined
       from the contained widgets.
       """
    def __init__(self, maxWidth, yGravity):
        self.maxWidth = maxWidth
        self.yGravity = yGravity
        self.totalMinWidth = 0
        self.height = 0
        self.widgets = []

    def add(self, widget):
        """Add the given widget to this layout line, returning True
           if we succeed or False if this line is full.
           """
        self.totalMinWidth += min(widget.minWidth, self.maxWidth)
        if self.totalMinWidth + len(self.widgets) > self.maxWidth:
            return False
        self.widgets.append(widget)
        self.height = max(self.height, widget.height)
        return True

    def layout(self, widgetRects):
        """Lay out all widgets on this line, storing the finished
           widget rectangles in the provided dict. self.rect must
           already contain the initial rectangle for this line.
           """
        remaining = self.rect
        self.widgets.sort(lambda a,b: cmp(a.gravity[0], b.gravity[0]))

        # Lay out all left-biased widgets
        lNeighbour = None
        for widget in self.widgets:
            if widget.gravity[0] < 0:
                if lNeighbour:
                    pad, remaining = remaining.splitLeft(1)
                wrect, remaining = remaining.splitLeft(widget.minWidth)
                lNeighbour = widget
                widgetRects[widget] = wrect

        # Now reverse the list and do this for right-biased widgets
        rNeighbour = None
        self.widgets.reverse()
        for widget in self.widgets:
            if widget.gravity[0] > 0:
                if rNeighbour:
                    pad, remaining = remaining.splitRight(1)
                wrect, remaining = remaining.splitRight(widget.minWidth)
                rNeighbour = widget
                widgetRects[widget] = wrect

        # Filter out all biased widgets while figuring out how much extra width we have
        unbiased = []
        extraWidth = remaining.width
        totalWeight = 0
        for widget in self.widgets:
            if widget.gravity[0] == 0:
                unbiased.append(widget)
                extraWidth -= widget.minWidth
                totalWeight += widget.sizeWeight

        # Account for padding
        extraWidth -= len(unbiased)-1
	if lNeighbour:
	    extraWidth -= 1
	if rNeighbour:
	    extraWidth -= 1

        # Lay out the unbiased widgets using this extra space
	if rNeighbour:
            pad, remaining = remaining.splitRight(1)
        for widget in unbiased:
            if lNeighbour:
                pad, remaining = remaining.splitLeft(1)
            width = widget.minWidth + int(extraWidth * widget.sizeWeight / totalWeight)
            wrect, remaining = remaining.splitLeft(width)
            lNeighbour = widget
            widgetRects[widget] = wrect


class Surface(object):
    """A fixed-size character array where widgets can be placed"""
    def __init__(self, width, height, widgets=(), background=" "):
        self.width = width
        self.height = height
        self.widgets = list(widgets)
        self.background = background
        self.widgetRects = {}
        self.lastTime = None

        self.lifetimes = []
        self.layoutRequired = True

    def add(self, widget, lifetime=None):
        """Add a new widget to the surface, optionally with a limited
           lifetime given in seconds.
           """
        self.widgets.append(widget)
        if lifetime is not None:
            self.lifetimes.append([lifetime, widget])
        self.layoutRequired = True

    def remove(self, widget):
        self.widgets.remove(widget)
        self.layoutRequired = True

    def update(self, dt=None):
        """Update all widgets with the given time step. This calculates
           its own time step if one isn't provided.
           """
        if dt is None:
            now = time.time()
            if self.lastTime is None:
                self.lastTime = now
                return
            else:
                dt = now - self.lastTime
                self.lastTime = now

        # Update the lifetimes of widgets that can expire
        i = 0
        while i < len(self.lifetimes):
            self.lifetimes[i][0] -= dt
            if self.lifetimes[i][0] > 0:
                i += 1
            else:
                try:
                    self.widgets.remove(self.lifetimes[i][1])
                    self.layoutRequired = True
                except ValueError:
                    pass
                del self.lifetimes[i]

        # Perform individual widget updates
        for widget in self.widgets:
            widget.update(dt)

    def draw(self):
        """Ask each widget to draw itself and copy them into their
           rectangles on our surface buffer. If self.layoutRequired is set,
           this also performs a layout before drawing.
           """
        if self.layoutRequired:
            self.layout()

        buffer = [self.background * self.width] * self.height
        for widget, rect in self.widgetRects.iteritems():
            w = widget.draw(rect.width, rect.height)
            for y in xrange(rect.height):
                line = buffer[rect.y + y]
                buffer[rect.y + y] = line[:rect.x] + w[y] + line[rect.x + rect.width:]
        return buffer

    def show(self):
        """Mostly for debugging, this will draw() the surface then
           display it to stdout.
           """
        print "+" + "-"*self.width + "+"
        for line in self.draw():
            print "|"+line+"|"
        print "+" + "-"*self.width + "+"

    def layout(self):
        """Examine our list of widgets and find a place for as many as
           we can on the surface. Afterwards, self.widgetRects will
           contain a mapping from each visible widget to its Rect.
           Normally this doesn't need to be called directly, since
           draw() will layout first if the layoutRequired flag is set.
           """
        # We always remove widgets in priority order, but we can place
        # them in any order since they get sorted again later.
        toRemove = list(self.widgets)
        toRemove.sort(lambda a,b: cmp(a.priority, b.priority))
        toPlace = [widget for widget in self.widgets if widget.visible]

        # Attempt the layout, removing a widget each time it fails
        while True:
            result = self.layoutIteration(toPlace)
            if result is True:
                return
            elif result is False:
                # Vertical failure, remove the lowest priority widget no matter what
                toPlace.remove(toRemove[0])
                del toRemove[0]
            else:
                # Horizontal failure, remove one of the suggested widgets, starting
                # with the lowest priorities
                for w in toRemove:
                    if w in result:
                        toPlace.remove(w)
                        toRemove.remove(w)
                        break

        self.layoutRequired = False

    def layoutIteration(self, widgets):
        """Attempt to lay out all given widgets. Returns True on success.
           If we have a horizontal placement error this returns a list of
           failed widgets. If we have a vertical placement error, returns False.
           """
        unplaceable = []

        # Divide all widgets into layout lines according to their Y gravity
        layoutLines = {}
        for widget in widgets:
            try:
                line = layoutLines[widget.gravity[1]]
            except KeyError:
                line = LayoutLine(self.width, widget.gravity[1])
                layoutLines[widget.gravity[1]] = line
            if not line.add(widget):
                unplaceable.append(widget)

        if unplaceable:
            return unplaceable

        # See if we ran out of vertical space
        totalHeight = 0
        for line in layoutLines.itervalues():
            totalHeight += line.height
        if totalHeight > self.height:
            return False

        # Sort the layout lines, and assign each a rectangle
        lineKeys = layoutLines.keys()
        lineKeys.sort()
        remaining = Rect(0, 0, self.width, self.height)
        for y in lineKeys:
            line = layoutLines[y]
            if y < 0:
                line.rect, remaining = remaining.splitTop(line.height)
        lineKeys.reverse()
        for y in lineKeys:
            line = layoutLines[y]
            if y > 0:
                line.rect, remaining = remaining.splitBottom(line.height)
        if 0 in lineKeys:
            layoutLines[0].rect = remaining

        # Lay out the widgets on each line
        self.widgetRects = {}
        for line in lineKeys:
            layoutLines[line].layout(self.widgetRects)
        return True


if __name__ == "__main__":
    w = [Text("Wgt%d" % i) for i in xrange(5)]
    w[0].gravity = (0,-1)
    w[1].gravity = (1,1)
    w[2].gravity = (1,2)
    w[3].gravity = (1,2)
    w[4].gravity = (-1,2)
    s = Surface(20, 5, w)
    s.show()

### The End ###
