""" BZFlag.UI.Drawable.Base

Class for drawing the team bases in the world
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
from BZFlag import Animated
from DisplayList import *
from OpenGL.GL import *
import Box, Numeric


colorScheme = {
    'red':    (1,   0.5, 0.5),
    'green':  (0.7, 1,   0.7),
    'blue':   (0.7, 0.7, 1),
    'purple': (1,   0.5, 0.8),
    }


class BaseDecal(Box.TopDecal):
    """This is the fixed portion of the base graphics, sort of a futuristic metal frame.
       It is colored to indicate the base's team, but has no animation.
       """
    
    textureName = 'base_fixed.png'

    def __init__(self, base):
        Box.TopDecal.__init__(self, base)

    def set(self, base):
        Box.TopDecal.set(self, base)
        self.color = colorScheme[base.team]

    def drawToList(self, rstate):
        # Disable lighting (since this decal is supposed to be glowing),
        # tint it to the team's color, and animate the color's intensity
        glDisable(GL_LIGHTING)
        glColor3f(*self.color)
        Box.TopDecal.drawToList(self, rstate)
        glEnable(GL_LIGHTING)
        glColor3f(1,1,1)


class BaseRotorDecal(Box.FixedTopDecal):
    """This is the center of the base decal, with animated rotation and lighting"""
    textureName = 'base_rotor.png'

    def __init__(self, base):
        Box.FixedTopDecal.__init__(self, base, min(base.size[0], base.size[1]) * 1.3)
        self.time = Animated.Timekeeper()
        self.color = Numeric.array(colorScheme[base.team])
        self.render.textures[0].setRepeat(GL_CLAMP, GL_CLAMP)

        # We still use display lists, but can't be cached in one because we do per-frame animation
        self.render.static = False

        # Animted rotor angle
        self.rotorAngle = Animated.Value(Animated.RampFunction(5, (0, 360)))

        # Animated glowing light intensity thingy
        self.intensity = Animated.Value(Animated.SineFunction(1, (0.9, 1.5)))

    def draw(self, rstate):
        dt = self.time.step()
        self.rotorAngle.integrate(dt)
        self.intensity.integrate(dt)
        glDisable(GL_LIGHTING)

        glColor3f(*tuple(self.color * self.intensity.value))

        # Use the texture matrix to rotate our decal around the texture's center
        glMatrixMode(GL_TEXTURE)
        glTranslatef(0.5, 0.5, 0)
        glRotatef(self.rotorAngle, 0,0,1)
        glTranslatef(-0.5, -0.5, 0)
        glMatrixMode(GL_MODELVIEW)

        Box.FixedTopDecal.draw(self, rstate)

        glEnable(GL_LIGHTING)
        glMatrixMode(GL_TEXTURE)
        glLoadIdentity()
        glMatrixMode(GL_MODELVIEW)
        glColor3f(1,1,1)

 
def detectBaseDrawables(base):
    """Given a base WorldObject, return a list of the drawables that should be used
       to represent it. This starts with a box, then adds some decorations to make it a base.
       """
    # This is a hack bzflag uses since the Z component of base size isn't sent over the wire.
    # if a base is not on the ground and it's height is zero, set the height to 1.
    if base.center[2] > 0 and base.size[2] == 0:
        base.size[2] = 1
    
    drawables = Box.detectBoxDrawables(base)
    
    drawables.append(BaseDecal(base))
    drawables.append(BaseRotorDecal(base))
        
    return drawables

### The End ###
