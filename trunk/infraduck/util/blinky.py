#!/usr/bin/python
import pygame, time
from OpenGL.GL import *
from pygame.locals import *

pygame.init()
surface = pygame.display.set_mode((1024,768), OPENGL|DOUBLEBUF)

delay = 0.5 / 48.2

while True:
    glClearColor(0,0,0,0)
    glClear(GL_COLOR_BUFFER_BIT)
    pygame.display.flip()
    time.sleep(delay)
    glClearColor(1,1,1,0)
    glClear(GL_COLOR_BUFFER_BIT)
    pygame.display.flip()
    time.sleep(delay)
