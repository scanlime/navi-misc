""" BZEngine.UI.RadarView

An overhead view implemented using OpenGL, optionally tracking
the position and orientation of one player.
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

import pygame, math, BZEngine
from pygame.locals import *
from OpenGL.GL import *
from OpenGL.GLU import *
from BZEngine import Event
from BZEngine.UI import Drawable, GLExtension

# Maximum number of radar laysrs per color, trades off Z accuracy for speed
numRadarLayers = 8


# Color scheme for radar. Objects not found specifically default to None
colorScheme = {
    None:           (0.38, 0.62, 0.62),
    'Teleporter':   (1,    1,    0   ),
    'player':       (1,    1,    1   ),
    'redPlayer':    (1,    0,    0   ),
    'greenPlayer':  (0,    1,    0   ),
    'bluePlayer':   (0,    0,    1   ),
    'purplePlayer': (0.5,  0,    1   ),
    }


def smoothedPoly(poly):
    """Render a polygon with smoothed edges. This uses GL_POLYGON to render the
       interior without cracks between triangles, then GL_LINE_LOOP to render
       the edges smoothly.
       """
    glBegin(GL_POLYGON)
    for vertex in poly:
        glVertex2f(*vertex)
    glEnd()
    glBegin(GL_LINE_LOOP)
    for vertex in poly:
        glVertex2f(*vertex)
    glEnd()


def colorScale(z, viewHeight):
    """This is the Enhanced Radar (tm) color scaling algorithm from bzflag proper,
       simplified a little to make the display list optimization possible.
       """
    alpha = 1.0 - abs(viewHeight - z) / 40.0
    if alpha < 0.35:
        alpha = 0.35
    return alpha


class RadarLayer(Drawable.DisplayList):
    """To achieve the enhanced radar effect, the world is broken up into
       several slices each showing a different range of Z values. These
       slices are then blended together at different alpha values.
       This class represents one slice that draws a list of pre-sliced
       world objects.
       """
    def set(self, color, z, objects):
        self.color = color
        self.z = z
        self.objects = objects

    def drawToList(self, rstate):
        for object in self.objects:
            smoothedPoly(object.toPolygon())


class RegularPolygon(Drawable.DisplayList):
    """A display-listed regular polygon, used to make dots on the map.
       Radius is 1 and polygon is at the origin. The drawAt() function
       scales these at render-time.
       """
    def set(self, sides):
        self.sides = sides

    def drawToList(self, rstate):
        points = []
        sideRadians = math.pi * 2 / self.sides
        for side in xrange(self.sides):
            theta = side * sideRadians
            points.append((math.cos(theta), math.sin(theta)))
        smoothedPoly(points)

    def drawAt(self, rstate, point, radius):
        glTranslatef(point[0], point[1], 0)
        glScalef(radius, radius, 1)
        self.draw(rstate)


class Scene:
    """Manages the internal representation of the world after loading.
       For the radar, this means a set of OpenGL display lists representing
       2D views of the world at different heights. These will all be
       drawn with different alpha values to achieve the enhanced radar effect.
       """
    def __init__(self, game):
        self.game = game
        game.world.onLoad.observe(self.reloadWorld)
        self.reloadWorld()

    def reloadWorld(self):
        """Z-sorts the world and divides it up into layers"""

        # First sort all visible objects by color
        colors = {}
        for object in self.game.world.blocks:
            if hasattr(object, 'center'):
                try:
                    color = colorScheme[object.__class__.__name__]
                except KeyError:
                    color = colorScheme[None]
                try:
                    colors[color].append(object)
                except KeyError:
                    colors[color] = [object]

        # Z-sort all the objects in each color
        def zSort(a, b):
            return cmp(a.center[2], b.center[2])
        for colorGroup in colors.values():
            colorGroup.sort(zSort)

        # Decompose each color group into a number of layers,
        # depending on the range of Z values present.
        self.layers = []
        for color, colorGroup in colors.items():
            minZ = colorGroup[0].center[2]
            maxZ = colorGroup[-1].center[2]
            layerHeight = (maxZ - minZ) / numRadarLayers
            layerObjects = []
            currentLayerZ = minZ

            for object in colorGroup:
                if object.center[2] > currentLayerZ + layerHeight:
                    # Exceeded the current layer's height, get a new layer
                    self.layers.append(RadarLayer(color, currentLayerZ, layerObjects))
                    layerObjects = []
                    currentLayerZ += layerHeight
                layerObjects.append(object)
            # New layer with remaining objects
            self.layers.append(RadarLayer(color, currentLayerZ, layerObjects))

    def render(self, rstate, viewHeight=0):
        for layer in self.layers:
            color = layer.color
            glColor4f(color[0], color[1], color[2], colorScale(layer.z, viewHeight))
            layer.draw(rstate)


class RadarView:
    """An overhead view implemented using OpenGL, optionally tracking
       the position and orientation of one player.
       """
    def __init__(self, game, viewport):
        self.scene = Scene(game)
        self.game = game
        self.viewport = viewport
        self.zoom = 0.95
        self.center = [0,0,0]
        self.angle = 0
        self.follow = None
        viewport.fov = None
        self.dot = RegularPolygon(8)

        viewport.setCaption("%s Radar View" % BZEngine.name)
        viewport.onDrawFrame.observe(self.render)

    def renderPlayers(self, rstate):
        # XXX - setting self.follow here is just for testing
        self.follow = None
        for player in self.game.players.values():
            if 'alive' in player.status:
                # XXX - same here
                self.follow = player

                # Pick player color based on team, alpha based on height
                try:
                    color = colorScheme[player.identity.team + "Player"]
                except KeyError:
                    color = colorScheme["player"]
                pos = player.motion.position
                glColor4f(color[0], color[1], color[2], colorScale(pos[2], self.center[2]))
                self.dot.drawAt(rstate, pos, 5)

    def updateFollowing(self):
        if self.follow:
            self.center = self.follow.motion.position
            self.angle  = -self.follow.motion.azimuth + 90
            self.zoom   = 2

    def render(self):
        self.updateFollowing()
        rstate = Drawable.RenderState(self)

        glEnable(GL_BLEND)
        glDisable(GL_LIGHTING)
        glDisable(GL_CULL_FACE)
        glDisable(GL_COLOR_MATERIAL)
        glDisable(GL_DEPTH_TEST)
        GLExtension.disableMultitex()
        glDisable(GL_TEXTURE_2D)
        glEnable(GL_LINE_SMOOTH)

        size = self.viewport.size
        glPushMatrix()
        glScalef(size[0], size[1], 1)
        glTranslatef(0.5, 0.5, 0)
        glScalef( float(self.zoom) / self.game.world.size[0],
                 -float(self.zoom) / self.game.world.size[1], 1)
        glRotatef(self.angle, 0,0,1)
        glTranslatef(-self.center[0], -self.center[1], 0)
        self.scene.render(rstate, self.center[2])
        self.renderPlayers(rstate)
        glPopMatrix()


class Setup:
    def __init__(self, game, eventLoop):
        from BZEngine.UI.Viewport import OpenGLViewport
        self.viewport = OpenGLViewport(eventLoop, (512,512))
        self.view = RadarView(game, self.viewport)

### The End ###
