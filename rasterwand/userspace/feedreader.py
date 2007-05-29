#!/usr/bin/env python
#
# RSS reader for the Raster Wand, with configuration and network info
# menus, as well as a clock.
#
# -- Micah Dowty <micah@navi.cx>
#

import sys, time, feedparser, rwand, netif


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
        d = feedparser.parse(self.feed)
        rwand.AutoMenuList.__init__(self, [
            rwand.MenuItem(ClockRenderer()),
            rwand.TextMenuItem('%s:' % d.feed.title),
            ]+[
            rwand.TextMenuItem(entry.title)
            for entry in d.entries
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


if __name__ == "__main__":
    if len(sys.argv) == 3:
        url = sys.argv[1]
        interval = 60 * int(sys.argv[2])
    else:
        sys.stderr.write("usage: %s <URL> <Update interval (minutes)>\n" % sys.argv[0])
        sys.exit(1)
    
    rwdc = rwand.RwdClient()
    rwdc.start()
    try:
        while True:        
            headlines = HeadlineMenu(url)
            headlines.activate(rwdc)
            time.sleep(interval)

    except KeyboardInterrupt:
        pass
    finally:
        rwdc.stop()
        rwdc.join()    
