"""
LaserObjects.py

Non-GUI objects for communicating with and calibrating my laser projector.
Copyright (c) 2008 Micah Dowty

   Permission is hereby granted, free of charge, to any person
   obtaining a copy of this software and associated documentation
   files (the "Software"), to deal in the Software without
   restriction, including without limitation the rights to use, copy,
   modify, merge, publish, distribute, sublicense, and/or sell copies
   of the Software, and to permit persons to whom the Software is
   furnished to do so, subject to the following conditions:

   The above copyright notice and this permission notice shall be
   included in all copies or substantial portions of the Software.

   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
   EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
   MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
   NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
   BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
   ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
   CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
   SOFTWARE.

"""

from __future__ import division

import struct

__all__ = ['ObservableValue', 'AdjustableValue', 'DependentValue', 'ScaledValue',
           'ThermValue', 'BTConnector']


class ObservableValue(object):
    """Simple abstraction for any mutable value that can be observed."""
    def __init__(self, value=None):
        self.value = value
        self.observers = []

    def set(self, value, omitFn=None):
        """Set the value, and notify all observers.  Optionally this
           may omit an observer, so that the caller can avoid getting
           notified about its own actions.
           """
        if value != self.value:
            self.value = value
            for fn in self.observers:
                # Must be "!=", instead of "is", since we want comparisons
                # between equivalent but not identical bound method objects
                # to succeed.
                if fn != omitFn:
                    fn(value)

    def observe(self, fn):
        """Start observing this value. Calls the provided function
           once, synchronously, to set an initial value.
           """
        fn(self.value)
        self.observers.append(fn)


class AdjustableValue(ObservableValue):
    """Abstraction for a numeric value which can be modified and
       observed.  AdjustableValues have a static minimum/maximum value
       and a dynamic current value. Any AdjustableValue can legally
       have None as its current value, indicating that no data is
       available.
       """
    def __init__(self, value=None, min=None, max=None):
        self.min = min
        self.max = max
        ObservableValue.__init__(self, value)

    def getUnitValue(self):
        """Scale 'value' into the range [0,1], where 0 is 'min' and 1 is 'max'."""
        if self.value is not None:
            return (self.value - self.min) / (self.max - self.min)

    def setUnitValue(self, v, omitFn=None):
        """Wrapper around 'set', to scale from [0,1] to [min,max]"""
        if v is not None:
            v = v * (self.max - self.min) + self.min
        self.set(v, omitFn)


class DependentValue(AdjustableValue):
    """An AdjustableValue which is connected to another via a pair of
       functions. origToDep() transforms the original value to this
       new dependent value, and depToOrig is its inverse.

       The min/max values may optionally be overridden. If not, they
       will be calculated by transforming the original max/min via
       the origToDep() function.
       """
    def __init__(self, original, origToDep, depToOrig,
                 min=None, max=None):
        self.original = original
        self._origToDep = origToDep
        self._depToOrig = depToOrig

        if min is None:
            min = self.origToDep(original.min)
        if max is None:
            max = self.origToDep(original.max)

        AdjustableValue.__init__(self, self.origToDep(original.value), min, max)

        # Set up observers without getting an initial callback.
        self.observers.append(self._updateOrig)
        self.original.observers.append(self._updateDep)

    def origToDep(self, v):
        if v is not None:
            return self._origToDep(v)

    def depToOrig(self, v):
        if v is not None:
            return self._depToOrig(v)

    def _updateOrig(self, v):
        self.original.set(self.depToOrig(v), self._updateDep)

    def _updateDep(self, v):
        self.set(self.origToDep(v), self._updateOrig)


class ScaledValue(DependentValue):
    """A DependentValue that simply scales another AdjustableValue."""
    def __init__(self, original, scale, min=None, max=None):
        DependentValue.__init__(self, original,
                                lambda x: x * scale,
                                lambda x: x / scale,
                                min, max)


class ThermValue(DependentValue):
    """A DependentValue that converts a raw temperature sensor reading to
       fahrenheit or celsius.
       """
    def __init__(self, original, toFahrenheit=True):
        if toFahrenheit:
            DependentValue.__init__(self, original,
                                    lambda x: x / 16 * 1.8 + 32,
                                    lambda x: (x - 32) / 1.8 * 16)
        else:
            DependentValue.__init__(self, original,
                                    lambda x: x / 16,
                                    lambda x: x * 16)


class BTConnector(object):
    """Link an AdjustableValue with a value accessible via a BluetoothConduit.
       By default, this is a mostly one-way connector: We read the value from
       hardware on connect, then it's write-only.

       For a polling read-only or read-mostly variant of BTConnector,
       see the PollingBTConnector in LaserWidgets.py.
       """
    def __init__(self, value, bt, regionName, offset=0, format="<I"):
        self.bt = bt
        self.value = value
        self.regionName = regionName
        self.offset = offset
        self.format = format
        
        # Observe without an immediate callback
        self.value.observers.append(self._valueChanged)

        # Asynchronously get the hardware's state, and set up callbacks.
        self.bt.addCallbacks(self.onConnect, self.onDisconnect)

    def onConnect(self):
        # Read the current value
        self.bt.setRegionByName(self.regionName)
        self.bt.seek(self.offset)
        self.bt.read(1, self.onRead)

    def onDisconnect(self):
        self.value.set(None, self._valueChanged)

    def onRead(self, data):
        v = struct.unpack(self.format, data)[0]
        self.value.set(v, self._valueChanged)

    def _valueChanged(self, v):
        self.bt.setRegionByName(self.regionName)
        self.bt.seek(self.offset)
        self.bt.write(struct.pack(self.format, int(v + 0.5)))

