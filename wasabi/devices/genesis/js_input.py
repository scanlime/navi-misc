#!/usr/bin/env python
import evdev, sys

ev = open("/dev/input/event0")
dev = evdev.Device()

def mapAxis(v, threshold=50):
    return v < -threshold, v > threshold

def packBits(*bits):
    v = 0
    i = 1
    for bit in bits:
        if bit:
            v |= i
        i <<= 1
    return v

while 1:
    dev.update(evdev.Event(readFrom=ev))

    up, down = mapAxis(dev['ABS_Y'])
    left, right = mapAxis(dev['ABS_X'])
    a = dev['BTN_B']
    b = dev['BTN_A']
    c = dev['BTN_C']
    start = dev['BTN_X']

    wvalue = 0
    windex = 2

    if dev['BTN_TL2']:
        # Controller 2
        wvalue |= packBits(up, down, left, right, a, b, c, start) << 8
    else:
        # Controller 1
        wvalue |= packBits(up, down, left, right, a, b, c, start)

    # Reset button
    if dev['BTN_TR2']:
        windex |= 1

    print "%d %d" % (wvalue, windex)
    sys.stdout.flush()

### The End ###
