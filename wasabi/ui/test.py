#!/usr/bin/env python
from BZEngine.UI import Viewport, ThreeDRender, Sequencer
from BZEngine import Event, Animated
from Wasabi import Logos, Menu
from math import *
import pygame

loop = Event.EventLoop()
viewport = Viewport.OpenGLViewport(loop)
viewport.setCaption("Wasabi Test")
view = ThreeDRender.View(viewport)

#viewport.display.toggle_fullscreen()
#pygame.mouse.set_visible(False)

def globalKeyDown(ev):
    if ev.key == pygame.K_ESCAPE:
        loop.stop()
viewport.onKeyDown.observe(globalKeyDown)

mainBook = Sequencer.CyclicBook(view, [
    # Cycle through wasabi logos until user intervention, then fade out
    Sequencer.FadeOut(0.2, (1,1,1), Sequencer.UserPageInterrupter(Logos.getLogoSubBook())),

    # Icon test
    Sequencer.FadeIn(0.2, (1,1,1), Sequencer.FadeOut(1, (0,0,0), Menu.RingMenu))
    ])

loop.run()
