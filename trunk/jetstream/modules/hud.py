#
# hud.py - A Heads Up Display based on Widget Templates
#
# Copyright (C) 2002-2004 Micah Dowty and David Trowbridge
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License
# as published by the Free Software Foundation; either version 2
# of the License, or (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
#

import PicoGUI

class Hud:
    def __init__(self, game):
        self.game = game
        self.app = game.app
        # Load our widget template, importing widgets from it
        self.template = self.app.newTemplate(open("data/hud.wt").read())
        self.inst = self.template.instantiate([
            'Velocity',
            'Lasers',
            ])

    def setVelocity(self, v):
        self.inst.Velocity.text = str(v)

    def setLasers(self, lasers):
        self.inst.Lasers.text = str(lasers)

    def hide(self):
        self.template.destroy()
