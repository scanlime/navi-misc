#
# Reader.py - parses bzflag world files
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

from pyparsing import *

class Reader:
    grammar = None

    def getGrammar (self):
        if self.grammar is None:
            comment = Literal('#') + Optional (restOfLine)
            float = Combine(Word('+-'+nums, nums) +
                            Optional(Literal('.') + Optional(Word(nums))) +
                            Optional(CaselessIteral('E') + Word('+-'+nums, nums)))
            3dPoint = float + float + float

            name = CaselessLiteral("name")
            pos = CaselessLiteral("pos") | CaselessLiteral("position")
            rot = CaselessLiteral("rot") | CaselessLiteral("rotation")
            scale = CaselessLiteral("scale")
            shear = CaselessLiteral("shear")
            shift = CaselessLiteral("shift")
            size = CaselessLiteral("size")
            spin = CaselessLiteral("spin")

            object = name + Word(alphanums)
            location =
                pos + 3dPoint
              | size + 3dPoint
              | rot + float
              | shift + 3dPoint
              | scale + 3dPoint
              | shear + 3dPoint
              | spin + 3dPoint
              | object
