#!/usr/bin/env python
#
# Test the pic's A/D converters by sampling all channels repeatedly.
# Draws a bargraph of all ADC channels using pygame
#

from __future__ import division
import rcpod, pygame
from pygame.locals import *


class AnalogSampler:
    def __init__(self):
        pic = rcpod.Device()
        self.pic = pic

        # Initialize the ADC speed and turn it on
        pic.ADCON0 = 0x81

    def read(self, repeats=1):
        """Read all ADC channels, return a list of floats.
           In order to provide more stable readings with high impedance inputs,
           this can optionally repeat each channel's reading several times to let
           the ADC's holding capacitor charge over a longer period of time.
           """
        results = [0] * 8
        for channel in xrange(8):
            for i in xrange(repeats):
                # Select a channel and start the conversion
                self.pic.ADCON0 = 0x85 | (channel << 3)

                # Normally we'd poll ADCON0 now to see if the conversion is done,
                # but rcpod is slow enough we don't need to worry about this.

            # Store result
            results[channel] = self.pic.ADRES / 255.0
        return results


class ChannelGraph:
    def __init__(self):
        pygame.init()
        self.screen = pygame.display.set_mode((512,320))
        pygame.display.set_caption("rcpod A/D Test")

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
        """Run until our window is closed, drawing data from the given sampler"""
        while 1:
            for event in pygame.event.get():
                if event.type == QUIT:
                    return
            self.drawChannels(sampler.read())
            pygame.display.flip()


ChannelGraph().run(AnalogSampler())
