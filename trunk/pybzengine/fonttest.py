#!/usr/bin/env python
#
# Test rig for our GLText module
#

from BZEngine.UI import Viewport, GLText, Instrument
from BZEngine.Event import EventLoop
from BZEngine import Animated
from OpenGL.GL import *
from math import floor


# Create a new orthogonal mode viewport
loop = EventLoop()
viewport = Viewport.OpenGLViewport(loop, (640,480))
viewport.setCaption("Font Test")
viewport.fov = None

time = Animated.Timekeeper()
spin = Animated.Value(Animated.Velocity(80))
zoom = Animated.Value(Animated.SineFunction(0.5, (0.8,1)))

def drawFrame():
    # Gradient
    glLoadIdentity()
    glDisable(GL_TEXTURE_2D)
    glBegin(GL_QUADS)
    glColor3f(0,0,0)
    glVertex2f(0,0)
    glVertex2f(viewport.size[0],0)
    glColor3f(1,1,1)
    glVertex2f(viewport.size[0],viewport.size[1])
    glVertex2f(0,viewport.size[1])
    glEnd()

    glLoadIdentity()
    glTranslatef(5,5,0)
    glColor3f(1,1,1)
    GLText.draw("\n\nBoing! This is the default size\n\n")
    glColor3f(1,1,0.5)
    GLText.draw("A little larger", 50)

    glLoadIdentity()
    glTranslatef(floor(viewport.size[0]/2), floor(viewport.size[1]/2), 0)
    glRotatef(spin.value, 0,0,1)
    glScalef(zoom.value, zoom.value, zoom.value)
    dt = time.step()
    spin.integrate(dt)
    zoom.integrate(dt)
    GLText.drawCentered("Spinny-widget", 30)

    glColor3f(0,0,0)
    glLoadIdentity()
    glTranslatef(floor(viewport.size[0]/2), floor(viewport.size[1] * 0.75), 0)
    GLText.drawCentered("Not-so-spinny-widget")

    glLoadIdentity()
    glTranslatef(floor(viewport.size[0]/2), floor(viewport.size[1] * 0.85), 0)
    GLText.drawCentered("(small print)", GLText.smallSize)

    glLoadIdentity()
    glTranslatef(floor(viewport.size[0]/2), floor(viewport.size[1] * 0.95), 0)
    glColor3f(0,0,1)
    GLText.drawCentered("bold", fontName="VeraBd.ttf")

viewport.onDrawFrame.observe(drawFrame)
frameRateInstrument = Instrument.FrameRate(viewport)
loop.run()


