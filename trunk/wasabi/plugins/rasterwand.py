#!/usr/bin/env python
""" Rasterwand spectrum analyzer plugin for Freevo

This is a spectrum analyzer for the Rasterwand, reading
audio data from the mplayer advanced visualization
shared memory segment.

This is really messy and needs lots of optimization-
it's a fun toy, but avert your eyes from the code.

Copyright (C) 2004 Micah Dowty <micah@navi.cx>
"""

import plugin

import os, mmap, struct, time, threading
from Numeric import *
import FFT


class MPAVClient:
    """MPlayer Audio Visualization client. This connects to the
       shared memory area used by MPlayer to export audio information,
       and packages the received data in Numeric arrays.
       """
    lastUpdateTime = 0
    lastUpdateCount = 0
    stop = False

    def __init__(self, filename="/tmp/mpav"):
        self.fd = os.open(filename, os.O_RDONLY)

        # Create a small map to read the header, then expand our mapping
        self.map = mmap.mmap(self.fd, 16, mmap.MAP_SHARED, mmap.PROT_READ)
        self.channels, self.bufferSize, count = struct.unpack("iiQ", self.map[:16])
        self.map.resize(16 + self.bufferSize)

    def getCount(self):
        """Read the current block count. This can be used to check
           for new samples, or see how many we've skipped.
           """
        return struct.unpack("iiQ", self.map[:16])[2]

    def getBuffer(self):
        """Read the latest buffer of audio data, return it as a Numeric
           array of shape (channels, samples)
           """
        a = fromstring(self.map[16:], Int16)
        a.shape = (self.channels, -1)
        return a

    def waitForBuffer(self):
        self.waitForUpdates()
        return self.getBuffer()

    def waitForUpdates(self, threshold=0.5, pollInterval=1.0):
        """This keeps track of how long it's been since the buffer's
           count has been updated. If the buffer is older than the
           specified threshold time, we poll every pollInterval seconds
           for new data.
           """
        currentTime = time.time()
        currentCount = self.getCount()
        if currentCount != self.lastUpdateCount:
            self.lastUpdateCount = currentCount
            self.lastUpdateTime = currentTime

        if currentTime > self.lastUpdateTime + threshold:
            while not self.stop:
                self.onIdle()
                time.sleep(pollInterval)
                if self.getCount() != currentCount:
                    break

    def onIdle(self):
        pass


class Delay:
    """Compensate for audio output latency by introducing latency in the
       visualization. This buffers 'size' results from the given callable,
       and can be used in place of the original to return the buffered results.
       """
    def __init__(self, size, original):
        self.size = size
        self.original = original
        self.buffer = [original() for i in xrange(size)]

    def __call__(self):
        self.buffer.append(self.original())
        r = self.buffer[0]
        del self.buffer[0]
        return r


class RasterBargraph:
    """An interface to the Rasterwand display that draws 1-D arrays of
       data as collections of bars with varying heights.
       """
    def __init__(self, device="/dev/usb/rwand0"):
        self.fd = os.open(device, os.O_WRONLY)
        self.bars = 255 - ((1 << arange(8, -1, -1)) - 1)

    def writeFrame(self, a):
        """Write a packed 1-bit image to the rasterwand. Each element
           of the provided 1D array is a byte holding a column of the display.
           """
        str = a.astype(UnsignedInt8).tostring()
        os.write(self.fd, str)

    def writeBars(self, a):
        """Display a 1D array of values between 0 and 1 as bars"""
        i = floor(clip(a, 0, 1) * (len(self.bars)-1) + 0.5)
        self.writeFrame(take(self.bars, i.astype(Int)))

    def clear(self):
        self.writeBars(array([0]))


class SDLBargraph:
    """A RasterBargraph-workalike that draws high-resolution bars
       using SDL, making it easier to see what's going on while tweaking
       constants.
       """
    def __init__(self):
        import pygame
        pygame.init()
        self.screen = pygame.display.set_mode((640,200))
        self.margin = 30

    def writeBars(self, a):
        import pygame
        a = clip(a, 0, 1)
        self.screen.fill(0xffffff)
        x = self.margin
        barWidth = (self.screen.get_width() - self.margin*2) / len(a)
        maxHeight = self.screen.get_height() - self.margin*2
        for bar in a:
            height = int((bar * maxHeight) + 0.5)
            self.screen.fill(0x000022, (x, self.screen.get_height() -
                                        self.margin - height, barWidth-1, height))
            x += barWidth
        pygame.display.flip()


class Visualizer:
    def __init__(self):
        self.inputVolume = 8e4
        self.bars = None

        # Frequency tap indices for each bar
        s = 20
        self.taps = (exp(arange(0, 1, 0.07)) * s - s).astype(Int)

    def transform(self, inputSignal):
        # Automatic gain control
        gain = 0.4e5 / self.inputVolume

        # Read the audio, take an FFT of the left channel
        audio = inputSignal[0]
        fft = abs(FFT.real_fft(audio * gain))

        # Track the input volume, for automatic gain control
        total = abs(add.reduce(audio))
        alpha = 0.001
        self.inputVolume = (1-alpha)*self.inputVolume + alpha*total

        # Scale down the frequency axis nicely by integrating,
        # sampling, then differentiating.
        sums = add.accumulate(fft)
        hscaled = take(sums, self.taps)
        hscaled = maximum(hscaled[1:] - hscaled[:-1], 0)
        vscaled = hscaled * 4e-7

        # Add gradual decay to bar heights
        if self.bars:
            self.bars -= 0.05
            self.bars = maximum(self.bars, vscaled)
        else:
            self.bars = vscaled
        return self.bars


class PluginThread(threading.Thread):
    stop = False

    def run(self):
        self.mpav = MPAVClient()
        rw = RasterBargraph()
        self.mpav.onIdle = rw.clear
        delayed = Delay(30, self.mpav.waitForBuffer)
        viz = Visualizer()
        while not self.stop:
            rw.writeBars(viz.transform(delayed()))
        rw.clear()

    def waitForCompletion(self):
        self.stop = True
	self.mpav.stop = True
	while self.thread.isAlive():
	    time.sleep(0.1)


class PluginInterface(plugin.DaemonPlugin):
    def __init__(self):
        self.thread = PluginThread()
        self.thread.setDaemon(1)
        self.thread.start()
	plugin.DaemonPlugin.__init__(self)

    def shutdown(self):
        self.thread.waitForCompletion()

### The End ###
