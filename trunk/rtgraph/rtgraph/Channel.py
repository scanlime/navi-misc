""" rtgraph.Channel

An abstraction for channels of data that vary over time, optionally
specifying drawing style.
"""
#
# rtgtaph real-time graphing package for PyGTK
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

from __future__ import generators
import colorsys
import time

__all__ = ["Channel", "FunctionChannel", "TimeVariable"]


class Channel(object):
    """An analog reading that varies over time, and information about drawing the channel.
       The 'value' property gets and sets this channel's value, updating its timestamp
       when set.
       """
    def __init__(self, value=None, color=(0,0,1), name=None):
        self._value = value
        self._timestamp = None
        self.timeMap = {}
        self.name = name
        self.setColor(color)

    def getValue(self):
        """Retrieves the current value, automatically called when accessing the 'value' property"""
        return self._value

    def setValue(self, value, timestamp=None):
        """Set the current value, by default giving it the current time as a timestamp"""
        if timestamp is None:
            timestamp = time.time()
        self._value = value
        self._timestamp = timestamp


    # Create wrappers around getValue/setValue so if subclasses override them,
    # the property still points to them properly.
    def _getValue(self):
        return self.getValue()
    def _setValue(self, value):
        return self.setValue(value)
    value = property(_getValue, _setValue)

    def getTimestamp(self):
        """Return the time at which this channel was last updated, to determine
           if there is new data available. Returns None if the value has not been set.
           """
        return self._timestamp

    def getColor(self):
        """Returns a color to graph this channel in, as a (red,green,blue) tuple with
           values between 0 and 1.
           """
        return self._color

    def setColor(self, color):
        """Change this channel's color, invalidating our GC cache"""
        self._color = tuple(color)
        self.gcMap = {}

    color = property(getColor, setColor)

    def getStyle(self, widget):
        """Returns a dictionary of keyword arguments to use when constructing a GC
           to draw this channel in.
           """
        color = self.getColor()
        gdkColor = widget.get_colormap().alloc_color(*[int(c * 65535) for c in color])
        return {'foreground': gdkColor}

    def getGC(self, widget):
        """For the given widget, return a GC for drawing this channel. Caches the GCs
           in our gcMap.
           """
        try:
            return self.gcMap[widget]
        except KeyError:
            gc = widget.window.new_gc(**self.getStyle(widget))
            self.gcMap[widget] = gc
            return gc

    def hasChanged(self, obj):
        """Determine whether this channel has changed, from the point of view of 'obj'.
           This returns True only once for a given value of 'obj' after this channel's
           timestamp has changed.
           """
        if not self.timeMap.has_key(obj):
            # obj has never seen this channel yet
            return True

        if self.timeMap[obj] == self._timestamp:
            return False
        else:
            self.timeMap[obj] = self._timestamp
            return True

    def __str__(self):
        """Returns the channel's name, if it has one, or the repr() otherwise"""
        if self.name is not None:
            return self.name
        else:
            return repr(self)

    def autoColor(self, i=None):
        """Automatically color this channel, returning an iterator that can be
           passed to autoColor instead of None to continue coloring other channels.
           For example:

              i = None
              for channel in channels:
                  i = channel.autoColor(i)
           """
        # If we don't have an iterator from before, start a fresh one
        if i is None:
            i = self.colorGenerator()
        self.color = i.next()
        return i

    def colorGenerator(self):
        """A generator that indefinitely yields color values to use when
           autocoloring channels.
           """
        while True:
            for luma in (0.9, 0.5):
                for hue in (0.66, 0, 0.33, 0.75):
                    yield colorsys.hsv_to_rgb(hue, 1, luma)

    def strValue(self):
        """Get a string representation of the current value. This works
           well on floats, tuples, and strings, but if you need other
           representations it's easy to override this in a Channel subclass.
           """
        return limitFloatPrecision(self.value, 4)


def limitFloatPrecision(v, d):
    """Return a string representation of v with at most d digits of precision.
       Operates recursively on tuples/lists, and uses repr() for other data types.
       """
    if type(v) == float:
        return ("%%.0%df" % d) % v
    elif type(v) == tuple:
        return "(%s)" % ", ".join([limitFloatPrecision(i, d) for i in v])
    elif type(v) == list:
        return "[%s]" % ", ".join([limitFloatPrecision(i, d) for i in v])
    else:
        return repr(v)


class FunctionChannel(Channel):
    """A channel that evaluates the given function.

       fString : The function to evaluate, as a string
       name    : The name to give this channel, None to use the function string
       modules : Modules to import the full contents of into globals
       globals : Extra items to place in the function's global namespace
       locals  : Extra items to place in the function's local namespace
       vars    : A dict of callables that are called and placed in the function's
                 global namespace before each evaluation.
       """
    def __init__(self,
                 fString,
                 name     = None,
                 modules  = ('math', 'random'),
                 globals  = {},
                 locals   = {},
                 vars     = {}
                 ):
        if name is None:
            name = fString
        self.globals = dict(globals)
        self.locals = dict(locals)
        self.vars = dict(vars)
        self.fString = fString
        Channel.__init__(self, name=fString)

        # Import the specified modules into globals
        for module in modules:
            self.globals.update(__import__(module).__dict__)

    def getValue(self):
        # Evaluate vars
        for var, callable in self.vars.iteritems():
            self.globals[var] = callable()

        return eval(self.fString, self.globals, self.locals)


class TimeVariable:
    """A callable class that returns the number of seconds since it was created.
       This is helpful for use with FunctionChannel's variables feature.
       """
    def __init__(self):
        self.tInitial = time.time()

    def __call__(self):
        return time.time() - self.tInitial

### The End ###
