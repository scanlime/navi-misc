""" BZFlag.Instrument

Instruments for measuring and displaying quantities
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

import Numeric, time
from BZFlag.UI import HUD, Layout, GLText


class RingBuffer:
    """Utility class implementing a ring buffer using a Numeric array.
       A ring buffer is an efficient way of implementing a fixed-size FIFO queue.
       This implementation defines the 'head' index as the first empty slot, and
       the 'tail' index as the oldest entry in the buffer.
       """
    def __init__(self, size, typecode=Numeric.Float):
        # We actually create an array larger than our maximum size by 1,
        # so that it's easier to detect when we overflow.
        self.buffer = Numeric.zeros(size+1, typecode)
        self.empty()

    def empty(self):
        self.head = self.tail = 0

    def push(self, x):
        """Add a value to the head of the queue"""
        self.buffer[self.head] = x
        self.head += 1
        self.head %= len(self.buffer)
        if self.head == self.tail:
            # We just overflowed. Discard the oldest value in the buffer
            self.tail += 1
            self.tail %= len(self.buffer)

    def pop(self):
        """Remove the value at the tail of the queue"""
        if self.tail == self.head:
            raise Exception("The RingBuffer is empty")
        self.tail += 1
        self.tail %= len(self.buffer)

    def peek(self):
        """Return the value at the tail of the queue"""
        if self.tail == self.head:
            raise Exception("The RingBuffer is empty")
        return self.buffer[self.tail]

    def __len__(self):
        return (self.head - self.tail) % len(self.buffer)


class FrequencyCounter:
    """Measure the frequency at which a specified event occurs, using a sliding
       window low-pass filter. The filter will always be sized between filterMinSamples
       and filterMaxSamples. If the filterMaxSamples limit isn't hit, it will be hold
       samples for the last filterSeconds of data.

       The value of the counter can be read at any time from the 'value' attribute, in Hz.
       If the filter hasn't collected filterMinSamples samples yet, value will be None.

       An alternate time() function can be provided. The default is to use time.time(),
       but you may want to use Timekeeper's time() function to measure in game time rather
       than in wallclock time.
       """
    def __init__(self, event,
                 filterMinSamples = 32,
                 filterMaxSamples = 1024,
                 filterSeconds    = 2,
                 timeFunction     = None):
        self.filterMinSamples = filterMinSamples
        self.filterSeconds = filterSeconds

        # Allow overriding our Timekeeper if necessary
        if not timeFunction:
            timeFunction = time.time
        self.time = timeFunction

        # Our filter is a Numeric array configured as a ring buffer, to minimize the
        # overhead of adding and removing samples at the ends.
        self.filter = RingBuffer(filterMaxSamples)

        self.reset()
        event.observe(self.trigger)

    def reset(self):
        """Reset the filter window. Our filterWindow attribute is a list of event sample times."""
        self.value = None
        self.filter.empty()

    def trigger(self, *args, **kw):
        """Take a new sample, clear out any old samples, and calculate a new frequency value"""
        now = self.time()
        self.filter.push(now)
        while len(self.filter) > self.filterMinSamples and (now - self.filter.peek()) > self.filterSeconds:
            self.filter.pop()
        if len(self.filter) >= self.filterMinSamples:
            self.value = (len(self.filter) - 1) / (now - self.filter.peek())


class TextInstrument(HUD.Text):
    """Abstract base class that attaches to a viewport and overlays a text readout on it"""
    def __init__(self, viewport):
        self.mainViewport = viewport
        margin = GLText.defaultSize / 2
        HUD.Text.__init__(self, viewport.region(Layout.Rect(viewport).margin(margin)),
                          shadow       = True,
                          shadowOffset = 1)


class FrameRate(TextInstrument):
    """An instrument that measures and displays the given viewport's frame rate"""
    def __init__(self, viewport, **kw):
        self.counter = FrequencyCounter(viewport.onFinishFrame)
        TextInstrument.__init__(self, viewport, **kw)

    def getText(self):
        if self.counter.value:
            return "FHz: %.01f" % self.counter.value
        else:
            return "Calculating frame rate..."

### The End ###
