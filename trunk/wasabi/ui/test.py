#!/usr/bin/env python
from BZEngine.UI import Viewport, ThreeDRender, ThreeDControl, Sequencer, HUD, GLOrtho
from BZEngine import Event, Animated
from Wasabi import Logos, Icon
from math import *
import pygame

loop = Event.EventLoop()
viewport = Viewport.OpenGLViewport(loop)
viewport.setCaption("Wasabi Test")
view = ThreeDRender.View(viewport)


class IconTest(Sequencer.Page):
    def __init__(self, book):
        Sequencer.Page.__init__(self, book)

        overlay = self.viewport.region(self.viewport.rect)
        self.background = HUD.Image(overlay,
                                    "brushed_metal.png",
                                    overlay.size)

        icon = Icon.Icon('navi512.png', 'Navi', imageAspect=1.623)

        self.dock = Icon.Dock(overlay, self.trackFunction, [icon] * 5)

        self.viewport.onKeyDown.observe(self.keyDown)

    def keyDown(self, ev):
        if ev.key == pygame.K_SPACE:
            self.onFinish()
        elif ev.key == pygame.K_LEFT:
            self.dock.selectionIndex -= 1
        elif ev.key == pygame.K_RIGHT:
            self.dock.selectionIndex += 1

    def trackFunction(self, x):
        """An example track function that moves the icons along a circle in
           the middle of the screen. The bottom of the circle is at 0,
           increasing parameters move clockwise. The icon at 0 is the largest.
           """
        radius = self.viewport.size[1] * 0.04
        theta = -x * 2*pi + pi/2
        size = pow(sin(theta)*0.5+0.5, 3) * 120 + 40
        center = (self.viewport.size[0] * 0.5,
                  self.viewport.size[1] * 0.25)
        aspect = 2.9
        return (
            (cos(theta) * aspect * (radius + size) + center[0],
             sin(theta) * (radius + size) + center[1]),
            size
            )


mainBook = Sequencer.CyclicBook(view, [
    # Cycle through wasabi logos until user intervention, then fade out
    Sequencer.FadeOut(0.2, (1,1,1), Sequencer.UserPageInterrupter(Logos.getLogoSubBook())),

    # Icon test
    Sequencer.FadeIn(0.2, (1,1,1), Sequencer.FadeOut(1, (0,0,0), IconTest))
    ])

# Exit our main loop once the mainBook finishes
mainBook.onFinish.observe(loop.stop)
loop.run()
