#!BPY
#
# asf-import.py - importer for ASF & AMC (acclaim skeleton file &
#       acclaim motion capture) data to blender
#
# Copyright (C) 2005-2007 David Trowbridge
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
Name: 'ASF Skeleton (.asf)...'
Blender: 236
Group: 'Import'
Tip: 'Import an ASF (Acclaim Skeleton File)'
"""

__author__ = "David Trowbridge"

import Blender
from Motion import ASFReader

def ASFImport (filename):
    reader = ASFReader.ASFReader ()
    try:
        reader.parse (filename)
    except IOError, s:
        ASFReader.log.err (s)

    if ASFReader.log.numErrors > 0:
        ASFReader.log.report ('Errors in loading ASF')
        return

    scene = Blender.Scene.getCurrent ()
    armature_obj = Blender.Object.New ('Armature', asf_reader.name)
    armature = Blender.Armature.New ()

    for name, data in asf_reader.bones.iteritems ():
        bone = Blender.Armature.Bone.New (name)

        # Head is at the origin. Tail is direction * length. Scale by 1/10
        # to better fit within blender's coordinate system.

        # FIXME - should probably do something more intelligent with scaling,
        # but that's a harder problem.  There may also be a way to scale the
        # entire armature instead of individual bones.
        bone.setHead (0.0, 0.0, 0.0)
        tail = [(float(n) * 0.1 * data.length) for n in data.direction]

        # Swizzle Y and Z here. ASF has Y pointing up, but blender has a more
        # traditional/sane Z-up.
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

def filesel_callback (filename):
    Blender.Window.WaitCursor (1)
    ASFImport (filename);
    Blender.Window.WaitCursor (0)

Blender.Window.FileSelector (filesel_callback, 'Load ASF Skeleton File')
