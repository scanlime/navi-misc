#!/usr/bin/python
import pygame, fieldbox, sys, pickle
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
        width = sensors[i] *10
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
    glTranslatef(0.4,-1.4,0)
    for i in range(0,8):
        height = sensors[i] * 60 
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

    sensor = fieldbox.filter(fieldbox.sensor())
    pattern = []

    for z in (0,0.25,0.5,0.75,1):
        for y in (0,0.25,0.5,0.75,1):
            for x in (0,0.25,0.5,0.75,1):
                coord = [x,y,z]
                print coord
                while 1:
                    event = pygame.event.poll()
                    draw(sensor.read())
                    if event.type == KEYDOWN:
                        break
                    pygame.display.flip()
                for i in range(0,5):
                    pattern.append( [sensor.read(), coord] )

    pickle.dump(pattern, open("nn_pattern.pickle", "w"))
    print "Patterns saved."

if __name__ == '__main__': main()
