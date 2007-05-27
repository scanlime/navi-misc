#!/usr/bin/env python

import time, math
import rwand, netif

class NetworkInterfaceMenu(rwand.MenuList):
    def __init__(self):
        rwand.MenuList.__init__(self, None)

    def activate(self, rwdc):
        interfaces = netif.all_interfaces()
        interfaces.sort()
        self.items = []
        for intf in interfaces:
            self.items.append(rwand.SubMenuItem(intf, rwand.MenuList([
                rwand.TextMenuItem(netif.get_ip_address(intf)),
                ])))
        rwand.MenuList.activate(self, rwdc)


class ClockRenderer(rwand.Renderer):
    def render(self, rwdc):
        text = u"\u231A " + time.strftime("%l:%M %p").strip()
        return rwand.TextRenderer(text).render(rwdc)


class GraphRenderer(rwand.Renderer):
    def __init__(self):
        self.bars = [0] * 20
        self.theta = 0

    def render(self, rwdc):
        for i in range(len(self.bars)):
            self.bars[i] *= 0.95

        self.bars[int((math.sin(self.theta) + 1) * len(self.bars) / 2)] = 1
        self.theta += 0.05

        return ''.join([chr(0xFF & (0xFF00 >> int(bar * 8 + 0.5)))
                        for bar in self.bars])


class InfoMenu(rwand.AutoMenuList):
    def __init__(self):
        rwand.AutoMenuList.__init__(self, [
            rwand.TextMenuItem("orange"),
            rwand.TextMenuItem("banana"),
            rwand.MenuItem(ClockRenderer()),
            rwand.MenuItem(GraphRenderer()),
            ], root=True)

    def press_select(self, rwdc):
        SetupMenu().activate(rwdc)


class SetupMenu(rwand.MenuList):
    def __init__(self, inactivityDelay=10):
        rwand.MenuList.__init__(self, [
            rwand.TextMenuItem(u"Setup \u2191\u2193"),
            rwand.SubMenuItem("Display", rwand.SettingsMenu()),
            rwand.SubMenuItem("Network", NetworkInterfaceMenu()),
            ])
        self.inactivityDelay = inactivityDelay

    def pollKeys(self, rwdc, buttons):
        if rwdc.getInactivityTime() > self.inactivityDelay:
            InfoMenu().activate(rwdc)


rwdc = rwand.RwdClient()
InfoMenu().activate(rwdc)
rwdc.run()
