#!BPY
#
# asf-amc_import.py - Import Acclaim ASF/AMC motion capture data, creating
#       an armature and keyframes for each pose
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
Name: 'ASF+AMC motion capture...'
Blender: 236
Group: 'Import'
Tip: 'Imports Acclaim ASF+AMC and builds an armature with the MC data'
"""

import Blender
import ASFReader
import AMCReader
import math
import string

# make it faster?
try:
    import psyco
except:
    pass

armature = None
armatureObj = None

def addVectors(a, b):
    x = []
    for i in range (len(a)):
        x.append (a[i] + b[i])
    return x

def loadASF (filename):
    reader = ASFReader.ASFReader ()
    try:
        reader.parse (filename)
    except IOError, s:
        ASFReader.log.err (s)

    if ASFReader.log.numErrors:
        # Report any errors we encountered and quit
        ASFReader.log.report ('Errors in loading ASF')
        return


    # Load our data into an armature.
    scene = Blender.Scene.getCurrent ()
    armatureObj = Blender.Object.New ('Armature', reader.name)
    armature = Blender.Armature.New ()

    bones = {}

    # Create all the bones. for now, they're just positioned at the origin.
    for name, data in reader.bones.iteritems ():
        bone = Blender.Armature.Bone.New (name)

        # Head is at the origin. Tail is direction * length. Scale by 1/10
        # to better fit within blender's coordinate system.

        # FIXME - should probably do something more intelligent with scaling,
        # but that's a hard problem.
        bone.setHead (0.0, 0.0, 0.0)
        tail = [(float(n) * 0.1 * data.length) for n in data.direction]
        # Swizzle Y and Z here. ASF has Y pointing up, but blender has a more
        # traditional Z-up.
        bone.setTail ([tail[0], tail[2], tail[1]])

        bones[name] = bone

    # Create the root and add it to the armature.
    bone = Blender.Armature.Bone.New ('root')
    bone.setHead (0.0, 0.0, 0.0)
    bone.setTail (0.0, 0.0, 0.0)
    bones['root'] = bone
    armature.addBone (bone)

    # Iterate through the hierarchy and position child bones at the tails of
    # their parents. Blender doesn't require a bone and its parent to be
    # touching, but ASF does.
    for set in reader.hierarchy:
        parent = set[0]
        for child in set[1:]:
            bones[child].setTail (addVectors (bones[child].tail, bones[parent].tail))
            bones[child].setHead (addVectors (bones[child].head, bones[parent].tail))

    # Iterate through the hierarchy again, linking bones to their parents and
    # adding them to the armature object.
    for set in reader.hierarchy:
        parent = set[0]
        for child in set[1:]:
            bones[child].setParent (bones[parent])
            armature.addBone (bones[child])

    # Link the armature to it's object, add it to the scene and queue a window redraw.
    armatureObj.link (armature)
    scene.link (armatureObj)
    armatureObj.makeDisplayList ()
    Blender.Window.RedrawAll ()

    # Finally, pop up a file selector for importing the AMC
    Blender.Window.FileSelector (loadAMC, 'Load AMC Motion Capture')

def loadAMC (filename):
    armature = None
    armatureObj = None

Blender.Window.FileSelector (loadASF, 'Load ASF Skeleton File')
