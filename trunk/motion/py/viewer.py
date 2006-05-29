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
from optparse import OptionParser
import time, Viewer

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


parser = OptionParser(usage="%prog [options] <asf file> <amc file>")
parser.add_option("--fps", dest="fps", type="int", default=25,
        help="Frames per second")
parser.add_option("-l", "--loop", dest="loop", default=False,
        action="store_true", help="Loop the sequence")
opts, args = parser.parse_args()

if len(args) < 2:
    parser.error("ASF and AMC file required")

# Load the ASF
asf = ASFReader()
asf.parse(args[0])

amc = AMC.from_file(args[1])
# Set up the display
pygame.init()
pygame.display.set_mode((640, 480), OPENGL | DOUBLEBUF)
resize((640, 480))
init()

# Run it
viewer = Viewer.Viewer(asf)
viewer.play(amc, opts.fps, opts.loop)

# vim: ts=4:sw=4:et
