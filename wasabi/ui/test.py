#!/usr/bin/env python
from BZEngine.UI import Viewport, ThreeDRender, ThreeDControl, Sequencer, HUD, GLOrtho, Layout
from BZEngine import Event, Animated
from Wasabi import Logos, Icon
from math import *
import pygame

loop = Event.EventLoop()
viewport = Viewport.OpenGLViewport(loop)
viewport.setCaption("Wasabi Test")
view = ThreeDRender.View(viewport)

#viewport.display.toggle_fullscreen()
#pygame.mouse.set_visible(False)


class IconTest(Sequencer.Page):
    def __init__(self, book):
        Sequencer.Page.__init__(self, book)

        overlay = self.viewport.region(Layout.Rect(self.viewport))
        self.background = HUD.Image(overlay,
                                    "brushed_metal.png",
                                    overlay.size)

        self.dock = Icon.Dock(overlay, self.trackFunction, [
            Icon.Icon('icon_navi.png', 'Navi', imageAspect=1.623),
            Icon.Icon('icon_n64.png', 'Nintendo 64', imageAspect=1.04),
            Icon.Icon('icon_atari.png', 'Atari', imageAspect=1.52),
            Icon.Icon('icon_ps2.png', 'Playstation 2', imageAspect=0.983),
            Icon.Icon('icon_dreamcast.png', 'Dreamcast', imageAspect=1.66),
            Icon.Icon('icon_nes.png', 'Nintendo', imageAspect=1.594),
	    Icon.Icon('icon_sega.png', 'Genesis', imageAspect=1.679),
            ])

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

    def finalize(self):
        # UGLY HACK!!!!!!!!!
        self.dock = None


def globalKeyDown(ev):
    if ev.key == pygame.K_ESCAPE:
        loop.stop()
viewport.onKeyDown.observe(globalKeyDown)

mainBook = Sequencer.CyclicBook(view, [
    # Cycle through wasabi logos until user intervention, then fade out
    Sequencer.FadeOut(0.2, (1,1,1), Sequencer.UserPageInterrupter(Logos.getLogoSubBook())),

    # Icon test
    Sequencer.FadeIn(0.2, (1,1,1), Sequencer.FadeOut(1, (0,0,0), IconTest))
    ])

loop.run()
