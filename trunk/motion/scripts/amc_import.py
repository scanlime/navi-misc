# amc_import.py - Import an Acclaim Motion Capture file and apply NLA
#                 transformations to a matching armature
#
# Copyright (C) 2005-2006 David Trowbridge
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
Name: 'AMC Motion Capture (.amc)...'
Blender: 236
Group: 'Import'
Tip: 'Imports an Acclaim AMC file and applies motion capture data to an armature'
"""

import Blender
from Blender.Armature.Bone import ROT, LOC, SIZE
import AMCReader

# if we've got psyco installed, use it
try:
    import psyco
except:
    pass

dofTable = {}
axisTable = {}
baxisTable = {}
boneTable = {}

def getCorrection (object, bone):
    try:
        axis = axisTable[bone]
        baxis = baxisTable[bone]
    except KeyError:
        axis = map(float, object.getProperty ('%s-axis' % bone).getData ().split (','))
        axisTable[bone] = axis
        baxis = map(float, object.getProperty ('%s-baxis' % bone).getData ().split (','))
        baxisTable[bone] = baxis

    for i in range (len (baxis)):
        baxis[i] = -baxis[i]

    return baxis

def getRot(object, bone, rot):
    try:
        dof = dofTable[bone]
    except KeyError:
        dof = object.getProperty('%s-dof' % bone).getData().split(',')
        if dof is None:
            dof = []
        dofTable[bone] = dof
    r = [0.0, 0.0, 0.0]
    i = 0
    for d in dof:
        if d == 'rx':
            r[0] = rot[i]
        elif d == 'ry':
            r[1] = rot[i]
        else:
            r[2] = rot[i]
        i += 1

    euler = Blender.Mathutils.Euler (r)
    #euler.x = euler.x - axis[0] + baxis[0]
    #euler.y = euler.y - axis[1] + baxis[1]
    #euler.z = euler.z - axis[2] + baxis[2]

    baxis = getCorrection (object, bone)
    pquat = boneTable[bone].getParent ().getQuat ()
    euler = Blender.Mathutils.Euler (baxis)

    quat = euler.toQuat()
    return (quat - pquat)

def importData (reader, object, filename):
    action = Blender.Armature.NLA.NewAction(filename.split('/')[-1])
    action.setActive(object)
    armature = object.getData()
    scene = Blender.Scene.getCurrent()
    context = scene.getRenderingContext()

    context.framesPerSec(120)

    bones = armature.getBones()
    for bone in bones:
        boneTable[bone.getName()] = bone
        bone.setPose([ROT, LOC, SIZE])

    for frame in reader.frames:
        context.currentFrame(frame.number)

        # set root position/rotation
        loc = frame.bones['root'][0:3]

        # swizzle, scale & invert
        (loc[0], loc[2], loc[1]) = (loc[0] * 0.1, loc[1] * 0.1, loc[2] * -0.1)
        rot = frame.bones['root'][3:6]
        euler = Blender.Mathutils.Euler (rot)
        quat = euler.toQuat()
        #b['root'].setLoc(loc)
        #b['root'].setQuat(quat)
        #b['root'].setPose([ROT, LOC])

        euler = Blender.Mathutils.Euler (getCorrection (object, 'root'))
        quat = euler.toQuat ();
        boneTable['root'].setQuat (quat)
        boneTable['root'].setPose ([ROT])

        #print frame.number

        for bname, bone in frame.bones.iteritems():
            if bname == 'root':
                continue
            bone = frame.bones[bname]
            quat = getRot (object, bname, bone)
            boneTable[bname].setQuat (quat)
            boneTable[bname].setPose ([ROT])

    Blender.Window.RedrawAll()

def fileSelectedCallback(filename):
    reader = AMCReader.AMCReader()
    try:
        reader.parse(filename)
    except IOError, s:
        AMCReader.log.err(s)
    else:
        objects = Blender.Object.GetSelected()
        if len(objects) != 1:
            AMCReader.log.error('Please select the armature to apply MC data to')
        else:
            importData(reader, objects[0], filename)

    if AMCReader.log.numErrors:
        AMCReader.log.report('Errors in loading AMC file')

if AMCReader:
    Blender.Window.FileSelector(fileSelectedCallback, 'Load AMC Motion Capture')
