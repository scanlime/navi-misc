#!/usr/bin/env python
#
# Menu test widget
#

from Wasabi import Main, Menu, Icon
from BZEngine.UI import Sequencer

class TestMain(Main.Main):
    def getPages(self):
        return [Sequencer.FadeIn(0.5, (0,0,0), Sequencer.FadeOut(0.25, (0,0,0), TestMenu))]

class TestMenu(Menu.ArcMenu):
    def __init__(self, book):
        # Show all icons
        items = [Menu.Item(Icon.load(name)) for name in Icon.getIconDict().keys()]
        Menu.ArcMenu.__init__(self, book, items, "All Icons")

TestMain().run()
