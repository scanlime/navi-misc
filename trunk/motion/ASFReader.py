#
# ASFReader.py: parses acclaim ASF (skeleton) files
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

asfbnf = None
def asf_BNF():
    global asfbnf
    if not asfbnf:
        float = Word('0123456789.e+-')
        comment = Literal('#') + Optional (restOfLine)

        axisOrder = oneOf("XYZ XZY YXZ YZX ZXY ZYX")
        ro = oneOf("TX TY TZ RX RY RZ")

        rootElement = \
            "order" + ro + ro + ro + ro + ro + ro \
          | "axis" + axisOrder \
          | "position" + float + float + float \
          | "orientation" + float + float + float

        unitElement = \
            "mass" + float \
          | "length" + float \
          | "angle" + oneOf("deg rad")

        dof = oneOf("rx ry rz")
        triplet = Literal('(').suppress() + float + float + Literal(')').suppress()

        boneElement = \
            "id" + Word(nums) \
          | "name" + Word(alphanums) \
          | "direction" + float + float + float \
          | "length" + float \
          | "axis" + float + float + float + axisOrder \
          | "dof" + oneOrMore(dof) \
          | "limits" + oneOrMore(triplet)
        bone = "begin" + oneOrMore(boneElement) + "end"
        bonedata = zeroOrMore(bone)

        sectstart = LineStart() + Literal(':').suppress()
        section = \
            sectstart + "version" + float \
          | sectstart + "name" + Word(printables) \
          | sectstart + "units" + oneOrMore(unitElement) \
          | sectstart + "documentation" + zeroOrMore(docLines) \
          | sectstart + "root" + oneOrMore(rootElement) \
          | sectStart + "bonedata" + bonedata \
          | sectStart + "hierarchy" + hierarchy
    return asfbnf

file = open("02.asf")
text = ''.join(file.readlines())
