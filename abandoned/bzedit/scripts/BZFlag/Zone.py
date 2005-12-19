""" BZFlag.Zone

BZFlag.Object implementing zones.
"""
#
# Copyright (C) 2005 David Trowbridge
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
from Box import Box
import Object
import Blender

class Zone(Box):
    type = 'zone'

    def __init__(self, list=None):
        self.set_mode()
        self.set_flag()
        self.set_team()
        self.set_safety()
        Box.__init__(self, list)

    def set_mode(self, mode='flag'):
        self.mode = mode

    def set_flag(self, flag=['']):
        self.flag = flag

    def set_team(self, team=['']):
        self.team = team

    def set_safety(self, safety=['']):
        self.safety = safety
