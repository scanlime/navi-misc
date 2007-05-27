#!/usr/bin/env python

import rwand, sys, random


class MenuItem(rwand.KeyListener):
    def __init__(self, name):
        self.name = name
        self.renderer = rwand.TextRenderer(name)


class MenuList(rwand.KeyListener):
    def __init__(self, items):
        self.items = items
        self.index = None

    def select(self, rwdc, index):
        """Select a new item, given its index. The menu is a torus. Out-of-range
           indices will wrap around. This automatically picks a transition, based
           on the old and new indices.
           """
        wrappedIndex = index % len(self.items)
        item = self.items[wrappedIndex]

        if self.items is None:
            rwdc.renderer = rwand.Dissolve(rwdc.renderer, item.renderer)
        elif index > self.index:
            rwdc.renderer = rwand.VScroll(rwdc.renderer, item.renderer, rwand.VScroll.UP)
        else:
            rwdc.renderer = rwand.VScroll(rwdc.renderer, item.renderer, rwand.VScroll.DOWN)

        self.index = wrappedIndex

    def press_down(self, rwdc):
        self.select(rwdc, self.index + 1)

    def press_up(self, rwdc):
        self.select(rwdc, self.index - 1)


menu = MenuList([
    MenuItem("Orange"),
    MenuItem("Banana"),
    MenuItem("Kiwi"),
    MenuItem("Toast"),
    MenuItem("Micah"),
    ])

rwdc = rwand.RwdClient(listeners=[menu])
menu.select(rwdc, 0)
rwdc.run()
