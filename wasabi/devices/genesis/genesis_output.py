#!/usr/bin/env python
import pyrcpod, sys

class GenesisJoystick:
    def __init__(self, rcpod=None):
        if not rcpod:
            rcpod = pyrcpod.devices[0].open()
        self.rcpod = rcpod

        # Skip pin descriptors, so we can set all button
        # states with a 1-byte poke rather than at least
        # two 4-pin assertion packets. All outputs are
	# open-drain.
	self.rcpod.poke('PORTB', 0x00)
        self.rcpod.poke('TRISB', 0xFF)

    def set(self, up=0, down=0, left=0, right=0,
     	    start=0, a=0, b=0, c=0):
        state = 0xFF
        if up:
            state &= ~0x01
        if down:
            state &= ~0x02
        if left:
            state &= ~0x04
        if right:
            state &= ~0x08
        if start:
            state &= ~0x10
	if a:
	    state &= ~0x20
	if b:
	    state &= ~0x40
	if c:
	    state &= ~0x80
        self.rcpod.poke('TRISB', state)


def main():
    joy = GenesisJoystick()
    while True:
        try:
	    joy.set(*map(int, raw_input().strip().split()))
	except EOFError:
	    break

if __name__ == "__main__":
    main()

### The End ###
