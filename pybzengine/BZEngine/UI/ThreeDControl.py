""" BZEngine.UI.ThreeDControl

Controller classes for manipulating 3D views
"""
#
# Python BZEngine Package
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

import math, sys, pygame
from BZEngine import Animated, Event
from BZEngine.UI import ThreeDRender, Viewport, Instrument, Input


class Viewing:
    """A controller for ThreeDView that provides rotation and panning through
       the level. This alone makes a good world viewer, and it is used as the
       foundation for a world editor.
       """
    def __init__(self, view, viewport):
        self.view = view
        self.viewport = viewport
        self.movieRecorder = None
        self.savedMode = None
        self.frameRateInstrument = None
        self.cameraInfoInstrument = None

        self.view.camera = ThreeDRender.SmoothedCamera()
        view.camera.position = (0, 0, 20)
        view.camera.distance  = 900
        view.camera.azimuth   = 0
        view.camera.elevation = 15
        view.camera.jump()

        self.bindings = Input.load(viewport, self, 'viewing_keys.py')

    def pan(self, horizontal, vertical):
        (x, y, z) = self.view.camera.position
        radians = -self.view.camera.azimuth * math.pi / 180
        cos = math.cos(radians)
        sin = math.sin(radians)
        x -=  cos*horizontal - sin*vertical
        y -= -sin*horizontal - cos*vertical
        self.view.camera.position = (x, y, z)

    def lift(self, ignoredAxis, lift):
        (x, y, z) = self.view.camera.position
        z += lift
        if z < 0:
            z = 0
        if z > 1000:
            z = 1000
        self.view.camera.position = (x, y, z)

    def rotate(self, azimuth, elevation):
        self.view.camera.azimuth -= azimuth
        self.view.camera.elevation += elevation
        if self.view.camera.elevation < -90:
            self.view.camera.elevation = -90
        if self.view.camera.elevation > 90:
            self.view.camera.elevation = 90

    def toggleFullscreen(self):
        self.viewport.display.toggle_fullscreen()

    def toggleViewportMode(self, modeClass, *args, **kw):
        """Toggles into or out of a given viewport mode, saving the old mode properly"""
        if isinstance(self.viewport.mode, modeClass):
            # Toggle out back to the saved mode
            self.viewport.mode = self.savedMode
            self.savedMode = None
        else:
            # If we aren't already in another mode we set, save the current mode
            if not self.savedMode:
                self.savedMode = self.viewport.mode

            # Toggle into the new mode
            self.viewport.mode = modeClass(*args, **kw)

    def toggleWireframe(self):
        self.toggleViewportMode(Viewport.GL.WireframeMode)

    def toggleXRay(self):
        self.toggleViewportMode(Viewport.GL.XRayMode)

    def toggleFrameRate(self):
        if self.frameRateInstrument:
            self.frameRateInstrument = None
        else:
            self.frameRateInstrument = Instrument.FrameRate(self.viewport)

    def toggleCameraInfo(self):
        if self.cameraInfoInstrument:
            self.cameraInfoInstrument = None
        else:
            # The alignment keyword is interpreted by HUD.Text, placing the info at the
            # bottom-left corner of the screen
            self.cameraInfoInstrument = Instrument.CameraInfo(self.viewport, self.view.camera,
                                                              alignment = (0,1))

    def quit(self):
        self.viewport.eventLoop.stop()

    def zoom(self, amount):
        """Zoom the camera by changing the distance from focus"""
        self.view.camera.distance *= math.exp(-amount)
        if self.view.camera.distance < 0.1:
            self.view.camera.distance = 0.1
        if self.view.camera.distance > 1500:
            self.view.camera.distance = 1500

    def fovZoom(self, amount):
        """Zoom the camera by changing the field of view"""
        self.view.viewport.fov *= math.exp(amount)
        if self.view.viewport.fov > 175:
            self.view.viewport.fov = 175
        if self.view.viewport.fov < 0.1:
            self.view.viewport.fov = 0.1

    def toggleRecorder(self):
        if self.movieRecorder:
            self.movieRecorder.stop()
            self.movieRecorder = None
            print "--- Stopped recording"
        else:
            try:
                from BZEngine.UI import MovieRecorder
                self.movieRecorder = MovieRecorder.Recorder(self.viewport)
                filename = self.movieRecorder.start()
                print "--- Recording video to %s" % filename
            except:
                exc_info = sys.exc_info()
                print "*** An exception occurred while trying to start the movie recorder:"
                print "    %s: %s" % (exc_info[0].__name__, exc_info[1])
                self.movieRecorder = None


class Editing(Viewing):
    """Implement a superset of the Viewing controls, used for editing worlds"""
    def __init__(self, view, viewport):
        Viewing.__init__(self, view, viewport)
        viewport.onMouseButtonDown.observe(self.mouseButtonDown)

    def mouseButtonDown(event):
        if event.button == 1:
            print 'picked: %r' % view.pick(event.pos)

### The End ###
