#!/usr/bin/env python
#
# A copy of the VFD code from the LED Board v2 project, hacked up a bit
# to work with the USB VFD controller as a real-world test application.
#

import xmms, time, os, mi6k
from Numeric import *


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

        if xmms.is_paused():
            spinner = self.spinnerPaused
        elif xmms.is_playing():
            self.spinnerIndex %= len(self.spinnerPlaying)
            spinner = self.spinnerPlaying[int(self.spinnerIndex)]
            self.spinnerIndex += dt * self.spinnerSpeed
        else:
            spinner = self.spinnerIdle

	clock = time.strftime("%H:%M", time.localtime())

        spinnerChar = self.vfd.userDefinedCharacters[0]
        self.vfd.defineCharacter(spinnerChar, spinner)
        self.vfd.writeScreen("%s\n%-9s%s %s" % (title, clock, playTime, spinnerChar))


if __name__ == '__main__':
    vfdupdater = VFDupdater(mi6k.MI6K().vfd)

    while True:
        vfdupdater.update()
