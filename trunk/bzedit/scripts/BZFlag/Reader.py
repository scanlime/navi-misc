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
from Types import typeMap

class Reader:
    grammar = None
    objects = []

    def gotElement(self, s, loc, toks):
        object = toks[0]
        objtype = typeMap[object[0]]
        if objtype is not None:
            self.objects.append(objtype(object))
        else:
            print 'unable to create object "%s"' % object[0]

    def getGrammar(self):
        if self.grammar is None:
            comment = Literal('#') + Optional (restOfLine)
            float = Combine(Word('+-'+nums, nums) +
                            Optional(Literal('.') + Optional(Word(nums))) +
                            Optional(CaselessLiteral('E') + Word('+-'+nums, nums))) | Combine(Literal('.') + Word(nums) +
                            Optional(CaselessLiteral('E') + Word('+-'+nums, nums)))
            int = Word('+-'+nums, nums)
            TwoDPoint = float + float
            ThreeDPoint = float + float + float
            globalReference = Word(alphanums + '/:*_?')
            localReference = Word(alphanums + '/_')
            flagShortName = Word(alphas, min=1, max=2)

            rgbColor = (float + float + float) | Word(alphanums)
            rgbaColor = rgbColor + Optional(float)

            phydrv = Group(CaselessLiteral('phydrv') + localReference)
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

            objectProperty = Group(name + restOfLine)
            locationProperty = (
                Group(pos + ThreeDPoint)
              | Group(size + ThreeDPoint)
              | Group(rot + float)
              | Group(shift + ThreeDPoint)
              | Group(scale + ThreeDPoint)
              | Group(shear + ThreeDPoint)
              | Group(spin + float + ThreeDPoint)
              | Group(xform + globalReference)
              | objectProperty
              )
            obstacleProperty = (
                Group(drivethrough)
              | Group(shootthrough)
              | Group(passable)
              | locationProperty
              )

            channel = (
                CaselessLiteral('red')
              | CaselessLiteral('green')
              | CaselessLiteral('blue')
              | CaselessLiteral('alpha')
              )
            dynamicColorState = Literal('0') | Literal('1') | Literal('2')
            dynamicColorProperty = (
                Group(channel + CaselessLiteral('limits') + float + float)
              | Group(channel + CaselessLiteral('sinusoid') + float + float + float)
              | Group(channel + CaselessLiteral('clampup') + float + float + float)
              | Group(channel + CaselessLiteral('clampdown') + float + float + float)
              | Group(channel + CaselessLiteral('sequence') + float + float + OneOrMore(dynamicColorState))
              | objectProperty
              )
            dynamicColor = Group(CaselessLiteral('dynamicColor') + OneOrMore(dynamicColorProperty) + end)

            box = Group(CaselessLiteral('box') + OneOrMore(obstacleProperty) + end)

            pyramidProperty = (
                CaselessLiteral('flipz')
              | obstacleProperty
              )
            pyramid = Group(CaselessLiteral('pyramid') + OneOrMore(pyramidProperty) + end)

            baseProperty = (
                Group(CaselessLiteral('color') + int)
              | obstacleProperty
              )
            base = Group(CaselessLiteral('base') + OneOrMore(baseProperty) + end)

            worldProperty = (
                Group(size + float)
              | Group(CaselessLiteral('flagHeight') + float)
              | objectProperty
              )
            world = Group(CaselessLiteral('world') + OneOrMore(worldProperty) + end)

            teleporterProperty = (
                Group(CaselessLiteral('border') + float)
              | obstacleProperty
              )
            teleporter = (
                Group(CaselessLiteral('teleporter') + OneOrMore(teleporterProperty) + end)
              | Group(CaselessLiteral('teleporter') + localReference + OneOrMore(teleporterProperty) + end)
              )

            teleporterSide = (
                CaselessLiteral('f')
              | CaselessLiteral('b')
              | Literal('?')
              | Literal('*')
              )
            teleporterSpec = (
                int
              | Combine(globalReference + Optional(Literal(':') + teleporterSide))
              )
            linkProperty = (
                Group(CaselessLiteral('to') + teleporterSpec)
              | Group(CaselessLiteral('from') + teleporterSpec)
              | objectProperty
              )
            link = Group(CaselessLiteral('link') + OneOrMore(linkProperty) + end)

            filename = Word(printables)
            ambient = CaselessLiteral('ambient')
            diffuse = CaselessLiteral('diffuse') | CaselessLiteral('color')
            specular = CaselessLiteral('specular')
            emission = CaselessLiteral('emission')
            shininess = CaselessLiteral('shininess')
            materialProperty = (
                Group(CaselessLiteral('texture') + filename)
              | Group(CaselessLiteral('addTexture') + filename)
              | Group(CaselessLiteral('notextures'))
              | Group(CaselessLiteral('notexcolor'))
              | Group(CaselessLiteral('notexalpha'))
              | Group(CaselessLiteral('noculling'))
              | Group(CaselessLiteral('nosorting'))
              | Group(CaselessLiteral('alphathresh') + float)
              | Group(CaselessLiteral('texmat') + localReference)
              | Group(CaselessLiteral('dyncol') + localReference)
              | Group(ambient + rgbaColor)
              | Group(diffuse + rgbaColor)
              | Group(specular + rgbaColor)
              | Group(emission + rgbaColor)
              | Group(shininess + float)
              | Group(CaselessLiteral('matref') + localReference)
              | Group(CaselessLiteral('resetmat'))
              | objectProperty
              )
            material = Group(CaselessLiteral('material') + ZeroOrMore(materialProperty) + end)

            curveProperty = (
                Group(CaselessLiteral('divisions') + int)
              | phydrv
              | smoothbounce
              | flatshading
              )

            meshboxSide = (
                CaselessLiteral('top')
              | CaselessLiteral('bottom')
              | CaselessLiteral('inside')
              | CaselessLiteral('outside')
              | CaselessLiteral('startside')
              | CaselessLiteral('endside')
              )
            meshboxProperty = (
                Group(CaselessLiteral('angle') + float)
              | Group(CaselessLiteral('ratio') + float)
              | Group(CaselessLiteral('texsize') + float + float + float + float)
              | materialProperty
              | Group(meshboxSide + materialProperty)
              | curveProperty
              | obstacleProperty
              )
            meshbox = Group(CaselessLiteral('meshbox') + OneOrMore(meshboxProperty) + end)

            meshpyrSide = (
                CaselessLiteral('edge')
              | CaselessLiteral('bottom')
              | CaselessLiteral('startside')
              | CaselessLiteral('endside')
              )
            meshpyrProperty = (
                Group(CaselessLiteral('angle') + float)
              | Group(CaselessLiteral('ratio') + float)
              | Group(CaselessLiteral('texsize') + float + float)
              | CaselessLiteral('flipz')
              | materialProperty
              | Group(meshpyrSide + materialProperty)
              | curveProperty
              | obstacleProperty
              )
            meshpyr = Group(CaselessLiteral('meshpyr') + OneOrMore(meshpyrProperty) + end)

            arcProperty = (
                Group(CaselessLiteral('ratio') + float)
              | Group(CaselessLiteral('angle') + float)
              | Group(CaselessLiteral('texsize') + float + float + float + float)
              | materialProperty
              | Group(meshboxSide + materialProperty)
              | curveProperty
              | obstacleProperty
              )
            arc = Group(CaselessLiteral('arc') + OneOrMore(arcProperty) + end)

            hemisphere = CaselessLiteral('hemisphere') | CaselessLiteral('hemi')
            sphereSide = (
                CaselessLiteral('edge')
              | CaselessLiteral('bottom')
              )
            sphereProperty = (
                Group(CaselessLiteral('radius') + float)
              | hemisphere
              | Group(CaselessLiteral('texsize') + float + float)
              | materialProperty
              | Group(sphereSide + materialProperty)
              | curveProperty
              | obstacleProperty
              )
            sphere = Group(CaselessLiteral('sphere') + OneOrMore(sphereProperty) + end)

            coneProperty = (
                Group(CaselessLiteral('angle') + float)
              | Group(CaselessLiteral('texsize') + float + float)
              | materialProperty
              | Group(meshpyrSide + materialProperty)
              | curveProperty
              | obstacleProperty
              )
            cone = Group(CaselessLiteral('cone') + OneOrMore(coneProperty) + end)

            tetraProperty = (
                CaselessLiteral('vertex') + ThreeDPoint
              | CaselessLiteral('normals') + ThreeDPoint
              | CaselessLiteral('texcoords') + TwoDPoint
              | materialProperty
              | obstacleProperty
              )
            tetra = Group(CaselessLiteral('tetra') + OneOrMore(tetraProperty) + end)

            flagSpec = (
                CaselessLiteral('good')
              | CaselessLiteral('bad')
              | flagShortName
              )
            zoneProperty = (
                Group(CaselessLiteral('flag') + OneOrMore(flagSpec))
              | Group(CaselessLiteral('team') + OneOrMore(int))
              | Group(CaselessLiteral('safety') + OneOrMore(int))
              | locationProperty
              )
            zone = Group(CaselessLiteral('zone') + OneOrMore(zoneProperty) + end)

            weaponProperty = (
                Group(CaselessLiteral('initdelay') + float)
              | Group(CaselessLiteral('delay') + OneOrMore(float))
              | Group(CaselessLiteral('type') + flagShortName)
              | locationProperty
              )
            weapon = Group(CaselessLiteral('weapon') + OneOrMore(weaponProperty) + end)

            waterLevelProperty = (
                Group(CaselessLiteral('height') + float)
              | materialProperty
              | objectProperty
              )
            waterLevel = Group(CaselessLiteral('waterLevel') + OneOrMore(waterLevelProperty) + end)

            physicsProperty = (
                Group(CaselessLiteral('linear') + ThreeDPoint)
              | Group(CaselessLiteral('angular') + ThreeDPoint)
              | Group(CaselessLiteral('slide') + float)
              | Group(CaselessLiteral('death') + restOfLine)
              | objectProperty
              )
            physics = Group(CaselessLiteral('physics') + OneOrMore(physicsProperty) + end)

            textureMatrixProperty = (
                Group(CaselessLiteral('fixedshift') + TwoDPoint)
              | Group(CaselessLiteral('fixedscale') + TwoDPoint)
              | Group(CaselessLiteral('fixedspin') + float)
              | Group(CaselessLiteral('fixedcenter') + TwoDPoint)
              | Group(shift + float + float)
              | Group(spin + float)
              | Group(scale + float + float + float + float)
              | Group(CaselessLiteral('center') + TwoDPoint)
              | objectProperty
              )
            textureMatrix = Group(CaselessLiteral('textureMatrix') + OneOrMore(textureMatrixProperty) + end)

            transformProperty = (
                Group(shift + ThreeDPoint)
              | Group(scale + ThreeDPoint)
              | Group(shear + ThreeDPoint)
              | Group(spin + float + ThreeDPoint)
              | Group(xform + globalReference)
              | objectProperty
              )
            transform = Group(CaselessLiteral('transform') + OneOrMore(transformProperty) + end)

            faceProperty = (
                Group(CaselessLiteral('vertices') + int + int + OneOrMore(int))
              | Group(CaselessLiteral('normals') + int + int + OneOrMore(int))
              | Group(CaselessLiteral('texcoords') + int + int + OneOrMore(int))
              | materialProperty
              | phydrv
              | Group(smoothbounce)
              | Group(noclusters)
              | Group(drivethrough)
              | Group(shootthrough)
              | Group(passable)
              )
            face = Group(CaselessLiteral('face') + OneOrMore(faceProperty) + CaselessLiteral('endface').suppress())

            meshProperty = (
                face
              | Group(CaselessLiteral('inside') + ThreeDPoint)
              | Group(CaselessLiteral('outside') + ThreeDPoint)
              | Group(CaselessLiteral('vertex') + ThreeDPoint)
              | Group(CaselessLiteral('normal') + ThreeDPoint)
              | Group(CaselessLiteral('texcoord') + TwoDPoint)
              | materialProperty
              | phydrv
              | Group(smoothbounce)
              | Group(noclusters)
              | obstacleProperty
              )
            mesh = Group(CaselessLiteral('mesh') + OneOrMore(meshProperty) + end)

            options = Group(CaselessLiteral('options') + SkipTo(LineStart() + end, include=True))

            groupProperty = (
                Group(CaselessLiteral('team') + int)
              | Group(CaselessLiteral('tint') + rgbaColor)
              | Group(CaselessLiteral('matref') + localReference)
              | phydrv
              | obstacleProperty
              )
            group = Group(CaselessLiteral('group') + Word(alphanums) + ZeroOrMore(groupProperty) + end)

            groupMember = (
                arc
              | box
              | base
              | cone
              | group
              | mesh
              | meshbox
              | meshpyr
              | pyramid
              | sphere
              | teleporter
              | tetra
              )
            define = Group(CaselessLiteral('define') + Word(alphanums) + OneOrMore(groupMember) + CaselessLiteral('enddef'))

            worldObject = (
                arc
              | base
              | box
              | cone
              | define
              | dynamicColor
              | group
              | link
              | material
              | mesh
              | meshbox
              | meshpyr
              | options
              | physics
              | pyramid
              | sphere
              | teleporter
              | tetra
              | textureMatrix
              | transform
              | waterLevel
              | weapon
              | world
              | zone
              )

            worldObject.setParseAction(self.gotElement)

            self.grammar = OneOrMore(worldObject)
            self.grammar.ignore(comment)
        return self.grammar

    def parse(self, filename):
        self.objects = []
        g = self.getGrammar()
        a = g.parseFile(filename)
        return a
