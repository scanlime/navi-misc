""" pyunicone.controller

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

import os
import linux_input


class Actuator(object):
    """A representation of some part of the controller's state. This includes
       buttons, axes, and force feedback motors. The actuator has a value,
       usually a floating point number where 0 is neutral and None is unknown.
       Delegates can optionally be added, to be notified when this value changes.
       """
    def __init__(self, name):
        self.name = name
        self._value = None
        self.delegates = []

    def _getValue(self):
        return self._value

    def _setValue(self, value):
        self._value = value
        for f in self.delegates:
            f()

    value = property(_getValue, _setValue)


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


class Joystick(object):
    """An analog or digital joystick or d-pad, composed of two
       Axis objects. The X axis is positive when moved to the right,
       the Y axis is positive when moved downward.
       """
    def __init__(self, name, *axes):
        self.name = name
        self.axes = axes

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
        self.delegates = []

    def __getitem__(self, name):
        """Search for an actuator value by name. Returns None if the
           actuator doesn't exist or it has an undefined value.
           """
        a = self.actuators.get(name)
        if a:
            return a.value

    def __setitem__(self, name, value):
        """Set an actuator value by name. Throws a KeyError if the
           named actuator doesn't exist.
           """
        self.actuators[name].value = value

    def addActuator(self, a):
        self.actuators[a.name] = a

    def delActuator(self, a):
        del self.actuators[a.name]

    def sync(self):
        """Called when all actuators on this controller have finished
           updating. This calls all our delegates, to let them know
           we're done updating.
           """
        for f in self.delegates:
            f()


class EvdevController(Controller):
    """A controller, supplied with input by a Linux event device.
       On initialization, the device's axes, buttons, and other
       features are determined and mapped to Actuator instances.
       """

    def __init__(self, filename):
        self.fd = os.open(filename, os.O_RDWR | os.O_NONBLOCK)
        Controller.__init__(self, linux_input.iocGetName(self.fd))

        # Store all the bitfields in a dictionary of lists
        self.evdevBits = {}
        for name in linux_input.iocGetBitNames(self.fd, None):
            try:
                self.evdevBits[name] = linux_input.iocGetBitNames(self.fd, name)
            except KeyError:
                pass

        # Store info on each absolute axis
        self.absAxisInfo = {}
        for name in self.evdevBits['EV_ABS']:
            self.absAxisInfo = linux_input.iocGetAbs(self.fd, name)

        # Create actuators
        for bitName, cls in (
            ('EV_ABS', Axis),
            ('EV_REL', Axis),
            ('EV_KEY', Button),
            ):
            if bitName in self.evdevBits:
                for itemName in self.evdevBits[bitName]:
                    self.addActuator(cls(itemName))

    def poll(self):
        """Receive and process all available input events"""
        while 1:
            try:
                buffer = os.read(self.fd, linux_input.Event.length)
            except OSError:
                return
            ev = linux_input.Event(unpack=buffer)
            f = getattr(self, "handle_%s" % ev.type, None)
            if f:
                f(ev)

    def handle_EV_KEY(self, ev):
        self[ev.code] = bool(ev.value)

    def handle_EV_REL(self, ev):
        self[ev.code] = self[ev.code] + ev.value

    def handle_EV_ABS(self, ev):
        """Scale the absolute axis into the range [-1, 1] using absAxisInfo"""
        try:
            info = self.absAxisInfo[ev.code]
        except KeyError:
            return
        range = float(info['max'] - info['min'])
        self[ev.code] = (ev.value - info['min']) / range * 2.0 - 1.0

    def handle_EV_SYN(self, ev):
        self.sync()


### The End ###
