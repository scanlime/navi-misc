""" BZEngine.UI.Environment

Scene objects and coordination classes for the environment our
BZEngine world is placed in. This includes the sky, and other possible
atmospheric effects like rain.
"""
#
# Python BZEngine Package
# Copyright (C) 2003-2004 Micah Dowty <micahjd@users.sourceforge.net>
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
from BZEngine.UI import Drawable, GLExtension
from BZEngine import Animated


class Sky:
    """A scene object representing the sky. This provides a getDrawables() interface
       so it can be added directly to a scene, and it coordinates the actions of each
       """
    def __init__(self):
        self.time = Animated.Timekeeper()
        self.drawables = []

        # The color gradient acting as the background for the sky
        self.drawables.append(Drawable.Sky.Colors(self))

        # Nifty clouds and mountains, but only if we support multitexture
        if GLExtension.multitexture:
            self.drawables.append(Drawable.Sky.Clouds(self))
            self.drawables.append(Drawable.Sky.Mountains(self))

        # Slightly shaded void below the horizon
        self.drawables.append(Drawable.Sky.Void(self))

        self.update()

    def getDrawables(self):
        return self.drawables

    def getTweakControls(self):
        import Tweak
        return (
            Tweak.Quantity(self, 'unitDayTime'),
            )

    def update(self):
        # Calculate the time of day, scaled from 0 to 1.
        # For now this just loops the day fairly fast so it's easy to see what's happening.
        # 'period' is the length of a day in seconds.
        period = 120
        self.unitDayTime = (self.time.time() % period) / period

### The End ###
