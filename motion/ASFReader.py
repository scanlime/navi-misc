#!/usr/bin/env python
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

class ASFReader:
    bones = {}
    order = []
    axis = None
    position = []
    orientation = []
    grammar = None

    def gotRoot(self, s, loc, toks):
        print 'root:\t',toks[0]
    def gotUnit(self, s, loc, toks):
        print 'units:\t',toks[0]
    def gotBone(self, s, loc, toks):
        print 'bone:\t',toks[0]

    def getGrammar(self):
        if self.grammar is None:
            float = Word(nums + '.e+-')
            comment = Literal('#') + Optional (restOfLine)
            axisOrder = oneOf("XYZ XZY YXZ YZX ZXY ZYX")
            ro = oneOf("TX TY TZ RX RY RZ")
            rootElement = Group(
                "order" + ro + ro + ro + ro + ro + ro \
              | "axis" + axisOrder \
              | "position" + float + float + float \
              | "orientation" + float + float + float
              )
            root = Group(OneOrMore(rootElement))
            unitElement = Group("mass" + float | "length" + float | "angle" + oneOf("deg rad"))
            unit = Group(OneOrMore(unitElement))
            dof = oneOf("rx ry rz")
            triplet = Literal('(').suppress() + float + float + Literal(')').suppress()
            boneElement = Group(
                "id" + Word(nums) \
              | "name" + Word(alphanums) \
              | "direction" + float + float + float \
              | "length" + float \
              | "axis" + float + float + float + axisOrder \
              | "dof" + OneOrMore(dof) \
              | "limits" + OneOrMore(triplet)
              )
            bone = Suppress("begin") + Group(OneOrMore(boneElement)) + Suppress("end")
            bonedata = ZeroOrMore(bone)
            hierElement = Group(OneOrMore(Word(alphanums)))
            hierarchy = Suppress("begin") + OneOrMore(hierElement)
            docLine = Combine(OneOrMore(Word(alphanums + '!"#$%&\'()*+,-./;<=>?@[\\]^_`{|}~')), joinString=' ', adjacent=False)
            sectstart = LineStart() + Literal(':').suppress()
            section = Group(
                sectstart + "version" + float \
              | sectstart + "name" + Word(printables) \
              | sectstart + "units" + unit \
              | sectstart + "documentation" + docLine \
              | sectstart + "root" + root \
              | sectstart + "bonedata" + bonedata \
              | sectstart + "hierarchy" + hierarchy
              )
            part = section | Suppress(comment)
            self.grammar = OneOrMore(part)

            root.setParseAction(self.gotRoot)
            unit.setParseAction(self.gotUnit)
            bone.setParseAction(self.gotBone)

        return self.grammar

# main program
reader = ASFReader()
asfFile = reader.getGrammar()
x = asfFile.parseFile('02.asf')

s = {}
for section in x:
    s[section[0]] = section[1:]

print s.keys()
