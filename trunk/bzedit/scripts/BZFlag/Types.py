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
from Material import Material
from Mesh import Mesh
from Pyramid import Pyramid
from Teleporter import Teleporter
from World import World

typeMap = {
    'arc'           : None,
    'base'          : None,
    'box'           : Box,
    'cone'          : None,
    'define'        : None,
    'dynamicColor'  : None,
    'link'          : None,
    'material'      : Material,
    'mesh'          : Mesh,
    'meshbox'       : None,
    'meshpyr'       : None,
    'options'       : None,
    'physics'       : None,
    'pyramid'       : Pyramid,
    'sphere'        : None,
    'teleporter'    : Teleporter,
    'tetra'         : None,
    'textureMatrix' : None,
    'transform'     : None,
    'waterLevel'    : None,
    'weapon'        : None,
    'world'         : None,
    'zone'          : None,
}
