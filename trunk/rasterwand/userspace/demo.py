#!/usr/bin/env python
import rasterwand, xmlrpclib, time

s = xmlrpclib.ServerProxy("http://navi.picogui.org:4510")
dev = rasterwand.Device()

dev.params.display_width = 0xFFFF * 0.75
dev.params.duty_cycle = 0xFFFF * 0.5

while 1:
    # Show the time
    dev.vScrollText(time.strftime("%l:%M %p"), width=60, pause=3)

    # Scroll all temperatures continuously
    desc = s.getDescriptions()
    avg = s.getAverages()
    text = (" "*10).join(["%s: %0.1f \xb0F" % (name, avg[id]) for id, name in desc.iteritems()])
    rasterwand.TextScroller(dev, text, width=70).scroll(start=(1, 0), end=(-1, 0), speed=1)

