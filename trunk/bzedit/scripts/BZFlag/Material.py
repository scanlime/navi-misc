""" BZFlag.Material

BZFlag.Object providing a 'material'
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
import Object
import Blender

class Material(Object.Object):
    type = 'material'

    def __init__(self, list=None):
        self.material = Blender.Material.New()
        if list is not None:
            for property in list[1:]:
                getattr(self, "set_%s" % property[0])(property[1:])

    def set_texture(self, texture):
        pass

    def set_ambient(self, ambient):
        pass

    def set_diffuse(self, diffuse):
        pass

    def set_specular(self, specular):
        pass

    def set_shininess(self, shininess):
        pass
