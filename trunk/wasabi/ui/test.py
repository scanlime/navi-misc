#!/usr/bin/env python
from BZEngine.UI import Viewport, ThreeDRender, Sequencer
from BZEngine import Event
from Wasabi import Logos, Menu, Icon
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

menuItems = [Menu.Item(Icon.load(name)) for name in Icon.getIconDict().keys()]

mainBook = Sequencer.CyclicBook(view, [
    Sequencer.FadeOut(0.2, (1,1,1), Sequencer.UserPageInterrupter(Logos.getLogoSubBook())),
    Sequencer.FadeIn(0.2, (1,1,1), Sequencer.FadeOut(1, (0,0,0), lambda book: Menu.RingMenu(book, menuItems)))
    ])

loop.run()
