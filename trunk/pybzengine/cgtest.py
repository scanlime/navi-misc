#!/usr/bin/env python
#
# Pretty much a straight translation of the CgGL example distributed with the Cg toolkit
# this is -not- platform agnostic at the moment
#
import sys
from BZEngine import Event
from BZEngine.UI import Viewport, HUD, Layout
from Cg import *
from OpenGL.GL import *
from OpenGL.GLU import *

normals = [(-1.0, 0.0,  0.0),
           (0.0,  1.0,  0.0),
	   (1.0,  0.0,  0.0),
	   (0.0, -1.0,  0.0),
	   (0.0,  0.0,  1.0),
	   (0.0,  0.0, -1.0)]

faces = [(0, 1, 2, 3),
         (3, 2, 6, 7),
	 (7, 6, 5, 4),
	 (4, 5, 1, 0),
	 (5, 6, 2, 1),
	 (7, 4, 0, 3)]

vertices = [(-1, -1,  1),
            (-1, -1, -1),
	    (-1,  1, -1),
	    (-1,  1,  1),
	    ( 1, -1,  1),
	    ( 1, -1, -1),
	    ( 1,  1, -1),
	    ( 1,  1,  1)]

program = None
Kd = None
modelViewProj = None
testColor = None

def CheckCGError():
    error = Cg.cgGetError()
    if error != CG_NO_ERROR:
        sys.exit('Cg error: %s' % Cg.cgGetErrorString(error))

def DrawCube():
    cgGLBindProgram(program)
    CheckCGError()

    if Kd != None:
        cgGLSetParameter4f(Kd, 1.0, 1.0, 0.0, 1.0)

    cgGLEnableProfile(CG_PROFILE_VP20)
    CheckCGError()

    glDisable(GL_LIGHTING)
    glEnable(GL_DEPTH_TEST)
    glMatrixMode(GL_PROJECTION)
    glLoadIdentity()
    gluPerspective(40, 1, 1, 10)
    glMatrixMode(GL_MODELVIEW)
    gluLookAt(0, 0, 5, 0, 0, 0, 0, 1, 0)
    glLoadIdentity()
    glColor3f(1, 1, 1)
    glTranslatef(0, 0, -6)
    glRotatef(60, 1, 0, 0)
    glRotatef(-20, 0, 0, 1)

    if modelViewProj != None:
        cgGLSetStateMatrixParameter(modelViewProj, CG_GL_MODELVIEW_PROJECTION_MATRIX, CG_GL_MATRIX_IDENTITY)

    for i in range(5):
        glBegin(GL_QUADS)
	glNormal3fv(normals[i])
	cgGLSetParameter3f(testColor, 1.0, 0.0, 0.0);
	glVertex3fv(vertices[faces[i][0]])
	cgGLSetParameter3f(testColor, 0.0, 1.0, 0.0);
	glVertex3fv(vertices[faces[i][1]])
	cgGLSetParameter3f(testColor, 0.0, 0.0, 1.0);
	glVertex3fv(vertices[faces[i][2]])
	cgGLSetParameter3f(testColor, 1.0, 1.0, 1.0);
	glVertex3fv(vertices[faces[i][3]])
	glEnd()

    cgGLDisableProfile(CG_PROFILE_VP20)
    CheckCGError()

loop = Event.EventLoop()
viewport = Viewport.OpenGLViewport(loop)

context = cgCreateContext()
CheckCGError()

program = cgCreateProgramFromFile(context, CG_SOURCE, 'extra_media/cgGL_vertex_example.cg', CG_PROFILE_VP20, 'main', None)
CheckCGError()

print 'LAST LISTING----%s----' % cgGetLastListing(context)
print '---- PROGRAM BEGIN ----'
programString = cgGetProgramString(program, CG_COMPILED_PROGRAM)
print programString
print '---- PROGRAM END ----'

cgGLLoadProgram(program)
CheckCGError()

Kd = cgGetNamedParameter(program, 'Kd')
CheckCGError()

modelViewProj = cgGetNamedParameter(program, 'ModelViewProj')
CheckCGError()

testColor = cgGetNamedParameter(program, 'IN.TestColor')
CheckCGError()

viewport.onDrawFrame.observe(DrawCube)

loop.run()

cgDestroyProgram(program)
cgDestroyContext(context)
