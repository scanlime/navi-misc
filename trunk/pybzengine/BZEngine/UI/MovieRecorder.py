""" BZFlag.UI.MovieRecorder

A mostly-real-time movie recorder that saves to NuppelVideo files.
This is based on the recorder I originally wrote for Jetstream and
later ported to Crystal Space.
"""
#
# Python BZFlag Package
# Copyright (C) 2003 Micah Dowty <micahjd@users.sourceforge.net>
#
#  This library is free software; you can redistribute it and/or
#  modify it under the terms of the GNU Lesser General Public
#  License as published by the Free Software Foundation; either
#  version 2.1 of the License, or (at your option) any later version.
#
#  This library is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
#  Lesser General Public License for more details.
#
#  You should have received a copy of the GNU Lesser General Public
#  License along with this library; if not, write to the Free Software
#  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
#

from __future__ import division
from Numeric import *
from OpenGL.GL import *
import os, Image
from StringIO import StringIO
from BZFlag import Animated


class Recorder:
    """Movie Recorder that attaches to a Viewport and records frames to
       a NuppelVideo file as they're rendered.
       """
    def __init__(self, viewport, fileTemplate="snapshot%02d.bzm", frameRate=30.0):
        self.frameRate = frameRate
        self.viewport = viewport
        self.fileTemplate = fileTemplate
        self.recordedFrames = 0
        self.movieTime = 0
        self.running = False
        self.encoder = 'JPEG'
        self.options = {'quality': 98}

    def start(self):
        """Start recording to a new file. Returns a filename if the recording
           started, or None if this was already recording.
           """
        if self.running:
            return
        self.fileName = self.getFilename()
        self.file = open(self.fileName, "wb")
        self.image = Image.new('RGB', self.viewport.size)
        self.writeHeader()

        # Insert our recorder at the beginning of the render sequence.
        # After the beginning the next frame will have started, but inserting
        # at the end isn't friendly to the way Viewport manages subviewports.
        self.viewport.renderSequence.insert(0, self.recordFrame)
        self.running = True
        Animated.currentTimeMaster = MovieTimeMaster(self)
        return self.fileName

    def stop(self):
        if not self.running:
            return
        Animated.currentTimeMaster = Animated.defaultTimeMaster
        self.viewport.renderSequence.remove(self.recordFrame)
        self.running = False
        self.file.close()
        self.file = None

    def getFilename(self):
        """Using our fileTemplate, find the next available filename"""
        fileNum = 0
        while os.path.isfile(self.fileTemplate % fileNum):
            fileNum += 1
        return self.fileTemplate % fileNum

    def recordFrame(self):
        """Read a raw frame from OpenGL, and use PIL to compress it."""
        rgb = glReadPixels(self.viewport.rect[0],
                           self.viewport.rect[1],
                           self.image.size[0],
                           self.image.size[1],
                           GL_RGB,
                           GL_UNSIGNED_BYTE)
        self.image.fromstring(rgb)

        # Encode the frame into a StringIO then stick that in the file,
        # so that we can save a one-line header for each frame with its size.
        frame = StringIO()
        self.image.save(frame, self.encoder, **self.options)
        frameString = frame.getvalue()
        self.file.write("%s\n" % len(frameString))
        self.file.write(frameString)

        # Advance the recording clock
        self.recordedFrames += 1
        self.movieTime += 1/self.frameRate

    def writeHeader(self):
        self.file.write("bzmovie %s %s %s %s\n" % (self.image.size[0],
                                                   self.image.size[1],
                                                   self.encoder,
                                                   self.frameRate))

        
class MovieTimeMaster(Animated.TimeMaster):
    """Replacement TimeMaster class that keeps the global timebase in
       sync with a recording's constant frame rate.
       """
    def __init__(self, recorder):
        self.recorder = recorder
        Animated.TimeMaster.__init__(self)

    def update(self):
        # Keep updating the default TimeMaster, to avoid a very large
        # timestep when disabling recording
        Animated.defaultTimeMaster.update()

        self.now = self.recorder.movieTime
        self.stepTime = 1 / self.recorder.frameRate
        self.stepNumber += 1


class Transcoder:
    """Utility for converting recorded files to a usable format"""
    def __init__(self, filename):
        self.file = open(filename, "rb")
        self.readHeader()
        self.firstFrame = self.file.tell()

    def readHeader(self):
        """Read the file's header, storing relevant values"""
        line = self.file.readline().strip().split(" ")
        if line[0] != "bzmovie":
            raise Exception("This doesn't appear to be a bzmovie file")
        self.size = (int(line[1]), int(line[2]))
        self.encoder = line[3]
        self.frameRate = float(line[4])

    def readFrame(self):
        """Read and decompress a frame"""
        # Our frame consists of a line of text with the frame's size in bytes, then the encoded frame
        # At both stages we need to check for end-of-file, which will be a read returning an empty string.
        line = self.file.readline()
        if not line:
            return None
        size = int(line.strip())
        data = self.file.read(size)
        if not data:
            return None

        img = Image.open(StringIO(data))
            
        # Flip the image vertically as we output it, since it was stored
        # in OpenGL's bottom-up orientation.
        return img.tostring('raw', 'RGB', 0, -1)

    def encode(self, options, program="mencoder"):
        """Feed this recording into mencoder with the given options"""

        # These options set up mplayer to read raw RGB frames. The fourcc there is
        # hex for ('R', 'G', 'B', 24). All other parameters should be supplied by the caller.
        # We use popen to pipe video into mencoder's stdin.
        cmdline = "%s - -rawvideo on:fps=%s:w=%d:h=%d:format=0x52474218:size=%d " % \
                  (program, self.frameRate, self.size[0], self.size[1], self.size[0] * self.size[1] * 3)
        cmdline += options
        
        print cmdline + "\n"
        encoder = os.popen(cmdline, 'w')

        frameCount = 0
        self.file.seek(self.firstFrame)
        while True:
            frame = self.readFrame()
            if frame is None:
                break
            frameCount += 1
            encoder.write(frame)
        encoder.close()

### The End ###
