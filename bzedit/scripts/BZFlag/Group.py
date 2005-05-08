""" BZFlag.Group

Subclass of BZFlag.Object implementing a group instance.
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
import Blender

class Group(Box):
    type = 'group'

    def __init__(self, list=None):
        Box.__init__(self, None)
        self.set_size([1.0, 1.0, 1.0])
        if list is not None:
            self.set_instance(list[1])

    def set_instance(self, name):
        self.instance = name
        self.name = name+'_instance'

    def createBlenderObject(self):
        obj = Blender.Object.New('Empty')
        Blender.Scene.GetCurrent().link(obj)
        return obj
