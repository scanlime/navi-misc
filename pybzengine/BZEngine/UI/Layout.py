""" BZFlag.UI.Layout

A collection of callable classes to provide lazy-evaluating
layout operators. This is intended to be used along with Viewport's
region() method, to create HUDs and menus that resize automatically.

All layout operators' inputs can be static rectangles, viewports
(in which case their size is used to create a rectangle), or other
callable layout operators.

All parameters can either be specified in pixels as integers,
or in fractions of the input rectangle's relevant axis, as floats,
or as a callable that takes the rectangle in question as a parameter.
"""
#
# Python BZFlag Package
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

from BZFlag.UI import Viewport
from __future__ import nested_scopes


def autoScale(rect, seq, axisIndex=2):
    """Performs the automatic conversion from scale factors
       to pixels mentioned in the module's documentation.
       Integer values pass straight through, floating point
       values are multiplied by the rectangle's minor axis.
       Callables are called with the rectangle in question.
       """
    result = []
    for item in seq:
        if type(item) == float:
            result.append(item * rect[axisIndex])
        elif callable(item):
            result.append(item(rect))
        else:
            result.append(item)
    return result


class Rect:
    """A lazy-evaluating layout rectangle class. Evaluate by calling.
       This class can be initialized from a Viewport instance, a
       sequence with 4 items, or a callable.
       """
    def __init__(self, value):
        if isinstance(value, Viewport.Viewport):
            self.__call__ = lambda: [0,0] + list(value.size)
        elif callable(value):
            self.__call__ = value
        else:
            self.__call__ = lambda: value

    def margin(self, *margin):
        """Remove the given margin from the edges of self.
           margin is specified CSS-style- it can be either
           a scalar for a margin to remove from all sides,
           a 2-tuple for vertical and horizontal margin,
           respectively, or a 4-tuple for margin on the
           top, right, bottom, and left sides, respectively.
           """
        if len(margin) == 2:
            (vertical, horizontal) = margin
            margin = (vertical, horizontal, vertical, horizontal)
        elif len(margin) == 1:
            margin *= 4

        def fMargin():
            r = self()
            (top, right, bottom, left) = autoScale(r, margin)
            return (r[0] + left,
                    r[1] + bottom,
                    r[2] - left - right,
                    r[3] - bottom - top)
        return Rect(fMargin)

    def bottom(self, height):
        """Split the rectangle horizontally at the given height from the bottom,
           in pixels or as a scale factor, returning the bottom rectangle.
           """
        def fBottom():
            r = self()
            h = autoScale(r, (height,), 3)[0]
            if h > r[3]:
                h = r[3]
            return (r[0],
                    r[1],
                    r[2],
                    h)
        return Rect(fBottom)

    def top(self, height):
        """Split the rectangle horizontally at the given height from the top,
           in pixels or as a scale factor, returning the top rectangle.
           """
        def fTop():
            r = self()
            h = autoScale(r, (height,), 3)[0]
            if h > r[3]:
                h = r[3]
            return (r[0],
                    r[1] + r[3] - h,
                    r[2],
                    h)
        return Rect(fTop)

    def left(self, width):
        """Split the rectangle vertically at the given width from the left side,
           in pixels or as a scale factor, returning the left rectangle.
           """
        def fLeft():
            r = self()
            w = autoScale(r, (width,))[0]
            if w > r[2]:
                w = r[2]
            return (r[0],
                    r[1],
                    w,
                    r[3])
        return Rect(fLeft)

    def right(self, width):
        """Split the rectangle vertically at the given width from the right side,
           in pixels or as a scale factor, returning the right rectangle.
           """
        def fRight():
            r = self()
            w = autoScale(r, (width,))[0]
            if w > r[2]:
                w = r[2]
            return (r[0] + r[2] - w,
                    r[1],
                    w,
                    r[3])
        return Rect(fRight)

    def hSplit(self, height):
        """Given the height of the top section, split the rectangle horizontally
           into top and bottom sections.
           """
        def fBottom():
            r = self()
            h = autoScale(r, (height,), 3)[0]
            if h > r[3]:
                h = r[3]
            return (r[0],
                    r[1],
                    r[2],
                    r[3] - h)
        def fTop():
            r = self()
            h = autoScale(r, (height,), 3)[0]
            if h > r[3]:
                h = r[3]
            return (r[0],
                    r[1] + r[3] - h,
                    r[2],
                    h)
        return (Rect(fTop), Rect(fBottom))

    def vSplit(self, width):
        """Given the width of the left section, split the rectangle vertically
           into left and right sections.
           """
        def fLeft():
            r = self()
            w = autoScale(r, (width,))[0]
            if w > r[2]:
                w = r[2]
            return (r[0],
                    r[1],
                    w,
                    r[3])
        def fRight():
            r = self()
            w = autoScale(r, (width,))[0]
            if w > r[2]:
                w = r[2]
            return (r[0] + w,
                    r[1],
                    r[2] - w,
                    r[3])
        return (Rect(fLeft), Rect(fRight))

### The End ###
