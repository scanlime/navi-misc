#!/usr/bin/python
import pygame, fieldbox
from OpenGL.GL import *
from OpenGL.GLU import *
from threading import *
from math import *
from pygame.locals import *


def draw(sensors):
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT)
    glLoadIdentity()
    glTranslatef(0.0,0.0,-4.0)

    glEnable(GL_POLYGON_SMOOTH)
    glPushMatrix()
    glTranslatef(-1.0,0,0)
    for i in range(0,8):
        width = sensors[i] *80
        pair = fieldbox.platePairs[i]
        glColor4fv(fieldbox.colors[i])
        glBegin(GL_TRIANGLES)
        txPosition = fieldbox.platePositions[pair[0]]
        glVertex3f(txPosition[0]-width, txPosition[1], txPosition[2])
        glVertex3f(txPosition[0]+width, txPosition[1], txPosition[2])
        glVertex3fv(fieldbox.platePositions[pair[1]])
        glEnd()
    glPopMatrix()
    glDisable(GL_POLYGON_SMOOTH)

    glPushMatrix()
    glTranslatef(0.4,0,0)
    for i in range(0,8):
        height = sensors[i] * 300
        glColor4fv(fieldbox.colors[i])
        glBegin(GL_QUADS)
        glVertex3f(-0.05,0,0)
        glVertex3f(-0.05,height,0)
        glVertex3f( 0.05,height,0)
        glVertex3f( 0.05,0,0)
        glEnd()
        glTranslatef(0.2,0,0)
    glPopMatrix()

def init():
    glEnable(GL_BLEND)
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA)
    glShadeModel(GL_SMOOTH)
    glClearColor(0.0, 0.0, 0.0, 0.0)
    glClearDepth(1.0)
    glEnable(GL_DEPTH_TEST)
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

    print "Initializing sensor, don't touch it!"
    sensor = fieldbox.baselineRelative(fieldbox.filter(fieldbox.sensor()))
    print "Running..."
    while 1:
        event = pygame.event.poll()
        if event.type == QUIT or (event.type == KEYDOWN and event.key == K_ESCAPE):
            break
        draw(sensor.read())
        pygame.display.flip()


if __name__ == '__main__': main()
