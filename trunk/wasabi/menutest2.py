#!/usr/bin/env python
#
# Menu test widget
#

from Wasabi import Main, Menu, Icon

class TestMain(Main.Main):
    def getPages(self):
        return [Menu.defaultFades(TestMenu)]

class TestMenu(Menu.VerticalMenu):
    def __init__(self, book):
        # Show all icons
        items = [Menu.Item(Icon.load(name)) for name in Icon.getIconDict().keys()]
        Menu.VerticalMenu.__init__(self, book, items)

TestMain().run()
