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
            int = Word('+-'+nums, nums)
            TwoDPoint = float + float
            ThreeDPoint = float + float + float
            globalReference = Word(alphanums + '/:*_?')
            flagShortName = Word(alphas, min=1, max=2)

            phydrv = Group(CaselessLiteral('phydrv') + Word(alphanums))
            smoothbounce = CaselessLiteral('smoothbounce')
            flatshading = CaselessLiteral('flatshading')
            shootthrough = CaselessLiteral('shootthrough')
            drivethrough = CaselessLiteral('drivethrough')
            passable = CaselessLiteral('passable')
            noclusters = CaselessLiteral('noclusters')

            end = Suppress(CaselessLiteral('end'))
            name = CaselessLiteral('name')
            pos = CaselessLiteral('position') | CaselessLiteral('pos')
            rot = CaselessLiteral('rotation') | CaselessLiteral('rot')
            scale = CaselessLiteral('scale')
            shear = CaselessLiteral('shear')
            shift = CaselessLiteral('shift')
            size = CaselessLiteral('size')
            spin = CaselessLiteral('spin')
            xform = CaselessLiteral('xform')

            objectProperty = Group(name + Word(alphanums))
            locationProperty =                 \
                Group(pos + ThreeDPoint)       \
              | Group(size + ThreeDPoint)      \
              | Group(rot + float)             \
              | Group(shift + ThreeDPoint)     \
              | Group(scale + ThreeDPoint)     \
              | Group(shear + ThreeDPoint)     \
              | Group(spin + ThreeDPoint)      \
              | Group(xform + globalReference) \
              | objectProperty
            obstacleProperty =                  \
                drivethrough                    \
              | shootthrough                    \
              | passable                        \
              | locationProperty

            channel =                    \
                CaselessLiteral('red')   \
              | CaselessLiteral('green') \
              | CaselessLiteral('blue')  \
              | CaselessLiteral('alpha')
            dynamicColorState = Literal('0') | Literal('1') | Literal('2')
            dynamicColorProperty = \
                Group(channel + CaselessLiteral('limits') + float + float) \
              | Group(channel + CaselessLiteral('sinusoid') + float + float + float) \
              | Group(channel + CaselessLiteral('clampup') + float + float + float) \
              | Group(channel + CaselessLiteral('clampdown') + float + float + float) \
              | Group(channel + CaselessLiteral('sequence') + float + float + OneOrMore(dynamicColorState)) \
              | objectProperty
            dynamicColor = Group(CaselessLiteral('dynamicColor') + OneOrMore(dynamicColorProperty) + end)

            box = Group(CaselessLiteral('box') + OneOrMore(obstacleProperty) + end)

            pyramidProperty =            \
                CaselessLiteral('flipz') \
              | obstacleProperty
            pyramid = Group(CaselessLiteral('pyramid') + OneOrMore(pyramidProperty) + end)

            baseProperty =                     \
                CaselessLiteral('color') + int \
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
                int          \
              | Combine(globalReference + Optional(Literal(':') + teleporterSide))
            linkProperty =                                      \
                Group(CaselessLiteral('to') + teleporterSpec)   \
              | Group(CaselessLiteral('from') + teleporterSpec) \
              | objectProperty
            link = Group(CaselessLiteral('link') + OneOrMore(linkProperty) + end)

            # FIXME - add material to this object
            curveProperty =                               \
                Group(CaselessLiteral('divisions') + int) \
              | phydrv                                    \
              | smoothbounce                              \
              | flatshading

            meshboxProperty =                                                     \
                Group(CaselessLiteral('angle') + float)                           \
              | Group(CaselessLiteral('ratio') + float)                           \
              | Group(CaselessLiteral('texsize') + float + float + float + float) \
              | curveProperty                                                     \
              | obstacleProperty
            meshbox = Group(CaselessLiteral('meshbox') + OneOrMore(meshboxProperty) + end)

            meshpyrProperty =                                     \
                Group(CaselessLiteral('angle') + float)           \
              | Group(CaselessLiteral('ratio') + float)           \
              | Group(CaselessLiteral('texsize') + float + float) \
              | CaselessLiteral('flipz')                          \
              | curveProperty                                     \
              | obstacleProperty
            meshpyr = Group(CaselessLiteral('meshpyr') + OneOrMore(meshpyrProperty) + end)

            arcProperty =                                                         \
                Group(CaselessLiteral('ratio') + float)                           \
              | Group(CaselessLiteral('angle') + float)                           \
              | Group(CaselessLiteral('texsize') + float + float + float + float) \
              | curveProperty                                                     \
              | obstacleProperty
            arc = Group(CaselessLiteral('arc') + OneOrMore(arcProperty) + end)

            hemisphere = CaselessLiteral('hemisphere') | CaselessLiteral('hemi')
            sphereProperty =                                      \
                Group(CaselessLiteral('radius') + float)          \
              | hemisphere                                        \
              | Group(CaselessLiteral('texsize') + float + float) \
              | curveProperty                                     \
              | obstacleProperty
            sphere = Group(CaselessLiteral('sphere') + OneOrMore(sphereProperty) + end)

            coneProperty =                                        \
                Group(CaselessLiteral('angle') + float)           \
              | Group(CaselessLiteral('texsize') + float + float) \
              | curveProperty                                     \
              | obstacleProperty
            cone = Group(CaselessLiteral('cone') + OneOrMore(coneProperty) + end)

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
            zoneProperty =                                    \
                CaselessLiteral('flag') + OneOrMore(flagSpec) \
              | CaselessLiteral('team') + OneOrMore(int)      \
              | CaselessLiteral('safety') + OneOrMore(int)    \
              | locationProperty
            zone = Group(CaselessLiteral('zone') + OneOrMore(zoneProperty) + end)

            weaponProperty = \
                Group(CaselessLiteral('initdelay') + float)        \
              | Group(CaselessLiteral('delay') + OneOrMore(float)) \
              | Group(CaselessLiteral('type') + flagShortName)     \
              | locationProperty
            weapon = Group(CaselessLiteral('weapon') + OneOrMore(weaponProperty) + end)

            # FIXME - add material to this object
            waterLevelProperty =                         \
                Group(CaselessLiteral('height') + float) \
              | objectProperty
            waterLevel = Group(CaselessLiteral('waterLevel') + OneOrMore(waterLevelProperty) + end)

            physicsProperty =                                   \
                Group(CaselessLiteral('linear') + ThreeDPoint)  \
              | Group(CaselessLiteral('angular') + ThreeDPoint) \
              | Group(CaselessLiteral('slide') + float)         \
              | Group(CaselessLiteral('death') + restOfLine)    \
              | objectProperty
            physics = Group(CaselessLiteral('physics') + OneOrMore(physicsProperty) + end)

            textureMatrixProperty = \
                Group(CaselessLiteral('fixedshift') + TwoDPoint) \
              | Group(CaselessLiteral('fixedscale') + TwoDPoint) \
              | Group(CaselessLiteral('fixedspin') + float) \
              | Group(CaselessLiteral('fixedcenter') + TwoDPoint) \
              | Group(shift + float + float) \
              | Group(spin + float) \
              | Group(scale + float + float + float + float) \
              | Group(CaselessLiteral('center') + TwoDPoint) \
              | objectProperty
            textureMatrix = Group(CaselessLiteral('textureMatrix') + OneOrMore(textureMatrixProperty) + end)

            transformProperty = \
                Group(shift + ThreeDPoint) \
              | Group(scale + ThreeDPoint) \
              | Group(shear + ThreeDPoint) \
              | Group(spin + float + ThreeDPoint) \
              | Group(xform + globalReference) \
              | objectProperty
            transform = Group(CaselessLiteral('transform') + OneOrMore(transformProperty) + end)

            # FIXME - add material to this object
            faceProperty =                                                       \
                Group(CaselessLiteral('vertices') + int + int + OneOrMore(int))  \
              | Group(CaselessLiteral('normals') + int + int + OneOrMore(int))   \
              | Group(CaselessLiteral('texcoords') + int + int + OneOrMore(int)) \
              | phydrv                                                           \
              | smoothbounce                                                     \
              | noclusters                                                       \
              | drivethrough                                                     \
              | shootthrough                                                     \
              | passable
            face = Group(CaselessLiteral('face') + OneOrMore(faceProperty) + CaselessLiteral('endface').suppress())

            # FIXME - add material to this object
            meshProperty =                                      \
                face                                            \
              | Group(CaselessLiteral('inside') + ThreeDPoint)  \
              | Group(CaselessLiteral('outside') + ThreeDPoint) \
              | Group(CaselessLiteral('vertex') + ThreeDPoint)  \
              | Group(CaselessLiteral('normal') + ThreeDPoint)  \
              | Group(CaselessLiteral('texcoord') + TwoDPoint)  \
              | phydrv                                          \
              | smoothbounce                                    \
              | noclusters                                      \
              | obstacleProperty
            mesh = Group(CaselessLiteral('mesh') + OneOrMore(meshProperty) + end)

            options = Group(CaselessLiteral('options') + SkipTo(end, include=True))

            worldObject =     \
                arc           \
              | box           \
              | base          \
              | cone          \
              | dynamicColor  \
              | link          \
              | mesh          \
              | meshbox       \
              | meshpyr       \
              | options       \
              | physics       \
              | pyramid       \
              | sphere        \
              | teleporter    \
              | tetra         \
              | textureMatrix \
              | transform     \
              | waterLevel    \
              | weapon        \
              | world         \
              | zone

            self.grammar = OneOrMore(worldObject)
            self.grammar.ignore(comment)
        return self.grammar

    def parse(self, filename):
        g = self.getGrammar()
        a = g.parseFile(filename)
        return a
