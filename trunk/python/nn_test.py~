#!/usr/bin/python
import pygame, fieldbox, sys, pickle, bpnn
from OpenGL.GL import *
from OpenGL.GLU import *
from threading import *
from math import *
from pygame.locals import *


def draw(sensors, nnoutput):
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT)
    glLoadIdentity()
    glTranslatef(0.0,0.0,-4.0)

    # Raw values
    glPushMatrix()
    glTranslatef(-2,-1.4,0)
    for i in range(0,8):
        height = sensors[i] * 20
        glColor4fv(fieldbox.colors[i])
        glBegin(GL_QUADS)
        glVertex3f(-0.05,0,0)
        glVertex3f(-0.05,height,0)
        glVertex3f( 0.05,height,0)
        glVertex3f( 0.05,0,0)
        glEnd()
        glTranslatef(0.2,0,0)
    glPopMatrix()

    # Neural network output values
    glPushMatrix()
    glTranslatef(0.7,-1.4,0)
    glScalef(1,1,0.25)
    glColor4f(1,1,0.5,1)
    for i in nnoutput:
        height = i
        glBegin(GL_QUADS)
        glVertex3f(-0.05,0,0)
        glVertex3f(-0.05,height,0)
        glVertex3f( 0.05,height,0)
        glVertex3f( 0.05,0,0)
        glEnd()
        glBegin(GL_LINES)
        for y in (0,0.5,1):
            glVertex3f(-0.08,y,0)
            glVertex3f(-0.1,y,0)
        glEnd()
        glTranslatef(0.2,0,0)
    glPopMatrix()

    # Cartesian graph
    if len(nnoutput) >= 3:
        z = nnoutput[2]
    else:
        z = 1
    if len(nnoutput) >= 2:
        y = nnoutput[1]
    else:
        y = 0
    x = nnoutput[0]
    
    glPushMatrix()
    glScalef(1.6,-1.6,1.6)
    glRotatef(60,1,0,0)
    glTranslatef(-0.5,-0.2,0.3)
    glScalef(1,1,0.6)
    
    # Base rectangle
    glColor4f(0.5,0.5,0.5,1)
    glBegin(GL_LINE_LOOP)
    glVertex3f(0,0,0)
    glVertex3f(1,0,0)
    glVertex3f(1,1,0)
    glVertex3f(0,1,0)
    glEnd()
    
    # Position indicator cube
    glColor4f(0.9,0.9,1,1)
    glPushMatrix()
    glTranslatef(x,y,z+0.01)
    glScalef(0.01,0.01,0.01)
    glBegin(GL_QUADS)
    glVertex3f(-1,-1,-1)
    glVertex3f( 1,-1,-1)
    glVertex3f( 1, 1,-1)
    glVertex3f(-1, 1,-1)
    glVertex3f(-1,-1, 1)
    glVertex3f( 1,-1, 1)
    glVertex3f( 1, 1, 1)
    glVertex3f(-1, 1, 1)
    glVertex3f(-1, 1,-1)
    glVertex3f( 1, 1,-1)
    glVertex3f( 1, 1, 1)
    glVertex3f(-1, 1, 1)
    glVertex3f(-1,-1,-1)
    glVertex3f( 1,-1,-1)
    glVertex3f( 1,-1, 1)
    glVertex3f(-1,-1, 1)
    glVertex3f(-1,-1,-1)
    glVertex3f(-1, 1,-1)
    glVertex3f(-1, 1, 1)
    glVertex3f(-1,-1, 1)
    glVertex3f( 1,-1,-1)
    glVertex3f( 1, 1,-1)
    glVertex3f( 1, 1, 1)
    glVertex3f( 1,-1, 1)
    glEnd()
    glPopMatrix()

    # Shadows of the position cube on each axis plane
    glColor4f(0.5,0.5,0.8,1)
    glPushMatrix()
    glTranslatef(x,y,0)
    glScalef(0.01,0.01,0.01)
    glBegin(GL_LINE_LOOP)
    glVertex3f(-1,-1,0)
    glVertex3f( 1,-1,0)
    glVertex3f( 1, 1,0)
    glVertex3f(-1, 1,0)
    glEnd()
    glPopMatrix()
    glPushMatrix()
    glTranslatef(x,0,z)
    glScalef(0.01,0.01,0.01)
    glBegin(GL_LINE_LOOP)
    glVertex3f(-1,0,-1)
    glVertex3f( 1,0,-1)
    glVertex3f( 1,0, 1)
    glVertex3f(-1,0, 1)
    glEnd()
    glPopMatrix()
    glPushMatrix()
    glTranslatef(0,y,z)
    glScalef(0.01,0.01,0.01)
    glBegin(GL_LINE_LOOP)
    glVertex3f(0,-1,-1)
    glVertex3f(0, 1,-1)
    glVertex3f(0, 1, 1)
    glVertex3f(0,-1, 1)
    glEnd()    
    glPopMatrix()

    glPopMatrix()


def init():
    glEnable(GL_BLEND)
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA)
    glShadeModel(GL_SMOOTH)
    glClearColor(0.0, 0.0, 0.0, 0.0)
    glClearDepth(1.0)
    #glEnable(GL_DEPTH_TEST)
    glDepthFunc(GL_LEQUAL)
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST)


def resize((width, height)):
    if height==0:
        height=1
    glViewport(0, 0, width, height)
    glMatrixMode(GL_PROJECTION)
    glLoadIdentity()
    gluPerspective(45, 1.0*width/height, 0.1, 100.0)
    glMatrixMode(GL_MODELVIEW)
    glLoadIdentity()

def main():
    global sensorPacket
    video_flags = OPENGL|DOUBLEBUF
    
    pygame.init()
    resolution = (1024,768)
    surface = pygame.display.set_mode(resolution, video_flags)
    resize(resolution)
    init()

    ticks = pygame.time.get_ticks()

    sensor = fieldbox.filter(fieldbox.sensor())
    nn = pickle.load(open("nn_network.pickle"))

    while 1:
        event = pygame.event.poll()
        if event.type == QUIT or (event.type == KEYDOWN and event.key == K_ESCAPE):
            break
        readings = sensor.read()
        draw(readings, nn.update(readings))
        pygame.display.flip()


if __name__ == '__main__': main()
