""" BZEngine.UI.ThreeDRender.Engine

The core classes forming our simple 3D engine as described in ThreeDRender
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

from BZEngine import Animated
from OpenGL.GL import *
from BZEngine.UI import Drawable
import Pass
from Numeric import subtract
from OpenGL.GLU import gluPickMatrix


class PickingState:
    """Holds render state information specific to picking. This is passed
       to render passes when doing a picking render, so the passes can tag
       all their objects with names.
       """
    def __init__(self):
        self.nextName = 1
        self.names = {}

    def name(self, drawable):
        """Move to the next OpenGL name ID, and associate it with the
           given drawable object.
           """
        self.names[self.nextName] = drawable
        glLoadName(self.nextName)
        self.nextName += 1


class Camera:
    """Abstraction for the OpenGL transforms used to set up the camera.
       For 1st person operation, use 'position', 'azimuth' and 'elevation',
       with 'distance' set to zero. For 3rd person operation, set 'distance'
       to a nonzero value.
       """
    def __init__(self):
        self.position  = (0, 0, 0)
        self.distance  = 0
        self.azimuth   = 0
        self.elevation = 0
	self.azimuthOffset = 0

    def jump(self):
        """If this camera class implements any animation techniques, this method
           should ensure that the camera arrives instantly at its destination after
           the coordinates have been changed.
           """
        pass

    def load(self):
        """Set the current OpenGL matrix according to the camera's location and orientation"""
        self.applyTransform(self.distance,
                            self.elevation,
                            self.azimuth,
                            self.position)

    def applyTransform(self, distance, elevation, azimuth, position):
        """This applies the camera's modelview matrix transform.
           Note that position is now the position the camera orbits around,
           and elevation specifies how far the camera is above the XY plane,
           in degrees.
           """
        glLoadIdentity()
        glTranslatef(0, 0, -distance)
        glRotatef(elevation - 90, 1.0, 0.0, 0.0)
        glRotatef(-(azimuth + self.azimuthOffset), 0.0, 0.0, 1.0)
        glTranslatef(*subtract((0,0,0), position))


class SmoothedCamera(Camera):
    """Replacement Camera class that smooths all axes with the Animated module"""
    def __init__(self):
        Camera.__init__(self)
        self.time = Animated.Timekeeper()
        self.animatedDistance  = Animated.Value (Animated.LogApproach(self.distance, 4),
                                                 self.distance)
        self.animatedPosition  = Animated.Vector(Animated.LogApproach(self.position, 10),
                                                 self.position)
        self.animatedAzimuth   = Animated.Value (Animated.LogApproach(self.azimuth, 10),
                                                 self.azimuth)
        self.animatedElevation = Animated.Value (Animated.LogApproach(self.elevation, 10),
                                                 self.elevation)
        self.animated = [self.animatedDistance,
                         self.animatedPosition,
                         self.animatedAzimuth,
                         self.animatedElevation]

    def jump(self):
        """This cuts directly to the target position, without all that pesky animation"""
        self.animatedDistance.value  = self.distance
        self.animatedPosition.set(self.position)
        self.animatedAzimuth.value = self.azimuth
        self.animatedElevation.value = self.elevation

    def load(self):
        self.animatedDistance.f.target  = self.distance
        self.animatedPosition.f.target  = self.position
        self.animatedAzimuth.f.target   = self.azimuth
        self.animatedElevation.f.target = self.elevation
        dt = self.time.step()
        for item in self.animated:
            item.integrate(dt)

        self.applyTransform(self.animatedDistance.value,
                            self.animatedElevation.value,
                            self.animatedAzimuth.value,
                            self.animatedPosition.get())


class Light:
    """Abstraction for an OpenGL light's position and intensity"""
    def __init__(self, lightnum):
        self.lightnum = lightnum
        self.reset()

    def reset(self):
        self.enabled  = False
        self.ambient  = (0, 0, 0, 1)
        self.diffuse  = (1, 1, 1, 1)
        self.position = (0, 0, 0, 1)

    def set(self):
        """Set up and enable the light in the current OpenGL context"""
        if self.enabled:
            glEnable(self.lightnum)
            glLightfv(self.lightnum, GL_AMBIENT, self.ambient)
            glLightfv(self.lightnum, GL_DIFFUSE, self.diffuse)
            glLightfv(self.lightnum, GL_POSITION, self.position)
        else:
            glDisable(self.lightnum)


class Scene:
    """Set of all the objects this view sees, organized into rendering passes
       and sorted by texture. Multiple rendering passes are necessary to deal
       with blended objects, and texture sorting is an important OpenGL optimization.
       """
    def __init__(self):
        self.erase()
        self.mainView = None
        self.dynTexRenderState = None
        self.passes = Pass.getRenderPasses()

    def hasMainView(self):
        return self.mainView is not None

    def setMainView(self, view):
        """The scene can be used with any number of views, but one 'main' view is
           required for rendering dynamic textures.
           """
        self.mainView = view
        self.dynTexRenderState = Drawable.RenderState(view)

        # Go through existing textures and set up any that haven't been already
        for drawables in self.objects.itervalues():
            for drawable in drawables:
                for texture in drawable.render.textures:
                    if hasattr(texture, 'hasRenderState'):
                        if not texture.hasRenderState():
                            texture.attachRenderState(self.dynTexRenderState)

    def erase(self):
        self.objects = {}
        self.dirty = True

    def add(self, object):
        """Add the given object to the scene.
           The object may be:
              - Any object that supports the getDrawables() method
              - A list of drawables
              - A drawable
           """
        if hasattr(object, 'getDrawables'):
            drawables = object.getDrawables()
        elif type(object) == type(()) or type(object) == type([]):
            drawables = object
        elif isinstance(object, Drawable.GLDrawable):
            drawables = (object,)
        else:
            raise TypeError("Scene.add() expects an object with getDrawables(), a drawable list, or a drawable")

        for drawable in drawables:
            for texture in drawable.render.textures:
                # If this is a dynamic texture and we have a renderstate to hand it, do so
                if self.dynTexRenderState and hasattr(texture, 'hasRenderState'):
                    if not texture.hasRenderState():
                        texture.attachRenderState(self.dynTexRenderState)

            drawable.parent(object)
        self.objects.setdefault(object, []).extend(drawables)
        self.dirty = True

    def preprocess(self):
        """Rebuilds rendering passes. This operation clears the 'dirty' flag, and
           is called automatically when a dirty scene needs rendering.
           """
        # Sort the rendering passes by decreasing render priority
        self.passes.sort(lambda a,b: cmp(b.renderPriority, a.renderPriority))

        # Make a list of rendering passes sorted by filter priority
        filterPasses = self.passes[:]
        filterPasses.sort(lambda a,b: cmp(b.filterPriority, a.filterPriority))

        # Clean out each rendering pass
        for rpass in self.passes:
            rpass.erase()

        # Divy up the drawables into rendering passes using the passes' filter functions
        for object, drawables in self.objects.iteritems():
            for drawable in drawables:
                for rpass in filterPasses:
                    if rpass.filter(drawable):
                        rpass.add(drawable)
                        break

        # Give each rendering pass a chance to preprocess
        for rpass in self.passes:
            rpass.preprocess()

        self.dirty = False

    def render(self, rstate):
        """Render the scene to the current OpenGL context"""
        if self.dirty:
            self.preprocess()

        glDisable(GL_BLEND)
        glEnable(GL_DEPTH_TEST)
        glEnable(GL_CULL_FACE)
        glEnable(GL_COLOR_MATERIAL)
	glEnable(GL_LIGHTING)
        glDisable(GL_LINE_SMOOTH)
        glColor4f(1,1,1,1)

        for rpass in self.passes:
            if not rpass.isEmpty():
                rpass.render(rstate)

    def pick(self, rstate, pos):
        """Returns the nearest scene object that was rendered at the given screen
           coordinates. If no object was rendered at this postion, returns None.
           """
        # Flip the Y axis to convert from the traditional top-left-based coordinate
        # system to OpenGL's bottom-left-based coordinate system.
        viewport = rstate.viewport
        pos = (pos[0], viewport.size[1] - pos[1])

        glSelectBuffer(len(self.objects) * 4)
        glRenderMode(GL_SELECT)
        glInitNames()
        glPushName(0)

        glMatrixMode(GL_PROJECTION)
        glPushMatrix()
        glLoadIdentity()
        gluPickMatrix(pos[0], pos[1], 5, 5, viewport.rect)
        viewport.setProjectionMatrix()
        glMatrixMode(GL_MODELVIEW)

        rstate.picking = PickingState()
        for rpass in self.passes:
            rpass.render(rstate)

        glMatrixMode(GL_PROJECTION)
        glPopMatrix()
        glMatrixMode(GL_MODELVIEW)

        hits = glRenderMode(GL_RENDER)
        if len(hits) > 0:
            depth = hits[0][1]
            choose = hits[0][2]
            for hit in hits:
                if hit[1] < depth:
                    depth = hit[1]
                    choose = hit[2]
            return rstate.picking.names[choose[0]].object
        return None


class View:
    """A generic 3D view with multiple rendering passes, usable with OpenGLViewport."""
    def __init__(self, viewport, scene=None):
        if not scene:
            scene = Scene()
        self.viewport = viewport
        self.camera = Camera()
        self.scene = scene
        viewport.onDrawFrame.observe(self.render)
        self.initLighting()
        if not scene.hasMainView():
            scene.setMainView(self)

    def initLighting(self):
        self.lights = []
        for light in xrange(glGetIntegerv(GL_MAX_LIGHTS)):
            self.lights.append(Light(GL_LIGHT0 + light))
        self.defaultLighting()

    def resetLighting(self):
        for light in self.lights:
            light.reset()
        glLightModelfv(GL_LIGHT_MODEL_AMBIENT, (0, 0, 0, 1.0))

    def defaultLighting(self):
        self.resetLighting()

        self.lights[0].enabled  = True
        self.lights[0].ambient  = (0.25, 0.25, 0.25, 1.0)
        self.lights[0].diffuse  = (0.65, 0.65, 0.65, 1.0)
        self.lights[0].position = (300, 400, 400, 1.0)

        self.lights[1].enabled  = True
        self.lights[1].ambient  = (0.05, 0.05, 0.05, 1.0)
        self.lights[1].diffuse  = (0.85, 0.85, 0.85, 1.0)
        self.lights[1].position = (0, 0, 400, 1.0)

    def render(self):
        """The main entry point for rendering"""
        self.camera.load()
        self.renderScene(Drawable.RenderState(self))

    def renderScene(self, rstate):
        """Set up lighting and render the scene. This is called
           when the camera has already been set up elsewhere.
           """
        for light in self.lights:
            light.set()
        self.scene.render(rstate)

    def pick(self, pos):
        self.camera.load()
        return self.scene.pick(Drawable.RenderState(self), pos)

### The End ###
