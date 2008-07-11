"""
VectorMachine.py

Python modules for for the LaserProp's vector graphics virtual
machine.  This includes code for encoding data in the VectorMachine
format, interpreting VectorMachine programs, and uploading
VectorMachine data over a BluetoothConduit.

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


# The VectorMachine's position registers are encoded in 16.16
# fixed-point format.
FIXED_POINT_BITS = 16

# Fields in our instruction format
X_SHIFT = 0
X_MASK = (1 << 9) - 1
OFFSET_SHIFT = 0
OFFSET_MASK = (1 << 16) - 1
Y_SHIFT = 9
Y_MASK = (1 << 9) - 1
EXP_SHIFT = 18
EXP_MASK = (1 << 4) - 1
SCNT_SHIFT = 22
SCNT_MASK = (1 << 7) - 1
LE_SHIFT = 29
LE_MASK = 1
REG_SHIFT = 30
REG_MASK = (1 << 2) - 1

# Values for the REG (destination register) field
REG_SP = 0
REG_S = 1
REG_DS = 2
REG_DDS = 3

# Values for the SP (special) field
SP_NOP = 0
SP_JUMP = 1
SP_CLEAR = 2
SP_INC = 3
SP_RESET = 4

def pack(reg=REG_S, le=0, scnt=0, exp=0, y=0, x=0, offset=0):
    """Encode a tuple into an integer VectorMachine instruction word.
       """
    return (((reg    & REG_MASK)    << REG_SHIFT) |
            ((le     & LE_MASK)     << LE_SHIFT) |
            ((scnt   & SCNT_MASK)   << SCNT_SHIFT) |
            ((exp    & EXP_MASK)    << EXP_SHIFT) |
            ((y      & Y_MASK)      << Y_SHIFT) |
            ((x      & X_MASK)      << X_SHIFT) |
            ((offset & OFFSET_MASK) << OFFSET_SHIFT))

def unpack(v):
    """Unpack an integer VectorMachine instruction word into a
       (reg, le, scnt, exp, y, x, offset) tuple.
       """
    # Sign extend X and Y
    x = (v >> X_SHIFT) & X_MASK
    if x & 0x100:
        x -= 0x200
    y = (v >> Y_SHIFT) & Y_MASK
    if y & 0x100:
        y -= 0x200
    
    return ((v >> REG_SHIFT)    & REG_MASK,
            (v >> LE_SHIFT)     & LE_MASK,
            (v >> SCNT_SHIFT)   & SCNT_MASK,
            (v >> EXP_SHIFT)    & EXP_MASK,
            y, x,
            (v >> OFFSET_SHIFT) & OFFSET_MASK)


class Point:
    """Container for interpolated point data."""
    def __init__(self, x, y, laserEnable=False, isControlPoint=False):
        self.__dict__.update(locals())

    def __repr__(self):
        return "Point(%d,%d,%d)" % (self.x, self.y, self.laserEnable)


def interpret(words):
    """Interpret a subset of the VectorMachine language.  Special
       instructions (REG_SP) will raise a NotImplementedError.
       Non-special instructions will update internal state and/or
       yield Point() instances.

       Points which immediately follow one or more VectorMachine words
       will have their isControlPoint flag set.
       """
    s = (0, 0)
    ds = (0, 0)
    dds = (0, 0)

    for word in words:
        reg, le, scnt, exp, y, x, offset = unpack(word)
        p = (x << exp, y << exp)

        if reg == REG_S:
            s = p
        elif reg == REG_DS:
            ds = p
        elif reg == REG_DDS:
            dds = p
        else:
            assert reg == REG_SP
            raise NotImplementedError("VectorMachine interpreter encountered special instruction 0x%08x" % word)

        for i in xrange(scnt):
            ds = (ds[0] + dds[0], ds[1] + dds[1])
            s = (s[0] + ds[0], s[1] + ds[1])
            yield Point(s[0] >> FIXED_POINT_BITS,
                        s[1] >> FIXED_POINT_BITS,
                        le, i==0)
    

class VectorMachine:
    def __init__(self, bt, cmdRegion="vm", memRegion="vector_mem"):
        self.bt = bt
        self.cmdRegion = cmdRegion
        self.memRegion = memRegion

    def cmd(self, instr):
        self.bt.setRegionByName(self.cmdRegion)
        self.bt.write(instr)

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

        # XXX
        #for i, b in enumerate(buf):
        #    self.write(i, b)

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

if __name__== "__main__":
    print list(interpret([
                pack(reg=REG_DDS, x=10, y=10, exp=10),
                pack(x=5, y=5, exp=15, scnt=50),
                ]))
