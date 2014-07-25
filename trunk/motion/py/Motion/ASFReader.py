#!/usr/bin/env python
"""An ASF file parser.

Utilities for parsing an ASF file.

Classes:
    - ASFReader     The ASF parser

Functions:
    - listToDict    Create a dictionary from a list
"""
#
# ASFReader.py: parses Acclaim ASF (skeleton) files
#
# Copyright (C) 2005-2007 David Trowbridge
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
import Log

def listToDict (list):
    """Create a dictionary from a list.

    Create a list using the the first item in the list as the key, and the rest
    of the list as the item.
    """
    s = {}
    for item in list:
        # If we've only got one item, use the item rather than a list
        if (len (item) == 2):
            s[item[0]] = item[1]
        else:
            s[item[0]] = item[1:]
    return s

# FIXME: bleh, this shouldn't be here
log = Log.Logger ()

class ASFReader:
    bones = {}
    order = []
    axis = None
    position = []
    name = None
    orientation = []
    grammar = None
    units = {}
    hierarchy = []

    def gotRoot (self, s, loc, toks):
        s = listToDict (toks[0])
        self.orientation = s['orientation']
        self.position    = s['position']
        self.order       = s['order']
        self.axis        = s['axis']

    def gotUnit (self, s, loc, toks):
        self.units = listToDict (toks[0])

    def gotBone (self, s, loc, toks):
        dict = listToDict (toks[0])
        bone = Bone (dict)
        self.bones[bone.name] = bone

    def gotName (self, s, loc, toks):
        self.name = toks[0]

    def gotHierarchy (self, s, loc, toks):
        lines = [s.strip (' \r') for s in toks[1].split ('\n')]
        [self.hierarchy.append (s.split (' ')) for s in lines]

    def getGrammar (self):
        if self.grammar is None:
            float = Combine (Word ('+-'+nums, nums) +
                             Optional (Literal ('.') + Optional (Word (nums))) +
                             Optional (CaselessLiteral ('E') + Word ('+-' + nums, nums)))
            name = Word (printables)
            comment = Literal ('#') + Optional (restOfLine)
            axisOrder = oneOf ("XYZ XZY YXZ YZX ZXY ZYX")
            ro = oneOf ("TX TY TZ RX RY RZ")
            rootElement = Group (
                "order" + ro + ro + ro + ro + ro + ro \
              | "axis" + axisOrder \
              | "position" + float + float + float \
              | "orientation" + float + float + float
              )
            root = Group (OneOrMore (rootElement))
            unitElement = Group ("mass" + float | "length" + float | "angle" + oneOf ("deg rad"))
            unit = Group (OneOrMore (unitElement))
            dof = oneOf ("rx ry rz")
            triplet = Literal ('(').suppress () + float + float + Literal (')').suppress ()
            boneElement = Group (
                "id" + Word (nums) \
              | "name" + Word (alphanums) \
              | "direction" + float + float + float \
              | "length" + float \
              | "axis" + float + float + float + axisOrder \
              | "dof" + OneOrMore (dof) \
              | "limits" + OneOrMore (triplet)
              )
            bone = Suppress ("begin") + Group (OneOrMore (boneElement)) + Suppress ("end")
            bonedata = ZeroOrMore (bone)
            docLine = Combine (OneOrMore (Word (alphanums + '!"#$%&\'()*+,-./;<=>?@[\\]^_`{|}~')), joinString=' ', adjacent=False)
            sectstart = LineStart () + Literal (':').suppress ()
            hierarchy = "begin" + SkipTo ("end", include=True)
            section = Group (
                sectstart + "version" + float \
              | sectstart + "name" + name \
              | sectstart + "units" + unit \
              | sectstart + "documentation" + docLine \
              | sectstart + "root" + root \
              | sectstart + "bonedata" + bonedata \
              | sectstart + "hierarchy" + hierarchy
              )
            part = section | Suppress (comment)
            self.grammar = OneOrMore (part)

            root.setParseAction (self.gotRoot)
            unit.setParseAction (self.gotUnit)
            bone.setParseAction (self.gotBone)
            name.setParseAction (self.gotName)
            hierarchy.setParseAction (self.gotHierarchy)

        return self.grammar

    def parse (self, filename):
        g = self.getGrammar ()
        g.parseFile (filename)

# vim: ts=4:sw=4:et:tw=80
