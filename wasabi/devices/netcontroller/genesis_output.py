#!/usr/bin/env python
import pyrcpod, sys

class GenesisJoystick:
    def __init__(self, rcpod=None):
        if not rcpod:
            rcpod = pyrcpod.devices[0].open()
        self.rcpod = rcpod

        print rcpod
        # Skip pin descriptors, so we can set all button
        # states with a 1-byte poke rather than at least
        # two 4-pin assertion packets.
        self.rcpod.poke('PORTB', 0xFF)
        self.rcpod.poke('TRISB', 0x70)

    def set(self, up=0, down=0, left=0, right=0, button=0):
        state = 0xFF
        if up:
            state &= ~0x01
        if down:
            state &= ~0x02
        if left:
            state &= ~0x04
        if right:
            state &= ~0x08
        if button:
            state &= ~0x80
        self.rcpod.poke('PORTB', state)


def main():
    joy = GenesisJoystick()
    for line in sys.stdin:
        joy.set(*map(int, line.strip().split()))

if __name__ == "__main__":
    main()

### The End ###
