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

armature    = None
armatureObj = None
asfReader   = None
scene       = None

def cleanup ():
    # Clean up our global data, so we can run multiple times without problems
    armature    = None
    armatureObj = None
    asfReader   = None
    scene       = None

def addVectors (a, b):
    x = []
    for i in range (len(a)):
        x.append (a[i] + b[i])
    return x

def loadASF (filename):
    print 'loading',filename
    asfReader = ASFReader.ASFReader ()
    try:
        asfReader.parse (filename)
    except IOError, s:
        ASFReader.log.err (s)

    if ASFReader.log.numErrors:
        # Report any errors we encountered and quit
        ASFReader.log.report ('Errors in loading ASF')
        cleanup ()
        return


    # Load our data into an armature.
    scene = Blender.Scene.getCurrent ()
    armatureObj = Blender.Object.New ('Armature', asfReader.name)
    armature = Blender.Armature.New ()

    bones = {}

    # Create all the bones. for now, they're just positioned at the origin.
    for name, data in asfReader.bones.iteritems ():
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
    for set in asfReader.hierarchy:
        parent = set[0]
        for child in set[1:]:
            bones[child].setTail (addVectors (bones[child].tail, bones[parent].tail))
            bones[child].setHead (addVectors (bones[child].head, bones[parent].tail))

    # Iterate through the hierarchy again, linking bones to their parents and
    # adding them to the armature object.
    for set in asfReader.hierarchy:
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

def getRotation (bone):
    pass

def loadAMC (filename):
    print 'loading',filename
    amcReader = AMCReader.AMCReader ()
    try:
        amcReader.parse (filename)
    except IOError, s:
        AMCReader.log.err (s)

    if AMCReader.log.numErrors:
        AMCReader.log.report ('Errors in loading AMC')
        cleanup ()

    # Create a new action, named after the file
    action = Blender.Armature.NLA.NewAction (filename.split (Blender.sys.dirsep)[-1])
    action.setActive (armatureObj)

    context = scene.getRenderingContext ()
    # FIXME - Pretty much all of the data we've gotten is at 120Hz. It would
    # be nice to pull this out of the data file, but for now, just hardcode it.
    context.framesPerSec (120)

    totalFrames = len (amcReader.frames)

    Blender.Window.WaitCursor (True)
    for frame in amcReader.frames:
        context.currentFrame (frame.number)

        # FIXME - a progress bar is nice, but profiling this to find out why it's so
        # freaking slow would be nicer
        Blender.Window.DrawProgressBar (float (frame.number) / totalFrames, 'Frame %d of %d' % (frame.number, totalFrames))

        # Set root position/orientation. We need to and scale this just like we
        # did for the positions of the individual bones
        location = [(n * 0.1) for n in frame.bones['root'][0:3]]
        rotation = Blender.Mathutils.Euler (frame.bones['root'][3:6]).toQuat ()

        bones['root'].setLoc (location)
        bones['root'].setQuat (quat)
        bones['root'].setPose ([ROT, LOC])

        # Set orientations for each bone for this frame
        for name, bone in frame.bones.iteritems ():
            if name == 'root':
                continue
            bone = bones[bname]
            quat = getRotation (bone)
            #bones[name].setQuat (quat)
            #bones[name].setPose ([ROT])

    # No more wait indicators, since we're done
    Blender.Window.WaitCursor (False)
    Blender.Window.DrawProgressBar (1.0, 'Finished AMC import')
    Blender.Window.RedrawAll ()
    cleanup ()

# Show our ASF file selector
Blender.Window.FileSelector (loadASF, 'Load ASF Skeleton File')
