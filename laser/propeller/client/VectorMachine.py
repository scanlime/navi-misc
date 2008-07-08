"""
VectorMachine.py

Python client for the LaserProp's vector graphics virtual machine.
Copyright (c) 2008 Micah Dowty

   Permission is hereby granted, free of charge, to any person
   obtaining a copy of this software and associated documentation
   files (the "Software"), to deal in the Software without
   restriction, including without limitation the rights to use, copy,
   modify, merge, publish, distribute, sublicense, and/or sell copies
   of the Software, and to permit persons to whom the Software is
   furnished to do so, subject to the following conditions:

   The above copyright notice and this permission notice shall be
   included in all copies or substantial portions of the Software.

   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
   EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
   MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
   NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
   BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
   ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
   CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
   SOFTWARE.

"""

from __future__ import division

import struct, math

# XXX
__all__ = ['VectorMachine', 'iterPathSampler', 'vecSub']


class VectorMachine:
    REG_SP = 0
    REG_S = 1
    REG_DS = 2
    REG_DDS = 3

    SP_NOP = 0
    SP_JUMP = 1
    SP_CLEAR = 2
    SP_INC = 3
    SP_RESET = 4

    FIXED_POINT_BITS = 16

    def __init__(self, bt, cmdRegion="vm", memRegion="vector_mem"):
        self.bt = bt
        self.cmdRegion = cmdRegion
        self.memRegion = memRegion

    def cmd(self, instr):
        self.bt.setRegionByName(self.cmdRegion)
        self.bt.write(instr)

    def pack(self, reg=REG_S, le=0, scnt=0, exp=0, y=0, x=0, offset=0):
        x &= 0x1FF
        y &= 0x1FF
        return struct.pack("<I", (reg << 30) | (le << 29) |
                           (scnt << 22) | (exp << 18) | (y << 9) | x | offset)

    def write(self, addr, data):
        self.bt.setRegionByName(self.memRegion)
        self.bt.seek(addr)
        self.bt.write(data)

    def jump(self, offset):
        """Issue a jump instruction to the provided address."""
        self.cmd(self.pack(reg=self.REG_SP, exp=self.SP_JUMP,
                           offset=offset))

    def stop(self):
        """Turn the laser off, and lock the instruction pointer
           at offset zero in an infinite loop.
           """
        self.cmd(self.pack(reg=self.REG_SP, exp=self.SP_RESET))

    def start(self):
        """Write a no-op instruction into offset zero, and continue
           execution there.
           """
        self.stop()
        self.write(0, self.pack(reg=self.REG_SP, exp=self.SP_NOP))

    def uploadVectors(self, vectors, scnt, cb):
        self.stop()
        buf = []

        for x, y, le in vectors:
            # Break up large sample counts into multiple instructions
            s = scnt
            while s > 0:
                iscnt = min(127, s)
                s -= iscnt            
        
                buf.append(self.pack(le=le, scnt=iscnt, exp=15,
                                     x=int(x), y=int(y)))

        # Loop
        # Note the nonzero scnt: we need this loop to generate samples,
        # otherwise the VM's timer will overflow if we send it an empty image.

        buf.append(self.pack(reg=self.REG_SP, scnt=1, exp=self.SP_JUMP, offset=1))

        print "Pattern length: %d" % len(buf)
        self.write(1, ''.join(buf))
        self.start()

        def finish(_):
            print "Finished writing pattern"
            cb()
        self.bt.read(1, finish)


def vecSub(a, b):
    return (a[0] - b[0], a[1] - b[1])

def vecAdd(a, b):
    return (a[0] + b[0], a[1] + b[1])

def vecMag(a):
    return math.sqrt(a[0] ** 2 + a[1] ** 2)

def vecMul(a, b):
    return (a[0] * b, a[1] * b)

def vecNorm(a):
    return vecMul(a, 1.0 / vecMag(a))

def vecDot(a, b):
    return a[0] * b[0] + a[1] * b[1]

def iterPathSampler(points, dist):
    """Given one line segment path, resample that path and yield
       new points every 'dist' units. Each point is an (x, y, payload)
       tuple, where 'payload' is an arbitrary Python object which is
       preserved in the resampled output. Each output sample carries
       the payload of the preceeding input sample.
       """
    prev = None

    # X is an accumulator that increments every time we pass a
    # pixel on the input line. When it crosses 'dist', we generate
    # an output sample and reset it to zero.
    x = 0

    for cur in points:
        if prev is None:
            prev = cur
            yield prev
            continue

        v = vecSub(cur[:2], prev[:2])
        vmag = vecMag(v)

        # Walk from the previous input sample to the current one,
        # emitting output samples as we go.

        remaining = vmag

        while True:
            travel = dist - x
            if remaining < travel:
                break

            prev = vecAdd(prev[:2], vecMul(v, travel / vmag)) + prev[2:]
            x = 0
            remaining -= travel
            yield prev

        prev = cur
        x += remaining
