""" BZFlag.UI.OverheadView

A 2D overhead view of the game, implemented using pygame.
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

import math, BZFlag, colorsys
import BZFlag
from BZFlag import Event


colorScheme = {
    'background':    (88,  112, 88 ),
    'outline':       (0,   0,   0  ),
    'Box':           (147, 147, 131),
    'Pyramid':       (130, 130, 145),
    'Wall':          (255, 255, 0  ),
    'Teleporter':    (255, 255, 128),
    'playerOutline': (255, 255, 255),
    'flag':          (255, 255, 255),
    'player':        (0,   0,   0  ),
    'redPlayer':     (128, 0,   0  ),
    'greenPlayer':   (0,   128, 0  ),
    'bluePlayer':    (0,   0,   128),
    'purplePlayer':  (96,  0,   144),
    'rabbitPlayer':  (128, 128, 128),
    'baseOutline':   (0  , 0  , 0  ),
    'redBase':       (128, 0,   0  ),
    'greenBase':     (0,   128, 0  ),
    'blueBase':      (0,   0,   128),
    'purpleBase':    (96,  0,   144),
    }


def coordWorldToView(size, world, point):
    """Convert world coordinates to view coordinates in pixels"""
    return (( point[0] * 0.98 / world.size[0] + 0.5) * size[0],
            (-point[1] * 0.98 / world.size[1] + 0.5) * size[1])


def worldToImage(world, size, oversample=2, colors=colorScheme):
    """Render a world to a PIL image, using oversampling
       to overcome PIL's lack of antialiased primitives.
       """
    import Image, ImageDraw
    img = Image.new("RGB", (size[0]*oversample, size[1]*oversample), colors['background'])
    draw = ImageDraw.Draw(img)
    toView = lambda point: coordWorldToView(img.size, world, point)

    # Z-sort the world blocks and remember the highest object
    sorted = filter(lambda b: hasattr(b,'center'), world.blocks)
    def zSort(a, b):
        return cmp(a.center[2], b.center[2])
    sorted.sort(zSort)

    # Use the world's mean height as a reference point for coloring
    total = 0
    for object in sorted:
        total += object.center[2]
    heightRef = total / len(sorted)

    for object in sorted:
        objClassName = object.__class__.__name__
        if objClassName == 'TeamBase':
            # Color the bases by team
            draw.polygon(map(toView, object.toPolygon()),
                         outline = colors['baseOutline'],
                         fill = colors[object.team + 'Base'])
        else:
            # Default representation
            color = colorScheme.get(objClassName, None)
            if color:
                poly = map(toView, object.toPolygon())
                # Shade the color according to height
                hsv = list(colorsys.rgb_to_hsv(color[0]/255.0,
                                               color[1]/255.0,
                                               color[2]/255.0))
                if heightRef:
                    hsv[2] += object.center[2] / heightRef * 0.1
                color = colorsys.hsv_to_rgb(*hsv)
                color = (color[0]*255, color[1]*255, color[2]*255)
                draw.polygon(poly, fill=color, outline=colors['outline'])

    return img.resize(size, Image.ANTIALIAS)


class OverheadView:
    """Shows an overhead view of the BZFlag game, renderable to a pygame
       viewport. Note that this observes the game state rather than the
       client, so it should be usable on either client or server side.
       """
    def __init__(self, game, viewport):
        self.game = game
        self.viewport = viewport
        self.resetCache()

        # When the world is reloaded, invalidate our cached rendering of it
        game.world.onLoad.observe(self.resetCache)

        # Set ourselves up to render to the given viewport
        viewport.setCaption("%s Overhead View" % BZFlag.name)
        viewport.onDrawFrame.observe(self.render)

    def resetCache(self):
        self.cachedWorld = None

    def worldToView(self, point):
        return coordWorldToView(self.size, self.game.world, point)

    def renderWorld(self, surface):
        """Render the normally-static world to the given surface. This uses worldToImage
           to draw a fairly high quality image using PIL, then converts it to a
           pygame surface.
           """
        import pygame
        img = worldToImage(self.game.world, self.size)
        surface.blit(pygame.image.fromstring(img.tostring(), img.size, "RGB"), (0,0))

    def render(self):
        """Render the overhead view to the given surface. This includes the
           game world, with transient objects such as players and flags overlaid on top.
           """
        import pygame
        surface = self.viewport.screen
        self.size = surface.get_size()

        # Since the game world doesn't usually change, we cache a rendered version of it.
        if (not self.cachedWorld) or self.cachedWorld.get_size() != surface.get_size():
            self.cachedWorld = pygame.Surface(surface.get_size()).convert()
            self.renderWorld(self.cachedWorld)
        surface.blit(self.cachedWorld, (0,0))

        # Now for the objects that change often
        self.renderFlags(surface)
        self.renderPlayers(surface)
        self.renderShots(surface)

    def renderPlayers(self, surface):
        import pygame
        color = colorScheme['playerOutline']
        for player in self.game.players.values():
            if 'alive' in player.status:
                # Pick player color based on team
                try:
                    bg = colorScheme[player.identity.team + "Player"]
                except KeyError:
                    bg = colorScheme["player"]

                # Draw a circle with black background and colored outline,
                # with a line indicating the tank's current heading.
                pos = self.worldToView(player.motion.position)
                height = player.motion.position[2]
                if height < 0:
                    height = 0
                size = height/4 + 6
                radians = -player.motion.azimuth * math.pi / 180
                heading = (pos[0] + math.cos(radians)*size,
                           pos[1] + math.sin(radians)*size)
                pygame.draw.circle(surface, bg, pos, size-1)
                pygame.draw.circle(surface, color, pos, size, 1)
                pygame.draw.line(surface, color, pos, heading)

    def renderFlags(self, surface):
        import pygame
        color = colorScheme['flag']
        for flag in self.game.flags.values():
            if 'onGround' == flag.status:
                # Draw a little cross
                radius = 2
                pos = self.worldToView(flag.motion.position)
                pygame.draw.line(surface, color, (pos[0]-radius, pos[1]), (pos[0]+radius, pos[1]))
                pygame.draw.line(surface, color, (pos[0], pos[1]-radius), (pos[0], pos[1]+radius))

    def renderShots(self, surface):
        pass


class Setup:
    def __init__(self, game, eventLoop):
        from BZFlag.UI.Viewport import PygameViewport
        self.viewport = PygameViewport(eventLoop, (512,512))
        self.view = OverheadView(game, self.viewport)

### The End ###
