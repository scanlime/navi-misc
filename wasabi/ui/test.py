#!/usr/bin/env python
from BZEngine.UI import Viewport, ThreeDRender, Sequencer, Layout, HUD, ThreeDControl, Input
from BZEngine import Event
from Wasabi import Logos, Menu, Icon, IR
from math import *
import pygame

# Set up the usual pybzflag event loop, viewport, and view
loop = Event.EventLoop()
viewport = Viewport.OpenGLViewport(loop)
viewport.setCaption("Wasabi UI")
view = ThreeDRender.View(viewport)

viewport.display.toggle_fullscreen()
pygame.mouse.set_visible(0)

# Set up an LIRC client
IR.getDefaultClient().attachToLoop(loop)

# A sequencer page that displays a piece of text in the center of the screen, over a background image
class TextPage(Sequencer.Page):
    def __init__(self, book, text):
        Sequencer.Page.__init__(self, book)
        self.overlay = self.viewport.region(Layout.Rect(self.viewport))
        self.background = HUD.Background(self.overlay, "brushed_metal.png")
        self.title = HUD.Text(self.overlay, text, fontSize=50, shadow=True, alignment=(0.5,0.5))


# A RingMenu subclass that shows every loadable icon and includes an onSelected handler
# to display a TextPage with the item's name whenever one is selected.
class TestMenu(Menu.RingMenu):
    def __init__(self, book):
        # Construct a list of Menu.Item instances for each loaded icon.
        menuItems = [Menu.Item(Icon.load(name)) for name in Icon.getIconDict().keys()]
        Menu.RingMenu.__init__(self, book, menuItems)

    def onSelected(self, item):
        text = lambda book: TextPage(book, item.icon.text)
        decoratedText = Sequencer.FadeIn(0.25, (0,0,0), Sequencer.FadeOut(1, (0,0,0), Sequencer.UserPageInterrupter(text)))
        self.book.pushBack(decoratedText)

def logoInterrupter(book):
    events = [
        Input.KeyPress(book.viewport, pygame.K_SPACE),
	IR.ButtonPress(book.viewport),
	]
    return Sequencer.PageInterrupter(events, Logos.getLogoSubBook())(book)

mainBook = Sequencer.CyclicBook(view, [
    Sequencer.FadeOut(0.2, (1,1,1), logoInterrupter),
    Sequencer.FadeIn(0.5, (1,1,1), Sequencer.FadeOut(0.25, (0,0,0), TestMenu))
    ])

loop.run()
