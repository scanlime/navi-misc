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
        self.actuators[name] = value

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
        Controller.__init__(self, self._getDeviceName())
        self._readDeviceInfo()

    def _getDeviceName(self):
        return ioctl(self.fd, EVIOCGNAME_512, "\0"*512)

    def _readDeviceInfo(self):
        # Absolute axes
        absmap = linux_input.Event.codeMaps['EV_ABS']
        buffer = "\0" * struct.calcsize("iiiii")
        self.absAxisInfo = {}
        for name, number in absmap.nameMap.iteritems():
            values = struct.unpack("iiiii", ioctl(self.fd, EVIOCGABS_512 + number, buffer))
            values = dict(zip(( 'value', 'min', 'max', 'fuzz', 'flat' ),values))
            self.absAxisInfo[name] = values

    def poll(self):
        """Receive and process all available input events"""
        while 1:
            try:
                buffer = os.read(self.fd, self.packetSize)
            except OSError:
                return
            self.update(Event(unpack=buffer))

        

### The End ###
