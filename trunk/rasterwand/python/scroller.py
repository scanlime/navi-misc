#!/usr/bin/env python
#
# Scrolls any lines received over stdin
#
import rasterwand, sys

dev = rasterwand.Device()

dev.params.display_width = 0xFFFF * 0.6
dev.params.duty_cycle = 0xFFFF * 0.2

while 1:
    line = sys.stdin.readline()
    if not line:
        break
    rasterwand.TextScroller(dev, line.strip(), width=40).scroll(start=(1, 0), end=(-1, 0), speed=1)

