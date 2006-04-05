#
# Bone.py: Simple class defining a bone
#
# Copyright (C) 2005-2006 David Trowbridge
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

class Bone:
    name = None
    id = None
    direction = []
    dof = []
    limits = []
    axis = []

    def __init__ (self, dict):
        self.name = dict['name']
        self.id = int (dict['id'])
        self.length = float (dict['length'])
        self.direction = dict['direction']
        self.axis = dict['axis'][:3]
        try:
            self.dof = dict['dof']
            self.limits = dict['limits']
            # Our parser will automatically convert any singular value to that value,
            # but we want a tuple. Test to make sure that we've really got a tuple.
            if type (self.dof) is str:
                self.dof = [self.dof]
                self.limits = [self.limits]
        except KeyError:
            self.dof = None
            self.limits = None
