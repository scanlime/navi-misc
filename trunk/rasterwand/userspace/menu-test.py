#!/usr/bin/env python

import time, feedparser, rwand, netif

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


class HeadlineMenu(rwand.AutoMenuList):
    def __init__(self, feed):
        self.feed = feed
        rwand.AutoMenuList.__init__(self, [], root=True)

    def update(self):
        d = feedparser.parse(self.feed)
        self.items = [
            rwand.MenuItem(ClockRenderer()),
            rwand.TextMenuItem('%s:' % d.feed.title),
        ]+[
            rwand.TextMenuItem(entry.title)
            for entry in d.entries
        ]

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


if __name__ == "__main__":
    rwdc = rwand.RwdClient()
    headlines = HeadlineMenu('http://www.theonion.com/content/feeds/daily')
    rwdc.start()
    try:
        headlines.update()
        headlines.activate(rwdc)
        while True:        
            time.sleep(60 * 30)
            headlines.update()

    except KeyboardInterrupt:
        pass
    finally:
        rwdc.stop()
        rwdc.join()    
