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
from BZEngine.UI import ThreeDRender, Viewport, Instrument


class Modifiers:
    """Represents a set of modifier keys. The modifer can be zero or more
       pygame.KMOD_* constants or'ed together, None, or 'any'.
       """
    def __init__(self, modifiers):
        if modifiers is None:
            modifiers = 0
        if modifiers == 'any':
            self.requireMods = 0
            self.disallowMods = 0
        else:
            self.requireMods = modifiers
            self.disallowMods = (~modifiers) & (pygame.KMOD_SHIFT | pygame.KMOD_CTRL |
                                                pygame.KMOD_ALT | pygame.KMOD_META)

    def test(self):
        """Return true if the currently pressed modifiers fit this class' criteria"""
        mods = pygame.key.get_mods()
        if mods & self.disallowMods:
            return False
        if mods & self.requireMods or not self.requireMods:
            return True
        return False


class MouseDrag(Event.Event):
    """Binds a mouse drag action to a function, with optional modifier keys.
       The function is called with X and Y relative motion as its two parameters.
       Buttons are numbered starting at 1.
       """
    def __init__(self, viewport, button, modifiers=None, scale=1):
        Event.Event.__init__(self)
        self.button = button
        self.scale = scale
        self.modifiers = Modifiers(modifiers)
        viewport.onMouseMotion.observe(self.handleMotion)

    def handleMotion(self, event):
        if event.buttons[self.button-1] and self.modifiers.test():
                self(event.rel[0] * self.scale, event.rel[1] * self.scale)


class MouseGrab:
    """When the mouse is dragged with the specified button, hide it and
       snap it back to its initial position. This makes it much more
       intuitive to perform drags that don't correspond directly to
       viewport coordinates. Since this doesn't perform an actual mouse grab,
       if the software hangs for any reason, the mouse will not be trapped.
       Buttons are numbered starting at 1.
       """
    def __init__(self, viewport, button):
        self.button = button
        viewport.onMouseMotion.observe(self.handleMotion)
        viewport.onMouseButtonDown.observe(self.handlePress)
        viewport.onMouseButtonUp.observe(self.handleRelease)
        self.homePos = None

    def handlePress(self, event):
        if event.button == self.button:
            pygame.mouse.set_visible(False)
            self.homePos = event.pos

    def handleRelease(self, event):
        if event.button == self.button:
            pygame.mouse.set_visible(True)
            self.homePos = None

    def handleMotion(self, event):
        if self.homePos:
            # Snap the mouse back to the position it was clicked at.
            # This generates an event, so we have to rummage through the event
            # queue and discard it, putting all the other events back.
            pygame.mouse.set_pos(*self.homePos)
            for newEvent in pygame.event.get([pygame.MOUSEMOTION]):
                if newEvent.pos != self.homePos:
                    pygame.event.post(newEvent)


class KeyPress(Event.Event):
    """An event that fires when a specified key is pressed. Either a
       string or a key constant can be given.
       """
    def __init__(self, viewport, key, modifiers=None):
        Event.Event.__init__(self)
        self.key = key
        self.modifiers = Modifiers(modifiers)
        viewport.onKeyDown.observe(self.handlePress)

    def handlePress(self, event):
        if type(self.key) == str or type(self.key) == unicode:
            if event.unicode == self.key and self.modifiers.test():
                self()
        else:
            if event.key == self.key and self.modifiers.test():
                self()


class KeyAxis(Event.Event):
    """A combination of two keys that simulate a scalar axis"""
    def __init__(self, viewport, keyPlus, keyMinus, scale=1, modifiers=None):
        Event.Event.__init__(self)
        self.scale = scale
        self.keyPlus = KeyPress(viewport, keyPlus, modifiers)
        self.keyMinus = KeyPress(viewport, keyMinus, modifiers)
        self.keyPlus.observe(self.increment)
        self.keyMinus.observe(self.decrement)

    def increment(self):
        self(self.scale)

    def decrement(self):
        self(-self.scale)


class MouseWheel(Event.Event):
    """An event that fires when the mous wheel is pressed.
       The event handler gets a positive or negative copy of
       'scale' depending on the direction.
       """
    def __init__(self, viewport, scale=1, modifiers=None):
        Event.Event.__init__(self)
        self.scale = scale
        self.modifiers = Modifiers(modifiers)
        viewport.onMouseButtonDown.observe(self.handleWheel)

    def handleWheel(self, event):
        if event.button == 4 and self.modifiers.test():
            self(self.scale)
        elif event.button == 5 and self.modifiers.test():
            self(-self.scale)


class Viewing:
    """A controller for ThreeDView that provides rotation and panning through
       the level. This alone makes a good world viewer, and it is used as the
       foundation for a world editor.
       """
    def __init__(self, view, viewport):
        self.bindings = []
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

        self.bind(KeyPress, 'f').observe(self.toggleFullscreen)
        self.bind(KeyPress, 'c').observe(self.toggleCameraInfo)
        self.bind(KeyPress, 'w').observe(self.toggleWireframe)
        self.bind(KeyPress, 'x').observe(self.toggleXRay)
        self.bind(KeyPress, 'r').observe(self.toggleRecorder)
        self.bind(KeyPress, 'h').observe(self.toggleFrameRate)
        self.bind(KeyPress, 'q').observe(self.quit)
        self.bind(KeyPress, pygame.K_ESCAPE).observe(self.quit)
        self.bind(MouseWheel, 0.1, 'any').observe(self.zoom)
        self.bind(KeyAxis, '=', '-', 0.1).observe(self.zoom)
        self.bind(KeyAxis, '[', ']', 0.1).observe(self.fovZoom)

        dragButton = 3
        self.bind(MouseGrab, dragButton)
        self.bind(MouseDrag, dragButton, None, 0.2).observe(self.rotate)
        self.bind(MouseDrag, dragButton, pygame.KMOD_SHIFT).observe(self.pan)
        self.bind(MouseDrag, dragButton, pygame.KMOD_CTRL).observe(self.lift)

    def bind(self, cls, *args, **kw):
        """Set up a key or mouse binding using the given class. Returns the class
           instance ready for binding to an observer, and stores the event for later
           editing.
           """
        binding = cls(self.viewport, *args, **kw)
        self.bindings.append(binding)
        return binding

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
