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
import AMCReader

def importData (reader, object, armature, filename):
    action = Blender.Armature.NLA.NewAction(filename)
    action.setActive(object)
    scene = Blender.Scene.getCurrent()
    context = scene.getRenderingContext()
    b = {}

    bones = armature.getBones()
    for bone in bones:
        b[bone.getName()] = bone

    for frame in reader.frames:
        context.currentFrame(frame.number)
        loc = frame.bones['root'][0:3]
        b['root'].setLoc(loc)

def fileSelectedCallback(filename):
    reader = AMCReader.AMCReader()
    try:
        reader.parse(filename)
    except IOError, s:
        AMCReader.log.err(s)
    else:
        objects = Blender.Object.GetSelected()
        if len(objects) == 0:
            AMCReader.log.error('Please select the armature to apply MC data to')
        else:
            armatures = Blender.Armature.Get()
            if len(armatures) == 0:
                AMCReader.log.error('No armatures exist!')
            else:
                if len(armatures) == 1:
                    importData(reader, objects[0], armatures[0], filename)
                else:
                    pass

    if AMCReader.log.numErrors:
        AMCReader.log.report('Errors in loading AMC file')

if AMCReader:
    Blender.Window.FileSelector(fileSelectedCallback, 'Load AMC Motion Capture')
