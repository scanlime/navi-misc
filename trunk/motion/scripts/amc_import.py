#!BPY
#
# amc_import.py - Import an Acclaim Motion Capture file and apply NLA
#                 transformations to a matching armature
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
Name: 'AMC Motion Capture (.amc)...'
Blender: 236
Group: 'Import'
Tip: 'Imports an Acclaim AMC file and applies motion capture data to an armature'
"""

import Blender
from Blender.Armature.Bone import ROT, LOC
import AMCReader

dofTable = {}

def getQuat(object, bone, rot):
    try:
        dof = dofTable[bone]
    except KeyError:
        dof = object.getProperty('%s-dof' % bone).getData().split(',')
        dofTable[bone] = dof
    r = [0.0, 0.0, 0.0]
    i = 0
    if dof:
        for d in dof:
            if d == 'rx':
                r[0] = rot[i]
            elif d == 'ry':
                r[2] = rot[i]
            else:
                r[1] = rot[i]
            i += 1
    return r

def importData (reader, object, filename):
    action = Blender.Armature.NLA.NewAction(filename)
    action.setActive(object)
    armature = object.getData()
    scene = Blender.Scene.getCurrent()
    context = scene.getRenderingContext()
    b = {}

    bones = armature.getBones()
    for bone in bones:
        b[bone.getName()] = bone

    for frame in reader.frames:
        context.currentFrame(frame.number)

        # set root position/rotation
        #loc = frame.bones['root'][0:3]
        #loc[0], loc[2], loc[1] = loc
        rot = frame.bones['root'][3:6]
        rot[0], rot[2], rot[1] = rot
        euler = Blender.Mathutils.Euler(rot)
        quat = euler.toQuat()
        #b['root'].setLoc(loc)
        b['root'].setQuat(quat)
        #b['root'].setPose([ROT,LOC], action)
        b['root'].setPose([ROT], action)

        print frame.number

        for bname, bone in frame.bones.iteritems():
            if bname == 'root':
                continue
            rot = frame.bones[bname]
            rot = getQuat(object, bname, rot)
            euler = Blender.Mathutils.Euler(rot)
            quat = euler.toQuat()
            b[bname].setQuat(quat)
            b[bname].setPose([ROT], action)

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
