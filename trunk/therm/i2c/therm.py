#!/usr/bin/env python

from __future__ import division
import time, re


def toFahrenheit(celcius):
        """Return the therm's value in degrees fahrenheit rather than the default degrees celcius"""
        return celcius * 9.0 / 5 + 32


def getTherms(file="therms.conf"):
    """Reads a config file with lines of the form "bus:address description"
       and returns a list of Therm instances.
       Supports extra whitespace and comment lines starting with '#'
       """
    therms = []
    for line in open(file).xreadlines():
        line = line.strip()
        if not line:
            continue
        if line[0] == '#':
            continue
        location, id, description = re.split("\s+", line, 2)
        bus, address = location.split(":")
        therms.append(Therm(int(bus), int(address), id, description))
    return therms


class Therm:
    """A description of a thermometer's bus location and its purpose,
       and a place for readings to be stored. Note that the address
       here is not the full I2C address, it is the number from 0 to 7
       printed on the TC74's package.

       'id' is a short identifier string used to uniquely identify
       this sensor, even if the address or description changes.

       The current temperature at this therm is stored in the 'value'
       attribute as degrees celsius. It will be None if no reading has
       been taken or this therm is disconnected.
       """
    def __init__(self, bus, address, id, description=None):
        self.bus = bus
        self.address = address
	self.id = id
        self.description = description
        self.value = None
        self.avgTotal = 0
        self.avgSamples = 0

    def __repr__(self):
        if self.value is None:
            data = "no reading"
        else:
            data = "%.02f F" % toFahrenheit(self.value)
        return '<Therm %r (%r) at %s:%s, %s>' % (
            self.id, self.description, self.bus, self.address, data)

    def update(self, newValue):
        """Set a new current reading for this therm. newValue may be None to indicate
           that data is no longer available.
           """
        self.value = newValue
        if newValue is not None:
            self.avgTotal += newValue
            self.avgSamples += 1

    def getAverage(self):
        """Read the average value since the last time getAverage was called"""
        if self.avgSamples > 0:
            avg = self.avgTotal / self.avgSamples
            self.avgTotal = 0
            self.avgSamples = 0
            return avg


class ThermSampler:
    """Given a list of Therm instances that need updating, determines which addresses
       need to be scanned on the given i2c bus and updates the therms.
       """
    def __init__(self):
        import pyrcpod, i2c
        self.rcpod = pyrcpod.devices[0].open()
        self.mbus = i2c.MultiBus(self.rcpod)
        self.mtherm = MultiTherm(self.mbus)

    def update(self, therms):
        """Scan for available readings on the given list of thermometers, and update
           them as necessary.
           """
        # Determine which addresses need to be scanned. There's no point in determining
        # which busses are in use, since all busses are scanned simultaneously for free.
        addrs = {}
        for therm in therms:
            if not addrs.has_key(therm.address):
                addrs[therm.address] = []
            addrs[therm.address].append(therm)

        # For each address in use, scan the whole bus
        for addr, addrTherms in addrs.items():
            temps = self.mtherm.readTemp(addr | 0x48)
            
            # File away our temperature readings into the proper therm devices.
            # Not the most efficient method, but who cares :)
            for addrTherm in addrTherms:
                for busNum, temp in temps.items():
                    if addrTherm.bus == busNum:

                        # If our temperature reading is 255, the thermometer is
                        # disconnected (the bus's idle state is 1, and the devices
                        # can't produce readings that high)
                        if temp == 255:
                            addrTherm.update(None)
                        else:
                            addrTherm.update(temp)


class MultiTherm:
    """A set of Microchip TC74 thermometers connected to the given I2C MultiBus,
       all read simultaneously.
       """
    def __init__(self, i2c):
        self.i2c = i2c

    def write(self, address, register, byte):
        """Send an I2C command to write the given byte to the given TC74 register.
           Sends identical data to this device address on all busses.
           """
        self.i2c.start()
        self.i2c.writebyte(address << 1)			# address, r/w bit is 0 (write)
        self.i2c.writebyte(register)
        self.i2c.writebyte(byte)
        self.i2c.stop()

    def read(self, address, register):
        """Send an I2C command to read the given TC74 register.
           Returns a list of the results received from each I2C bus.
           """
        self.i2c.start()
        self.i2c.writebyte(address << 1)			# address, r/w bit is 0 (write)
        self.i2c.writebyte(register)
        return self.receive(address)

    def receive(self, address):
        """Read the value of the previously addressed TC74 register,
           returns a list of the results received from each I2C bus.
           """
        self.i2c.start()
        self.i2c.writebyte(address << 1 | 0x01)                 # address, r/w bit is 1 (read)
        a = self.i2c.readbyte(address)
        self.i2c.stop()
        return a

    def readTemp(self, address):
        """Poll for a new temperature reading to become available from devices with the given address
           on any I2C bus, and return it in degrees C.
           Returns a dictionary mapping bus number to temperature reading, since any number of devices
           may report that they have data available.
           """
        temps = {}            

        # Take the therms out of standby mode
        self.write(address, 0x01, 0x00)

        # Poll until at least one bus has data available
        while not temps:
            configs = self.read(address, 0x01)
            for busNum in xrange(len(configs)):
                # Does this bus's device have data available?
                if (configs[busNum] >> 6) & 0x1:
                    # Put a placeholder value in the temps map,
                    # so we know there's data ready when we read from
                    # all busses below
                    temps[busNum] = None
            # Don't suck up all the CPU polling
            time.sleep(0.05)

        # Read all busses simultaneously, return only the available data
        readings = self.read(address, 0x00)
        for busNum in temps.keys():
            # Sign extend the 8-bit 2's complement values
            if readings[busNum] & 0x80:
                temps[busNum] = readings[busNum] - 256
            else:
                temps[busNum] = readings[busNum]

        return temps

