#!/usr/bin/env python
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
import pygame
from pygame.locals import *
from Motion import ASFReader, AMC
from Viewer import Skeleton
from optparse import OptionParser

def resize((width, height)):
    """Size the OpenGL window"""
    if height==0:
        height=1
    glViewport(0, 0, width, height)
    glMatrixMode(GL_PROJECTION)
    glLoadIdentity()
    gluPerspective(45, 1.0*width/height, 0.1, 100.0)
    glMatrixMode(GL_MODELVIEW)
    glLoadIdentity()

def init():
    """Initialize the OpenGL window"""
    glShadeModel(GL_SMOOTH)
    glClearColor(0.0, 0.0, 0.0, 0.0)
    glClearDepth(1.0)
    glEnable(GL_DEPTH_TEST)
    glDepthFunc(GL_LEQUAL)
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST)

def buildFrame(data, index):
    frame = {}
    for name, value in data.iteritems():
        frame[name] = value[index]

    return frame


parser = OptionParser(usage="%prog <asf file>")
opts, args = parser.parse_args()

if len(args) < 1:
    parser.error("ASF file required")

asf = ASFReader()
asf.parse(args[0])

bones = {}

bones["root"] = Skeleton.StickFigure("root", asf.orientation, 0.0, asf.order)
for name, bone in asf.bones.iteritems():
    bones[name] = Skeleton.StickFigure(name, bone.direction, bone.length, bone.dof)

for line in asf.hierarchy:
    for child in line[1:]:
        bones[line[0]].addChild(bones[child])

pygame.init()
pygame.display.set_mode((640, 480), OPENGL | DOUBLEBUF)

resize((640, 480))
init()

while True:
    glLoadIdentity()
    glTranslate(0.0, 0.0, -56.0)
    bones["root"].draw()
    pygame.display.flip()


# vim: ts=4:sw=4:et
