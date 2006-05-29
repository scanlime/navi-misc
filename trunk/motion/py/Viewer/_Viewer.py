# Copyright (C) 2006 W. Evan Sheehan
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

from OpenGL.GL import *
from OpenGL.GLU import *
import pygame, Skeleton, time

class Viewer:
    def __init__(self, asf, skeleton=Skeleton.StickFigure):
        """Create an AMC viewer.

        'asf' is an ASFReader object used to build body for rendering.
        'skeleton' is the class used to draw the bones of the body.
        """
        # Create the bones from the ASF
        bones = {}
        bones["root"] = skeleton("root", asf.orientation, 0.0, asf.order)
        for name, bone in asf.bones.iteritems():
            bones[name] = skeleton(name, bone.direction, bone.length, bone.dof)

        # Build the body
        for line in asf.hierarchy:
            for child in line[1:]:
                bones[line[0]].addChild(bones[child])

        # Save the root because that's the only one we need
        self.root = bones["root"]

    def play(self, amc, fps=25, loop=False):
        """Play an AMC file.

        'amc' is an AMC object created from an AMC file. 'loop' sets whether to
        loop the sequence.
        """
        def buildFrame(data, index):
            frame = {}
            for name, value in data.iteritems():
                frame[name] = value[index]

            return frame

        frames = []
        for i in range(len(amc.bones.keys())):
            frames.append(buildFrame(amc.bones, i))

        while True:
            for frame in frames:
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT)
                glLoadIdentity()
                glTranslate(0.0, 0.0, -56.0)
                self.root.draw(frame)

                # FIXME - Hack because we're only using pygame right now, this
                # will need to be made more generic somehow.
                pygame.display.flip()

                time.sleep(1.0/fps)

            # Hack to loop playback
            if not loop:
                break

    def stillFrame(self, amc, frame):
        """Display only a single frame in a sequence.

        'amc' is an AMC object. 'frame' is the frame number in the sequence to
        display.
        """
        pass


# vim: ts=4:sw=4:et
