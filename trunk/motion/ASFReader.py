#!/usr/bin/env python
#
# ASFReader.py: parses Acclaim ASF (skeleton) files
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
from Bone import *

def listToDict(list):
    s = {}
    for item in list:
        if (len(item) == 2):
            s[item[0]] = item[1]
        else:
            s[item[0]] = item[1:]
    return s

class ASFReader:
    bones = {}
    order = []
    axis = None
    position = []
    orientation = []
    grammar = None
    units = {}
    hierarchy = {}

    def gotRoot(self, s, loc, toks):
        s = listToDict(toks[0])
        self.orientation = s['orientation']
        self.position    = s['position']
        self.order       = s['order']
        self.axis        = s['axis']
    def gotUnit(self, s, loc, toks):
        self.units = listToDict(toks[0])
    def gotBone(self, s, loc, toks):
        dict = listToDict(toks[0])
        bone = Bone(dict)
        self.bones[bone.name] = bone

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
            docLine = Combine(OneOrMore(Word(alphanums + '!"#$%&\'()*+,-./;<=>?@[\\]^_`{|}~')), joinString=' ', adjacent=False)
            sectstart = LineStart() + Literal(':').suppress()
            section = Group(
                sectstart + "version" + float \
              | sectstart + "name" + Word(printables) \
              | sectstart + "units" + unit \
              | sectstart + "documentation" + docLine \
              | sectstart + "root" + root \
              | sectstart + "bonedata" + bonedata \
              | sectstart + "hierarchy" + docLine
              )
            part = section | Suppress(comment)
            self.grammar = OneOrMore(part)

            root.setParseAction(self.gotRoot)
            unit.setParseAction(self.gotUnit)
            bone.setParseAction(self.gotBone)

        return self.grammar

    def parse(self, filename):
        g = self.getGrammar()
        g.parseFile(filename)
        file = open(filename)
        lines = file.readlines()
        for i in range(len(lines)):
            if lines[i][:10] == ':hierarchy':
                break
        i += 2
        for i in range(i, len(lines)):
            l = lines[i].strip(' \n\r')
            if l == 'end':
                break
            s = l.split(' ')
            self.hierarchy[s[0]] = s[1:]
        file.close()

# main program
reader = ASFReader()
reader.parse('02.asf')
print 'bones:'
for bone in reader.bones.keys():
    print '\t',bone
