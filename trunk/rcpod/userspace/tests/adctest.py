#!/usr/bin/env python
#
# Test the pic's A/D converters by sampling all channels repeatedly.
# Draws a bargraph of all ADC channels using pygame
#

from __future__ import division
import rcpod, pygame
from pygame.locals import *


class AnalogSampler:
    def __init__(self, pic=None):
        if not pic:
            pic = rcpod.Device()
        self.pic = pic

        # Initialize the ADC speed and turn it on
        pic.ADCON0 = 0x81

    def readChannel(self, channel):
        """Read one ADC channel, return a value in the range [0,1]"""
        # Select a channel and start the conversion
        self.pic.ADCON0 = 0x81 | (channel << 3)
        self.pic.ADCON0 = 0x85 | (channel << 3)

        # Normally we'd poll ADCON0 now to see if the conversion is done,
        # but rcpod is slow enough we don't need to worry about this.
        return self.pic.ADRES / 255.0

    def readAll(self, repeats=1, channels=xrange(8)):
        """Read all ADC channels, return a list of floats.
           In order to provide more stable readings with high impedance inputs,
           this can optionally repeat each channel's reading several times to let
           the ADC's holding capacitor charge over a longer period of time.
           """
        results = []
        for channel in xrange(8):
            for i in xrange(repeats):
                v = self.readChannel(channel)
            results.append(v)
        return results


class ChannelGraph:
    def __init__(self, resolution=(512,320), title="rcpod A/D Test"):
        pygame.init()
        self.screen = pygame.display.set_mode(resolution)
        pygame.display.set_caption(title)

    def drawChannels(self, channels, border=1, bgColor=(0,0,0), fgColor=(102,103,174)):
        """Draws bargraphs for the given list of channels"""
        width, height = self.screen.get_size()
        self.screen.fill(bgColor)
        chWidth = width / len(channels)
        x = 0
        for reading in channels:
            chHeight = reading * (height-border*2)
            self.screen.fill(fgColor, (x+border, height-border*2-chHeight, chWidth-border*2, chHeight))
            x += chWidth

    def run(self, sampler):
        """Run until our window is closed, drawing data from the given sampler function"""
        while 1:
            for event in pygame.event.get():
                if event.type == QUIT:
                    return
            self.drawChannels(sampler())
            pygame.display.flip()


if __name__ == '__main__':
    ChannelGraph().run(AnalogSampler().readAll)
