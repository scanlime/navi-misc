""" PyUnicone.Controller

An abstraction for controller devices- both physical controllers attached
via event devices and virtual controllers we're emulating. All controllers
have some set of axes and buttons that can change state.

"""
#
# Universal Controller Emulator project
# Copyright (C) 2004 Micah Dowty
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

import os, sys, select, time
import LinuxInput, Event


class Actuator(object):
    """A representation of some part of the controller's state. This includes
       buttons, axes, and force feedback motors. The actuator has a value,
       usually a floating point number where 0 is neutral and None is unknown.
       """
    def __init__(self, name):
        self.name = name
        self._value = None
        Event.attach(self, "onChange")

    def __repr__(self):
        return "<%s %r=%r>" % (self.__class__.__name__, self.name, self._value)

    def _getValue(self):
        return self._value

    def _setValue(self, value):
        if value != self._value:
            self._value = value
            self.onChange(value)

    value = property(_getValue, _setValue)

    def mapTo(self, other):
        """Forward all changes from this actuator to the other one given,
           by adding an onChange observer that copies our value.
           Returns a reference to the observer function- this reference
           must remain held for the mapping to be preserved.
           """
        def mapper(value):
            other.value = value
        self.onChange.observe(mapper)
        return mapper


class Button(Actuator):
    """An actuator that can only take on the values 0 or 1"""
    pass

class Axis(Actuator):
    """An analog or digital axis, ranging between -1 and 1
       with the neutral position at 0.
       """
    pass

class RumbleMotor(Actuator):
    """An output actuator that controls a force feedback motor.
       A value between 0 and 1 may be used to set the intensity
       if supported, otherwise any nonzero value turns on the motor.
       """
    pass


class Joystick(Actuator):
    """An analog or digital joystick or d-pad, composed of two
       Axis objects. The X axis is positive when moved to the right,
       the Y axis is positive when moved downward.
       """
    def __init__(self, name, *axes):
        Actuator.__init__(self, name)
        self.axes = axes
        for axis in axes:
            axis.onChange.observe(self.axesChanged)

    def axesChanged(self, v):
        self.onChange(self.value)

    def _getValue(self):
        return tuple([axis.value for axis in self.axes])

    def _setValue(self, value):
        for i in xrange(len(value)):
            self.axes[i].value = value[i]

    value = property(_getValue, _setValue)


class Controller(object):
    """An abstract controller, with a set of named Actuator instances
       holding the controller's state. Rather than tracking changes
       on individual actuators, the entire controller is updated
       when it is sync'ed. Callbacks can be attached to this sync event.
       """
    def __init__(self, name):
        self.name = name
        self.actuators = {}
        Event.attach(self, "onSync")

    def __repr__(self):
        return "<%s %r>" % (self.__class__.__name__, self.name)

    def __getitem__(self, name):
        """Search for an actuator by name. Returns None if the
           actuator doesn't exist.
           """
        return self.actuators.get(name)

    def addActuator(self, a):
        self.actuators[a.name] = a

    def delActuator(self, a):
        del self.actuators[a.name]

    def findActuators(self, cls):
        for actuator in self.actuators.itervalues():
            if isinstance(actuator, cls):
                yield actuator


class EvdevController(Controller):
    """A controller, supplied with input by a Linux event device.
       On initialization, the device's axes, buttons, and other
       features are determined and mapped to Actuator instances.
       """

    def __init__(self, filename):
        self.filename = filename
        self.fd = os.open(filename, os.O_RDWR | os.O_NONBLOCK)
        Controller.__init__(self, LinuxInput.iocGetName(self.fd))
        Event.attach(self, "onDisconnect")

        # Store all the bitfields in a dictionary of lists
        self.evdevBits = {}
        for name in LinuxInput.iocGetBitNames(self.fd, None):
            try:
                self.evdevBits[name] = LinuxInput.iocGetBitNames(self.fd, name)
            except KeyError:
                pass

        # Store info on each absolute axis
        self.absAxisInfo = {}
        for name in self.evdevBits['EV_ABS']:
            self.absAxisInfo[name] = LinuxInput.iocGetAbs(self.fd, name)

        # Create actuators
        for bitName, cls in (
            ('EV_ABS', Axis),
            ('EV_REL', Axis),
            ('EV_KEY', Button),
            ):
            if bitName in self.evdevBits:
                for itemName in self.evdevBits[bitName]:
                    self.addActuator(cls(itemName))

        # Join pairs of axes into joysticks where appropriate
        for jsName, axisPrefix in (
            ('JS_ABS',       'ABS_'),
            ('JS_ABS_RIGHT', 'ABS_R'),
            ('JS_ABS_HAT0',  'ABS_HAT0'),
            ('JS_ABS_HAT1',  'ABS_HAT1'),
            ('JS_ABS_HAT2',  'ABS_HAT2'),
            ('JS_ABS_HAT3',  'ABS_HAT3'),
            ('JS_REL',       'REL_'),
            ):
            x, y = axisPrefix + 'X', axisPrefix + 'Y'
            if x in self.actuators and y in self.actuators:
                self.addActuator(Joystick(jsName, self.actuators[x], self.actuators[y]))

    def poll(self):
        """Receive and process all available input events. Returns True if any
           devices were sync'ed, false otherwise.
           """
        while 1:
            try:
                buffer = os.read(self.fd, LinuxInput.LinuxEvent.length)
            except OSError:
                # Blah, we have to detect a few different errors by their strings.
                e = str(sys.exc_info()[1])

                if e.startswith("[Errno 11]"):
                    # Resource temporarily unavailable.
                    # This is normal, we have nothing to read
                    return False

                if e.startswith("[Errno 19]"):
                    # No such device. We've been disconnected.
                    self.onDisconnect()
                    return False

                raise

            ev = LinuxInput.LinuxEvent(unpack=buffer)
            f = getattr(self, "handle_%s" % ev.type, None)
            if f:
                return f(ev)
            else:
                return False

    def handle_EV_KEY(self, ev):
        self[ev.code].value = bool(ev.value)
        return False

    def handle_EV_REL(self, ev):
        self[ev.code].value += ev.value
        return False

    def handle_EV_ABS(self, ev):
        """Scale the absolute axis into the range [-1, 1] using absAxisInfo"""
        try:
            info = self.absAxisInfo[ev.code]
        except KeyError:
            return False
        range = float(info['max'] - info['min'])
        self[ev.code].value = (ev.value - info['min']) / range * 2.0 - 1.0
        return False

    def handle_EV_SYN(self, ev):
        self.onSync()
        return True


class EvdevBus(object):
    """A bus holding all input devices currently attached. We have a
       blocking main loop that detects hotplugging, and polls all
       controllers as necessary. We have events for controller plugin/unplug,
       and for synchronizing all controllers simultanously after one main
       loop iteration.
       """
    def __init__(self, path="/dev/input"):
        self.path = path
        # Maps filename to EvdevController instance
        self.devices = {}
        Event.attach(self, 'onPlug', 'onUnplug', 'onSync')

    def scan(self):
        """Look for newly attached devices"""
        for file in os.listdir(self.path):
            if not file.startswith("event"):
                continue
            path = os.path.join(self.path, file)
            if path in self.devices:
                continue
            try:
                dev = EvdevController(path)
            except:
                continue
            self._attachDevice(dev)

    def poll(self, timeout=None):
        """Service activity on all input devices. Invokes onSync if any
           devices had a synchronization.
           """
        sync = False
        devs = self.devices.values()
        select.select([dev.fd for dev in devs], [], [], timeout)
        for dev in devs:
            sync |= dev.poll()
        if sync:
            self.onSync()

    def mainLoop(self, scanInterval=1.0):
        """Run continuously, looking for hotplugging and for events
           on existing input devices. Scans for new devices every scanInterval
           seconds- this may be a floating point number.
           """
        nextScanAt = time.time()
        while 1:
            now = time.time()
            if nextScanAt <= now:
                self.scan()
                nextScanAt += scanInterval
            self.poll(max(0, nextScanAt - now))

    def _attachDevice(self, dev):
        dev.onDisconnect.strongObserve(lambda: self._detachDevice(dev))
        self.devices[dev.filename] = dev
        self.onPlug(dev)

    def _detachDevice(self, dev):
        try:
            del self.devices[dev.filename]
        except KeyError:
            return
        self.onUnplug(dev)

### The End ###
