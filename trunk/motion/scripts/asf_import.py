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
Name: 'ASF Skeleton File (.asf)...'
Blender: 236
Group: 'Import'
Tip: 'Imports an Acclaim ASF file and builds a set of armatures'
"""

import Blender
import ASFReader
import math
import string

# make it faster?
try:
    import psyco
except:
    pass

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

    # import each bone. for now, just position it at the origin
    for name, data in reader.bones.iteritems():
        bone = Blender.Armature.Bone.New(name)

        bone.setHead(0.0, 0.0, 0.0)
        l = data.length * 0.1
        d = data.direction
        # swizzle Y and Z
        bone.setTail(float(d[0]) * l, float(d[2]) * l, float(d[1]) * l)

        bones[name] = bone

        # Set properties for axis & dof as given in the ASF. We'll use these
        # values later on in parsing the AMC
        armObj.addProperty('%s-axis' % name, ','.join(data.axis[:3]), 'STRING')
        if data.dof is None:
            armObj.addProperty('%s-dof' % name, '', 'STRING')
        else:
            armObj.addProperty('%s-dof' % name, ','.join(data.dof), 'STRING')

    # add root
    bone = Blender.Armature.Bone.New('root')
    bone.setHead(0.0, 0.0, 0.0);
    bone.setTail(0.0, 0.0, 0.0);
    armData.addBone(bone)
    bones['root'] = bone

    # run through the hierarchy: for each, parent and add to the armature
    for set in reader.hierarchy:
        parent = set[0]
        children = set[1:]
        for bone in children:
            bones[bone].setTail(addVectors(bones[bone].tail, bones[parent].tail))
            bones[bone].setHead(addVectors(bones[bone].head, bones[parent].tail))
    for set in reader.hierarchy:
        parent = set[0]
        children = set[1:]
        for bone in children:
            bones[bone].setParent(bones[parent])
            armData.addBone(bones[bone])

            # now that we're linked, set the bonespace axis transformation
            # as a property on the object
            ba = bones[bone].getRestMatrix ('bonespace').rotationPart ().toEuler ()
            ba = map(str, [ba.x, ba.y, ba.z])
            armObj.addProperty('%s-baxis' % bone, ','.join(ba), 'STRING')

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
