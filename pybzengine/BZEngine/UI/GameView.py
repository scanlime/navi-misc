""" BZFlag.UI.GameView

A frontend for running several views at once to provide
the full in-game experience.
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

from BZFlag.UI import Viewport, ThreeDView, ThreeDControl, RadarView, Layout, HUD
import BZFlag, sys
from BZFlag import Animated


class Setup:
    def __init__(self, game, eventLoop):
        self.viewport = Viewport.OpenGLViewport(eventLoop)
        self.time = Animated.Timekeeper()
        padding = 10
        fullHudSize = 0.3

        # 3D view in the root viewport
        self.view = ThreeDView.ThreeDView(game, self.viewport)
        self.control = ThreeDControl.Viewing(self.view, self.viewport)
        remaining = Layout.Rect(self.viewport).margin(padding)

        # HUD panel along the bottom of the screen, with animated resize
        self.hudSize = Animated.Value(Animated.SigmoidApproach(fullHudSize, 15, (0, fullHudSize)),
                                      fullHudSize)
        self.inMotion = False
        self.viewport.onSetupFrame.observe(self.setupFrame)
        self.viewport.onKeyDown.observe(self.keyDown)

        (remaining, hudRect) = remaining.hSplit(lambda r: (1-self.hudSize.value) * r[3])
        hudPanel = self.viewport.region(hudRect)
        self.panel = HUD.Panel(hudPanel)

        # Stick a radar in the left side of the HUD panel. The lambda expression
        # lazily evaluates the panel's height and uses it as a width for the left
        # split, keeping the radar square. The rest of the panel gets used for messages.
        (radarRect, messageRect) = Layout.Rect(hudPanel).vSplit(lambda r: r[3])
        self.radar = RadarView.RadarView(game, hudPanel.region(radarRect.margin(2)))

        # Redirect our stdout to this text panel
        sys.stdout = HUD.ScrolledText(hudPanel.region(messageRect.margin(2)))
        print BZFlag.about

        # Logo-thingy!
        self.logo = HUD.Text(self.viewport.region(remaining), BZFlag.name,
                             shadow=True, color=(1,1,0,1), fontSize=35)

    def setupFrame(self):
        self.hudSize.integrate(self.time.step())

    def keyDown(self, event):
        if event.unicode == " ":
            if self.hudSize.f.target == fullHudSize:
                self.hudSize.f.target = 0
            else:
                self.hudSize.f.target = fullHudSize

### The End ###
