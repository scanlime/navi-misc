#!BPY
#
# asf_import.py - Import an Acclaim Skeleton File and create a set
#                 of armatures matching the data
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

""" Registration info for Blender menus
Name: 'ASF Skeleton File...'
Blender: 236
Group: 'Import'
Tip: 'Imports an Acclaim ASF file and builds a set of armatures'
"""

import Blender
import math

try:
    import ASFReader
except ImportError:
    Blender.Draw.PupMenu('Error importing "ASFReader" module%t|'
                         'You may have to start the blender game engine|'
                         'momentarily then exit it to set the Python|'
                         'path properly.')
    ASFReader = None

def addVectors(a, b):
    x = []
    for i in range(len(a)):
        x.append(a[i] + b[i])
    return x

def importObjects(reader):
    scene = Blender.Scene.getCurrent()
    armObj = Blender.Object.New('Armature', reader.name)
    armData = Blender.Armature.New()

    bones = {}

    # import each bone. the head is always at the origin
    for k in reader.bones.keys():
        b = reader.bones[k]
        bone = Blender.Armature.Bone.New(k)

        bone.setHead(0.0, 0.0, 0.0)

        l = b.length
        d = b.direction
        bone.setTail(float(d[0]) * l, float(d[1]) * l, float(d[2]) * l)

        armData.addBone(bone)

        bones[k] = bone

    # add root
    bone = Blender.Armature.Bone.New('root')
    bone.setHead(0.0, 0.0, 0.0);
    bone.setTail(0.0, 0.0, 0.0);
    armData.addBone(bone)
    bones['root'] = bone

    for set in reader.hierarchy:
        parent = set[0]
        children = set[1:]
        for bone in children:
            pass
            bones[bone].setHead(addVectors(bones[bone].head, bones[parent].tail))
            bones[bone].setTail(addVectors(bones[bone].tail, bones[parent].tail))
            #bones[bone].setParent(bones[parent])
            #bones[bone].setLoc(bones[parent].loc + bones[parent].tail)

    armData.drawAxes(1)
    armObj.link(armData)
    scene.link(armObj)
    armObj.makeDisplayList()
    Blender.Window.RedrawAll()

def fileSelectedCallback(filename):
    reader = ASFReader.ASFReader()
    try:
        reader.parse(filename)
    except IOError, s:
        ASFReader.log.err(s)
    else:
        importObjects(reader)

    if ASFReader.log.numErrors:
        ASFReader.log.report('Errors in loading ASF file')

if ASFReader:
    Blender.Window.FileSelector(fileSelectedCallback, 'Load ASF Skeleton File')
