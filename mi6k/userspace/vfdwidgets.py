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
    """A simple fixed-size text widget, with no scrolling, aligned somehow
       in its allocated rectangle. The default is for it to be centered.
       """
    def __init__(self, text='', gravity=(-1, -1), align=(0.5, 0.5), priority=1):
        Widget.__init__(self)
        self.text = text
        self.align = align
        self.gravity = gravity
        self.priority = priority

    def setText(self, text):
        self._text = text
        self.textChanged()

    def getText(self):
        return self._text

    text = property(getText, setText)

    def textChanged(self):
        self._lines = self._text.split("\n")
        w = 0
        for line in self._lines:
            w = max(w, len(line))
        self.minWidth = w
        self.height = len(self._lines)

    def draw(self, width, height):
        buffer = []
        x = int((width - self.minWidth) * self.align[0] + 0.5)
        y = int((height - self.height) * self.align[1] + 0.5)

        buffer.extend([" "*width] * y)
        for line in self._lines[:height]:
            if len(line) > width:
                buffer.append(line[:width])
            else:
                buffer.append(" "*x + line + " "*(width-len(line)-x))
        while len(buffer) < height:
            buffer.append(" "*width)
        return buffer


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
        self.totalMinWidth += widget.minWidth
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
        self.widgets.sort(lambda a,b: cmp(a.gravity[0], a.gravity[1]))

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

        # Lay out the unbiased widgets using this extra space
        if rNeighbour:
            pad, remaining = remaining.splitRight(1)
        for widget in unbiased:
            if lNeighbour:
                pad, remaining = remaining.splitLeft(1)
            width = widget.minWidth + extraWidth * widget.sizeWeight // totalWeight
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

    def draw(self):
        """Ask each widget to draw itself and copy them into their
           rectangles on our surface buffer. layout() must have already
           been run since the last change to the widget lists or to
           widget sizes or priorities.
           """
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
    s.layout()
    s.show()

### The End ###
