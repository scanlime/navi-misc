#!/usr/bin/env python
#
# A copy of the VFD code from the LED Board v2 project, hacked up a bit
# to work with the USB VFD controller as a real-world test application.
#

import xmms, time, os

class CenturyVFD:
    width = 20
    lines = 2

    def __init__(self):
        self.dev = open("/dev/usb/widget", "w")

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
        "Write a complete screen of data, without clearing the screen"
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


class VFDupdater:
    "Collects information from XMMS and displays it on the VFD panel"
    scrollerDelay = 6
    scrollerIndex = 0
    scrollerGap = 5
    lastTitle = ''
    spinnerState = '/-\\|'

    def __init__(self, vfd):
        self.vfd = vfd

    def fastUpdate(self):
        pass

    def slowUpdate(self):
        # Get the current song title
        title = str(xmms.get_playlist_title(xmms.get_playlist_pos()))
        if len(title) > self.vfd.width:
            # Scroll it around so we can see the whole thing. At the beginning
            # of the scroll cycle it delays by scrollerDelay scroll cycles,
            # and there is a scrollerGap character gap between copies of the scrolled title.
            if title != self.lastTitle:
                self.scrollerIndex = -self.scrollerDelay
                self.lastTitle = title
            if self.scrollerIndex > 0:
                self.scrollerIndex %= self.scrollerGap + len(title)
                if self.scrollerIndex == 0:
                    self.scrollerIndex = -self.scrollerDelay
                else:
                    title = ("%s%s%s" % (title, " " * self.scrollerGap, title))[self.scrollerIndex:]
            self.scrollerIndex += 1

        # get the current time index
        if xmms.is_playing():
            millisec = xmms.get_output_time()
            minutes = millisec / 60000
            seconds = (millisec % 60000) / 1000.0
            playTime = "%3d:%02d" % (minutes, seconds)
        else:
            playTime = " --:--"

        # Status string
        if xmms.is_paused():
            status = 'Paused'
            spinner = '-'
        elif xmms.is_playing():
            status = 'Playing...'
            self.spinnerState = self.spinnerState[1:] + self.spinnerState[0]
            spinner = self.spinnerState[0]
        else:
            status = 'Idle'
            spinner = ' '

        self.vfd.writeScreen("%s\n%-12s%s %s" % (title, status, playTime, spinner))


if __name__ == '__main__':
    vfdupdater = VFDupdater(CenturyVFD())
    components = [vfdupdater]
    then = time.time()

    while True:
        # slowUpdates every 0.1 seconds, fastUpdates every 0.01 seconds nominally
        for c in components:
            c.slowUpdate()
        for i in range(10):
            for c in components:
                c.fastUpdate()
            now = time.time()
            target = then + 0.01
            if now < target:
                time.sleep(target - now)
            then = now
