#
# Python library for accessing an electric field sensor box
# Copyright (C) 2003  Micah Dowty <micah@navi.picogui.org>
#
#    Requires the 'serial' and Numeric modules.
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

import serial, time, Numeric


platePairs = [ (0,4),
               (1,4),
               (2,4),
               (3,4),
               (0,5),
               (1,5),
               (2,5),
               (3,5) ]

platePositions = [ ( 0.8, 1.2, 0.0),
                   (-0.8, 1.2, 0.0),
                   (-0.8,-1.2, 0.0),
                   ( 0.8,-1.2, 0.0),
                   ( 1.0, 0.0, 0.0),
                   (-1.0, 0.0, 0.0) ]

colors = [ (1.0,0.8,0.6,0.9),
           (1.0,0.7,0.6,0.9),
           (1.0,0.6,0.6,0.9),
           (1.0,0.5,0.6,0.9),
           (0.6,0.8,1.0,0.9),
           (0.6,0.7,1.0,0.9),
           (0.6,0.6,1.0,0.9),
           (0.6,0.5,1.0,0.9),
           ]


class sensor:
    "Low-level class to connect to the sensor board and read packets"
    
    def __init__(self, serialPort=None):
        if not serialPort:
            serialPort = 0
        self.port = serial.Serial(
            port = serialPort,
            baudrate = 19200,
            stopbits = serial.STOPBITS_ONE,
            timeout = 2.0)
        self.buffer = ''
        self.reset()

    def reset(self):
        # Reset the board
        self.port.setRTS(0)
        self.port.setDTR(1)
        time.sleep(0.005)
        self.port.setDTR(0)

        # Get WLoader to start our app
        self.sendSlowly('0000hg');

    def sendSlowly(self, str):
        for chr in str:
            time.sleep(0.05)
            self.port.write(chr)

    def read(self):
        while True:
            # Synchronize to 0x80 synchronization byte
            while self.port.read() != chr(0x80):
                pass

            rawPacket = self.port.read(16)
            theirChecksum = ord(self.port.read())

            # Verify the checksum byte
            ourChecksum = 0
            for c in rawPacket:
                ourChecksum += ord(c)
            ourChecksum %= 256
            if ourChecksum == theirChecksum:
                cookedPacket = Numeric.fromstring(rawPacket, Numeric.UInt16).byteswapped()
                return cookedPacket / 65535.0


class filter:
    "Sliding window filter for smoothing out sensor readings"
    def __init__(self, sensor, filterLen=None):
        if not filterLen:
            filterLen = 32
        self.filterBuffer = []
        self.filterSum = Numeric.zeros(8)
        self.filterLen = filterLen
        self.sensor = sensor
        for i in range(0,filterLen):
            reading = self.sensor.read()
            self.filterBuffer.append(reading)
            self.filterSum = self.filterSum + reading

    def read(self):
        reading = self.sensor.read()
        self.filterBuffer.append(reading)
        self.filterSum += reading
        self.filterSum -= self.filterBuffer[0]
        self.filterBuffer = self.filterBuffer[1:]
        return self.filterSum / self.filterLen


class baselineRelative:
    "Takes a baseline reading and reports relative to that"
    def __init__(self, sensor):
        self.sensor = sensor
        self.resetBaseline()

    def resetBaseline(self, baselineSamples=128):
        self.baseline = self.sensor.read()
        for i in range(0, baselineSamples-1):
            self.baseline += self.sensor.read()
        self.baseline /= baselineSamples

    def read(self):
        return self.sensor.read() - self.baseline

