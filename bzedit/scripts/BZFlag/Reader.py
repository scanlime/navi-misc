""" BZFlag.Reader

Parser for BZFlag world files.
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

from pyparsing import *

class Reader:
    grammar = None

    def getGrammar (self):
        if self.grammar is None:
            comment = Literal('#') + Optional (restOfLine)
            float = Combine(Word('+-'+nums, nums) +
                            Optional(Literal('.') + Optional(Word(nums))) +
                            Optional(CaselessLiteral('E') + Word('+-'+nums, nums)))
            TwoDPoint = float + float
            ThreeDPoint = float + float + float
            globalReference = Word(alphanums + '/:*_?')
            flagShortName = Word(alphas, min=1, max=2)

            end = Suppress(CaselessLiteral('end'))
            name = CaselessLiteral('name')
            pos = CaselessLiteral('position') | CaselessLiteral('pos')
            rot = CaselessLiteral('rotation') | CaselessLiteral('rot')
            scale = CaselessLiteral('scale')
            shear = CaselessLiteral('shear')
            shift = CaselessLiteral('shift')
            size = CaselessLiteral('size')
            spin = CaselessLiteral('spin')

            objectProperty = Group(name + Word(alphanums))
            locationProperty =      \
                Group(pos + ThreeDPoint)   \
              | Group(size + ThreeDPoint)  \
              | Group(rot + float)         \
              | Group(shift + ThreeDPoint) \
              | Group(scale + ThreeDPoint) \
              | Group(shear + ThreeDPoint) \
              | Group(spin + ThreeDPoint)  \
              | objectProperty
            obstacleProperty =                  \
                CaselessLiteral('drivethrough') \
              | CaselessLiteral('shootthrough') \
              | CaselessLiteral('passable')     \
              | locationProperty

            box = Group(CaselessLiteral('box') + OneOrMore(obstacleProperty) + end)

            pyramidProperty =            \
                CaselessLiteral('flipz') \
              | obstacleProperty
            pyramid = Group(CaselessLiteral('pyramid') + OneOrMore(pyramidProperty) + end)

            baseProperty =                            \
                CaselessLiteral('color') + Word(nums) \
              | obstacleProperty
            base = Group(CaselessLiteral('base') + OneOrMore(baseProperty) + end)

            worldProperty =                           \
                size + float                          \
              | CaselessLiteral('flagHeight') + float \
              | objectProperty
            world = Group(CaselessLiteral('world') + OneOrMore(worldProperty) + end)

            teleporterProperty =                  \
                CaselessLiteral('border') + float \
              | obstacleProperty
            teleporter = Group(CaselessLiteral('teleporter') + OneOrMore(teleporterProperty) + end)

            teleporterSide =         \
                CaselessLiteral('f') \
              | CaselessLiteral('b') \
              | Literal('?')         \
              | Literal('*')
            teleporterSpec = \
                Word(nums)   \
              | Combine(globalReference + Optional(Literal(':') + teleporterSide))
            linkProperty =                                      \
                Group(CaselessLiteral('to') + teleporterSpec)   \
              | Group(CaselessLiteral('from') + teleporterSpec) \
              | objectProperty
            link = Group(CaselessLiteral('link') + OneOrMore(linkProperty) + end)

            # FIXME - add material to this object
            arcProperty =                                                  \
                CaselessLiteral('divisions') + Word(nums)                  \
              | CaselessLiteral('angle') + float                           \
              | CaselessLiteral('ratio') + float                           \
              | CaselessLiteral('texsize') + float + float + float + float \
              | CaselessLiteral('phydrv') + Word(alphanums)                \
              | CaselessLiteral('smoothbounce')                            \
              | CaselessLiteral('flatshading')                             \
              | obstacleProperty
            arc = Group(CaselessLiteral('arc') + OneOrMore(arcProperty) + end)

            # FIXME - add material to this object
            tetraProperty =                              \
                CaselessLiteral('vertex') + ThreeDPoint  \
              | CaselessLiteral('normals') + ThreeDPoint \
              | CaselessLiteral('texcoords') + TwoDPoint \
              | obstacleProperty
            tetra = Group(CaselessLiteral('tetra') + OneOrMore(tetraProperty) + end)

            flagSpec =                  \
                CaselessLiteral('good') \
              | CaselessLiteral('bad')  \
              | flagShortName
            zoneProperty =                                        \
                CaselessLiteral('flag') + OneOrMore(flagSpec)     \
              | CaselessLiteral('team') + OneOrMore(Word(nums))   \
              | CaselessLiteral('safety') + OneOrMore(Word(nums)) \
              | locationProperty
            zone = Group(CaselessLiteral('zone') + OneOrMore(zoneProperty) + end)

            weaponProperty = \
                Group(CaselessLiteral('initdelay') + float)        \
              | Group(CaselessLiteral('delay') + OneOrMore(float)) \
              | Group(CaselessLiteral('type') + flagShortName)     \
              | locationProperty
            weapon = Group(CaselessLiteral('weapon') + OneOrMore(weaponProperty) + end)

            worldObject =  \
                box        \
              | pyramid    \
              | base       \
              | teleporter \
              | link       \
              | arc        \
              | tetra      \
              | zone

            self.grammar = OneOrMore(worldObject)
            self.grammar.ignore(comment)
        return self.grammar

    def parse(self, filename):
        g = self.getGrammar()
        a = g.parseFile(filename)
        return a
