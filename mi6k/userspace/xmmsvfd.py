#!/usr/bin/env python
#
# A copy of the VFD code from the LED Board v2 project, hacked up a bit
# to work with the USB VFD controller as a real-world test application.
#

import xmms, time, os
from fcntl import ioctl
from Numeric import *

class CenturyVFD:
    width = 20
    lines = 2
    userDefinedCharacters = "\xF6\xF7\xF8\xF9\xFA\xFB\xFC\xFD\xFE\xFF"

    def __init__(self):
        self.dev = open("/dev/usb/mi6k0", "w")
        self.powerOn()

    def powerOn(self):
        ioctl(self.dev, 0x3600, 1)

    def powerOff(self):
        ioctl(self.dev, 0x3600, 0)

    def write(self, data):
        "Write a string, handling newlines"
        lines = data.split('\n')
        lastLine = lines[-1]
        for l in lines[:-1]:
            self.writeVFD(l)
            self.nl()
        self.writeVFD(lastLine)

    def writeVFD(self, data):
        self.dev.write(data)

    def cr(self):
        self.dev.write("\r")

    def lf(self):
        self.dev.write("\n")

    def nl(self):
        self.cr()
        self.lf()

    def clear(self):
        self.writeVFD(chr(0x15))

    def home(self):
        self.writeVFD(chr(0x16))

    def cursorOff(self):
        self.writeVFD(chr(0x0E))

    def writeScreen(self, data):
        """Write a complete screen of data, without clearing the screen"""
        lines = data.split('\n')
        while len(lines)<self.lines:
            lines.append("")
        self.home()
        self.cursorOff()
        for l in lines:
            if len(l) < self.width:
                l += " " * (self.width - len(l))
            elif len(l) > self.width:
                l = l[:self.width]
            self.writeVFD(l)
        self.dev.flush()

    def defineCharacter(self, char, data):
        """Set a user defined character from a 5x7 Numeric array"""

        # The VFD has a strange dot ordering, we swizzle our original
        # bitmap into an array ordered first by bit position then by byte
        bits = take(data.flat, ( (17, 13,  9,  5,  1),
                                 ( 0, 31, 27, 23, 19),
                                 (16, 12,  8,  4,  0),
                                 (34, 30, 26, 22, 18),
                                 (15, 11,  7,  3,  0),
                                 (33, 29, 25, 21,  0),
                                 (14, 10,  6,  2,  0),
                                 (32, 28, 24, 20,  0) ))

        # Pack this resulting array into bytes
        bytes = bits[0] + bits[1]*2 + bits[2]*4 + bits[3]*8 + bits[4]*16 + bits[5]*32 + bits[6]*64 + bits[7]*128

        # Convert to a command string and send it
        self.writeVFD(chr(0x18) + char + bytes.astype(UInt8).tostring())


class VFDupdater:
    "Collects information from XMMS and displays it on the VFD panel"
    scrollerDelay = 6
    scrollerIndex = 0
    scrollerGap = 5
    scrollSpeed = 12
    lastTitle = ''
    spinnerStates = '/-\\|'
    spinnerIndex = 0
    spinnerSpeed = 18

    spinnerIdle = array(
        [ [ 0, 0, 0, 0, 0 ],
          [ 0, 0, 0, 0, 0 ],
          [ 0, 1, 1, 1, 0 ],
          [ 0, 1, 1, 1, 0 ],
          [ 0, 1, 1, 1, 0 ],
          [ 0, 0, 0, 0, 0 ],
          [ 0, 0, 0, 0, 0 ] ])

    spinnerPaused = array(
        [ [ 0, 0, 0, 0, 0 ],
          [ 0, 0, 0, 0, 0 ],
          [ 0, 1, 1, 1, 0 ],
          [ 0, 0, 0, 0, 0 ],
          [ 0, 1, 1, 1, 0 ],
          [ 0, 0, 0, 0, 0 ],
          [ 0, 0, 0, 0, 0 ] ])

    spinnerPlaying = (
        array([ [ 0, 0, 0, 0, 0 ],
                [ 0, 0, 1, 0, 0 ],
                [ 0, 0, 1, 0, 0 ],
                [ 0, 0, 1, 0, 0 ],
                [ 0, 0, 1, 0, 0 ],
                [ 0, 0, 1, 0, 0 ],
                [ 0, 0, 0, 0, 0 ] ]),
        array([ [ 0, 0, 0, 0, 0 ],
                [ 0, 0, 0, 1, 0 ],
                [ 0, 0, 1, 0, 0 ],
                [ 0, 0, 1, 0, 0 ],
                [ 0, 0, 1, 0, 0 ],
                [ 0, 1, 0, 0, 0 ],
                [ 0, 0, 0, 0, 0 ] ]),
        array([ [ 0, 0, 0, 0, 0 ],
                [ 0, 0, 0, 0, 0 ],
                [ 0, 0, 0, 1, 0 ],
                [ 0, 0, 1, 0, 0 ],
                [ 0, 1, 0, 0, 0 ],
                [ 0, 0, 0, 0, 0 ],
                [ 0, 0, 0, 0, 0 ] ]),
        array([ [ 0, 0, 0, 0, 0 ],
                [ 0, 0, 0, 0, 0 ],
                [ 0, 0, 0, 0, 1 ],
                [ 0, 1, 1, 1, 0 ],
                [ 1, 0, 0, 0, 0 ],
                [ 0, 0, 0, 0, 0 ],
                [ 0, 0, 0, 0, 0 ] ]),
        array([ [ 0, 0, 0, 0, 0 ],
                [ 0, 0, 0, 0, 0 ],
                [ 0, 0, 0, 0, 0 ],
                [ 1, 1, 1, 1, 1 ],
                [ 0, 0, 0, 0, 0 ],
                [ 0, 0, 0, 0, 0 ],
                [ 0, 0, 0, 0, 0 ] ]),
        array([ [ 0, 0, 0, 0, 0 ],
                [ 0, 0, 0, 0, 0 ],
                [ 1, 0, 0, 0, 0 ],
                [ 0, 1, 1, 1, 0 ],
                [ 0, 0, 0, 0, 1 ],
                [ 0, 0, 0, 0, 0 ],
                [ 0, 0, 0, 0, 0 ] ]),
        array([ [ 0, 0, 0, 0, 0 ],
                [ 0, 0, 0, 0, 0 ],
                [ 0, 1, 0, 0, 0 ],
                [ 0, 0, 1, 0, 0 ],
                [ 0, 0, 0, 1, 0 ],
                [ 0, 0, 0, 0, 0 ],
                [ 0, 0, 0, 0, 0 ] ]),
        array([ [ 0, 0, 0, 0, 0 ],
                [ 0, 1, 0, 0, 0 ],
                [ 0, 1, 0, 0, 0 ],
                [ 0, 0, 1, 0, 0 ],
                [ 0, 0, 0, 1, 0 ],
                [ 0, 0, 0, 1, 0 ],
                [ 0, 0, 0, 0, 0 ] ]),
        )

    def __init__(self, vfd):
        self.vfd = vfd
        self.lastTime = time.time()

    def update(self):
        # Calculate the time step
        now = time.time()
        dt = now - self.lastTime
        self.lastTime = now

        # Get the current song title
        title = str(xmms.get_playlist_title(xmms.get_playlist_pos()))
        if len(title) > self.vfd.width:
            # Scroll it around so we can see the whole thing. At the beginning
            # of the scroll cycle it delays by scrollerDelay scroll cycles,
            # and there is a scrollerGap character gap between copies of the scrolled title.
            if title != self.lastTitle:
                self.scrollerIndex = -self.scrollerDelay
                self.lastTitle = title
            if self.scrollerIndex >= 1:
                self.scrollerIndex %= self.scrollerGap + len(title)
                if self.scrollerIndex < 1:
                    self.scrollerIndex = -self.scrollerDelay
                else:
                    title = ("%s%s%s" % (title, " " * self.scrollerGap, title))[int(self.scrollerIndex):]
            self.scrollerIndex += dt * self.scrollSpeed

        # get the current time index
        if xmms.is_playing():
            millisec = xmms.get_output_time()
            minutes = millisec / 60000
            seconds = (millisec % 60000) / 1000.0
            playTime = "%3d:%05.2f" % (minutes, seconds)
        else:
            playTime = " --:--.--"

        # Status string
        if xmms.is_paused():
            status = 'Paused'
            spinner = self.spinnerPaused
        elif xmms.is_playing():
            status = 'Playing'
            self.spinnerIndex %= len(self.spinnerPlaying)
            spinner = self.spinnerPlaying[int(self.spinnerIndex)]
            self.spinnerIndex += dt * self.spinnerSpeed
        else:
            status = 'Idle'
            spinner = self.spinnerIdle

        spinnerChar = self.vfd.userDefinedCharacters[0]
        self.vfd.defineCharacter(spinnerChar, spinner)
        self.vfd.writeScreen("%s\n%-9s%s %s" % (title, status, playTime, spinnerChar))


if __name__ == '__main__':
    vfdupdater = VFDupdater(CenturyVFD())

    while True:
        vfdupdater.update()
