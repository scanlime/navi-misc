#!/usr/bin/env python
from BZEngine.UI import Viewport, ThreeDRender, ThreeDControl, HUD, Drawable
from BZEngine import Event, Animated, Geometry, Util
import cPickle, copy
from Numeric import *

loop = Event.EventLoop()
viewport = Viewport.OpenGLViewport(loop)
viewport.setCaption("Wasabi")
view = ThreeDRender.View(viewport)
control = ThreeDControl.Viewing(view, viewport)


class OrbitingParticles:
    """A particle system orbiting around the origin"""
    def __init__(self, particle, radius=3, rotationPeriod=1, noisePeriod=10, seed=0):
        self.particle = particle
        self.plane = Animated.Vector(Animated.PerlinNoise(frequency = 1.0 / noisePeriod))
        self.angle = Animated.Value(Animated.RampFunction(rotationPeriod, (0, 2*math.pi)))
        self.radius = radius

    def getDrawables(self):
        return [self.particle]

    def integrate(self, dt):
        self.angle.integrate(dt)
        self.plane.integrate(dt)

        # Create two perpendicular vectors on our orbit plane, and use those
        # and our angle to calculate the emitter position
        plane = self.plane.get()
        v1 = Geometry.normalize(Geometry.cross(plane, (1,0,0)))
        v2 = Geometry.normalize(Geometry.cross(plane, v1))
        self.particle.emitter.position = (cos(self.angle.value)*v1 + sin(self.angle.value)*v2) * self.radius


class Page:
    """A collection of scene objects, event handlers, and other objects forming an
       'interactive video clip' with a definite beginning and end. These objects
       are put into a Book along with other Pages.

       A page is active as long as it exists in memory- between when its constructor
       finishes and when its finalize() member is called.

       The Book manages instantiating and deleting Pages. A page signals that it's
       job is finished by triggering its onFinish event.
       """
    def __init__(self, view):
        Event.attach(self, "onFinish")
        self.viewport = view.viewport
        self.view = view
        self.time = Animated.Timekeeper()

    def finalize(self):
        """Optional hook to perform any deinitialization the Page needs"""
        pass


class Book(Page):
    """A collection of Pages. A Book is responsible for controlling the overal flow
       of control in a program built from multiple Pages. The page at the beginning
       of the Book is always the active page. Pages are removed from
       the Book when their onFinish event is triggered. Pages can be added either at
       the beginning or at the end of the book- pages added at the beginning are made
       active immediately, and the formerly active page resumes when that page finishes.
       Pages added at the end are activated after all other pages have finished.

       Note that since pages are only instantiated when active, pages specified for
       insertion into the Book are given as a (class, parameters) tuple. The 'view'
       parameter is assumed, and is not given in the parameters list. If no parameters
       are needed, the class can be given bare rather than as part of a tuple.

       Also note that a book is, itself, a page. This is a convenient way to group
       pages hierarchially.
       """
    def __init__(self, view, initialPages):
        Page.__init__(self, view)
        self.pages = initialPages
        self.activeInstance = None
        self.activeSpec = None
        self.evaluatePages()

    def pushFront(self, pages):
        """Add the given list of pages to the front of the book. The first page
           given will become active.
           """
        self.pages = pages + self.pages
        self.evaluatePages()

    def pushBack(self, pages):
        """Add the given list of pages to the back of the book"""
        self.pages = self.pages + pages
        self.evaluatePages()

    def popFront(self):
        """Remove the currently active page. This is called automatically
           by a page's onFinish handler.
           """
        del self.pages[0]
        self.evaluatePages()

    def evaluatePages(self):
        """Used internally, this looks at the list of pages and determines if the active
           page needs to be changed. If so, the new active page is instantiated and the
           reference to the old active page is lost, terminating it.
           """
        if self.pages:
            # If the currently active page wasn't created from the spec at the front of the page list,
            # it should no longer be active. Make it so.
            if self.activeSpec is not self.pages[0]:
                if self.activeInstance:
                    self.activeInstance.finalize()
                self.activeSpec = None
                self.activeInstance = None

                # Transform the item at the front of self.pages into a pageClass and pageParameters.
                # This accounts for the format described in this class's doc string- a page specification
                # may be a bare class or it may be a (class, parameters) tuple.
                try:
                    pageClass, pageParameters = self.pages[0]
                except TypeError:
                    pageClass = self.pages[0]
                    pageParameters = ()

                # Create the new active page instance
                self.activeSpec = self.pages[0]
                self.activeInstance = pageClass(self.view, *pageParameters)

                # Set up an observer on the page's onFinish handler that removes it from the book
                self.activeInstance.onFinish.observe(self.popFront)
        else:
            # No more pages, disable any currently active page and call our onFinish event
            if self.activeInstance:
                self.activeInstance.finalize()
            self.activeSpec = None
            self.activeInstance = None
            self.onFinish()


class WasabiLogo(Page):
    def __init__(self, view):
        Page.__init__(self, view)

        self.viewport.mode = Viewport.GL.ClearedMode(clearColor=(0, 0, 0, 1))

        self.view.camera.position = (0.0314, -3, 0)
        self.view.camera.distance = 4.49
        self.view.camera.azimuth = 3
        self.view.camera.elevation = 2.4
        self.view.camera.jump()

        self.title = HUD.Text(self.viewport.region(self.viewport.rect),
                              "wasabi",
                              color     = (0, 0, 0, 1),
                              fontSize  = self.viewport.size[1] / 5,
                              alignment = (0.5, 0.5),
                              fontName  = "geodesic.ttf")

        orbitParticles = ("green_flame.particle", "green_nebula.particle")
        self.orbits = [OrbitingParticles(cPickle.load(open(Util.dataFile(name)))) for name in orbitParticles]
        for orbit in self.orbits:
            self.view.scene.add(orbit)
        self.viewport.onSetupFrame.observe(self.setupFrame)

        self.timer = 0

    def setupFrame(self):
        dt = self.time.step()
        for orbit in self.orbits:
            orbit.integrate(dt)

        self.timer += dt
        if self.timer > 5:
            self.onFinish()

    def finalize(self):
        for orbit in self.orbits:
            self.view.scene.remove(orbit)


class Monkey(Page):
    def __init__(self, view):
        Page.__init__(self, view)

        self.viewport.mode = Viewport.GL.ClearedMode(clearColor=(0, 0, 0, 1))

        self.meshes = tuple(Drawable.VRML.load("monkey.wrl").values())
        view.scene.add(self.meshes)

        self.view.camera.position = (0,0,0)
        self.view.camera.distance = 73
        self.view.camera.azimuth = 210
        self.view.camera.elevation = -2
        self.view.camera.jump()

        self.viewport.onSetupFrame.observe(self.setupFrame)

    def setupFrame(self):
        dt = self.time.step()
        self.view.camera.azimuth += dt * 80

    def finalize(self):
        self.scene.remove(self.meshes)


book = Book(view, [WasabiLogo, Monkey])
loop.run()
