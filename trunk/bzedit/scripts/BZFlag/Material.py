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
import string

class Material(Object.Object):
    type = 'material'

    def __init__(self, list=None):
        self.material = Blender.Material.New()
        if list is not None:
            for property in list[1:]:
                try:
                    getattr(self, "set_%s" % property[0])(property[1:])
                except Exception, e:
                    print e

    def toBlender(self):
        pass

    def set_name(self, name):
        if type(name) is str:
            self.material.setName(name)
        else:
            self.material.setName(string.join(name, ' '))

    def set_texture(self, texture):
        pass

    def set_ambient(self, ambient):
        pass

    def set_diffuse(self, diffuse):
        pass

    def set_emission(self, emission):
        # FIXME - need to set emission color
        if len(emission) == 3:
            self.material.setEmit(1.0)
        else:
            self.material.setEmit(float(emission[3]))

    def set_specular(self, specular):
        if (len(specular) == 4):
            self.material.setSpecCol([float(n) for n in specular[:3]])
        else:
            self.material.setSpecCol([float(n) for n in specular])

    def set_shininess(self, shininess):
        self.material.setSpec(float(shininess[0]) / 128)

    def set_texmat(self, texmat):
        pass

    def set_notexalpha(self, notexalpha):
        pass

    def set_color(self, color):
        pass

    def set_notextures(self, notextures):
        pass

    def set_addTexture(self, addtexture):
        pass

    def set_dyncol(self, dyncol):
        pass
