#!BPY
#
# asf-amc_import.py - Import Acclaim ASF/AMC motion capture data, creating
#       an armature and keyframes for each pose
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

from Blender.Armature.Bone import ROT, LOC

# make it faster?
try:
    import psyco
except:
    pass

class dataStorage:
    armature    = None
    armatureObj = None
    asfReader   = None
    scene       = None
    bones       = {}

def cleanup (d):
    # Clean up our global data, so we can run multiple times without problems
    d.armature    = None
    d.armatureObj = None
    d.asfReader   = None
    d.scene       = None
    d.bones       = {}

def addVectors (a, b):
    x = []
    for i in range (len(a)):
        x.append (a[i] + b[i])
    return x

def loadASF (filename):
    d = dataStorage ()
    d.asfReader = ASFReader.ASFReader ()
    try:
        d.asfReader.parse (filename)
    except IOError, s:
        ASFReader.log.err (s)

    if ASFReader.log.numErrors:
        # Report any errors we encountered and quit
        ASFReader.log.report ('Errors in loading ASF')
        cleanup (d)
        return

    # Load our data into an armature.
    d.scene = Blender.Scene.getCurrent ()
    d.armatureObj = Blender.Object.New ('Armature', d.asfReader.name)
    d.armature = Blender.Armature.New ()

    # Create all the bones. for now, they're just positioned at the origin.
    for name, data in d.asfReader.bones.iteritems ():
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

        d.bones[name] = bone

    # Create the root and add it to the armature.
    bone = Blender.Armature.Bone.New ('root')
    bone.setHead (0.0, 0.0, 0.0)
    bone.setTail (0.0, 0.0, 0.0)
    d.bones['root'] = bone
    d.armature.addBone (bone)

    # Iterate through the hierarchy and position child bones at the tails of
    # their parents. Blender doesn't require a bone and its parent to be
    # touching, but ASF does.
    for set in d.asfReader.hierarchy:
        parent = set[0]
        for child in set[1:]:
            d.bones[child].setTail (addVectors (d.bones[child].tail, d.bones[parent].tail))
            d.bones[child].setHead (addVectors (d.bones[child].head, d.bones[parent].tail))

    # Iterate through the hierarchy again, linking bones to their parents and
    # adding them to the armature object.
    for set in d.asfReader.hierarchy:
        parent = set[0]
        for child in set[1:]:
            d.bones[child].setParent (d.bones[parent])
            d.armature.addBone (d.bones[child])

    # Link the armature to it's object, add it to the scene and queue a window redraw.
    d.armatureObj.link (d.armature)
    d.scene.link (d.armatureObj)
    d.armatureObj.makeDisplayList ()

    # Finally, pop up a file selector for importing the AMC
    # Blender.Window.FileSelector (loadAMC, 'Load AMC Motion Capture')
    loadAMC ('/home/david/projects/motion/data/1a_5_small.amc', d)

def getRotation (bone, rotation, d):
    """Retrieves the rotation for a bone, correcting for the difference between
       ASF angles and blender's orientation system.
       """
    name = bone.getName ()

    # Retrieve the axis as stored in the ASF file from the ASFReader, and the blender
    # axes from the rest matrix of the bone. Create a correction euler to convert
    # between these two reference frames
    axis = Blender.Mathutils.Euler (map (float, d.asfReader.bones[name].axis[0:3]))


    baxis = bone.getRestMatrix ('bonespace').rotationPart ().toEuler ()

    # Find the degrees of freedom supported by this joint and map our rotation
    # parameter onto a 3-tuple based on this
    dof = d.asfReader.bones[name].dof

    # Some bones have no DOF, so just use an empty list.
    if dof is None:
        dof = []

    # Create a worldspace representation of this bone's position
    euler = [0.0, 0.0, 0.0]
    for i in range (len (dof)):
        if dof[i] == 'rx':
            euler[0] = rotation[i] + axis.x
        if dof[i] == 'ry':
            euler[1] = rotation[i] + axis.y
        if dof[i] == 'rz':
            euler[2] = rotation[i] + axis.z
    euler = Blender.Mathutils.Euler (euler)

    # Subtract away all our parent's transformations
    parent = bone
    while parent.hasParent ():
        parent = parent.getParent ()
        parentRotation = parent.getQuat().toEuler ()
        euler.x = euler.x - parentRotation.x
        euler.y = euler.y - parentRotation.y
        euler.z = euler.z - parentRotation.z

    # finally, subtract away our rest position
    rest = bone.getRestMatrix('bonespace').rotationPart().toEuler ()
    euler.x = euler.x - rest.x
    euler.y = euler.y - rest.y
    euler.z = euler.z - rest.z

    return euler.toQuat ()

def loadAMC (filename, d):
    amcReader = AMCReader.AMCReader ()
    try:
        amcReader.parse (filename)
    except IOError, s:
        AMCReader.log.err (s)

    if AMCReader.log.numErrors:
        AMCReader.log.report ('Errors in loading AMC')
        cleanup (d)
        return

    # Create a new action, named after the file
    action = Blender.Armature.NLA.NewAction (filename.split (Blender.sys.dirsep)[-1])
    action.setActive (d.armatureObj)

    context = d.scene.getRenderingContext ()
    # FIXME - All of the data we've got from CMU is at 120Hz, but capture rate
    # can vary. There's no place that this is stored in either file, so it would
    # be nice to let the user enter this. It would also be nice to be able to drop
    # frames, since 120Hz is a little excessive
    context.framesPerSec (120)

    totalFrames = len (amcReader.frames)

    Blender.Window.WaitCursor (True)
    for i in range (len (amcReader.frames)):

        # 4/5 dentists recommend frame decimation!
        if (i % 5 != 0):
            continue
        frame = amcReader.frames[i]
        context.currentFrame (frame.number)

        # FIXME - a progress bar is nice, but profiling this to find out why it's so
        # freaking slow would be nicer
        Blender.Window.DrawProgressBar (float (frame.number) / totalFrames, 'Frame %d of %d' % (frame.number, totalFrames))
        print 'Frame %d of %d' % (frame.number, totalFrames)

        # Set root position/orientation. We need to swizzle and scale this just
        # like we did for the positions of the individual bones, but we also need
        # to flip the Z orientation, for some reason.
        location = [(n * 0.1) for n in frame.bones['root'][0:3]]
        location = [location[0], location[2] * -1, location[1]]
        rotation = Blender.Mathutils.Euler (frame.bones['root'][3:6]).toQuat ()

        #d.bones['root'].setLoc (location)
        #d.bones['root'].setQuat (rotation)
        #d.bones['root'].setPose ([ROT, LOC])

        # Run through this using the hierarchy list in the ASF file rather than the
        # order we get in the AMC. AMC transformations are order independent, since
        # each bone has it's own coordinate system that's related to the global one.
        # However, blender does things based on the transformation of the parent,
        # so we need to be careful to start at the root and work our way out to the
        # terminals.
        for set in d.asfReader.hierarchy:
            for child in set[1:]:
                bone = d.bones[child]
                quat = None

                # If we don't have any degrees of freedom, we don't see a rotation
                # in the AMC file. In that case, just pass 0,0,0 euler.
                try:
                    rotation = frame.bones[child]
                    quat = getRotation (bone, rotation, d)
                except KeyError:
                    quat = getRotation (bone, [0.0, 0.0, 0.0], d)
                d.bones[child].setQuat (quat)
                d.bones[child].setPose ([ROT])

    # No more wait indicators, since we're done
    Blender.Window.WaitCursor (False)
    Blender.Window.DrawProgressBar (1.0, 'Finished AMC import')
    Blender.Window.RedrawAll ()
    cleanup (d)

# Show our ASF file selector
Blender.Window.FileSelector (loadASF, 'Load ASF Skeleton File')
