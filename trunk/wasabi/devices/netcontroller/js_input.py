#!/usr/bin/env python
import evdev, sys

ev = open("/dev/input/event0")
dev = evdev.Device()

def mapAxis(v, threshold=50):
    return v < -threshold, v > threshold

while 1:
    dev.update(evdev.Event(readFrom=ev))

    up, down = mapAxis(dev.axes.get('ABS_Y', 0))
    left, right = mapAxis(dev.axes.get('ABS_X', 0))

    print "%d %d %d %d %d %d %d %d" % (
        up, down, left, right,
        dev.buttons.get('BTN_X', 0),
        dev.buttons.get('BTN_B', 0),
        dev.buttons.get('BTN_A', 0),
        dev.buttons.get('BTN_C', 0))
    sys.stdout.flush()

### The End ###
