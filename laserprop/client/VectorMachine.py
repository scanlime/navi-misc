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


# The VectorMachine's position registers are encoded in 16.16 fixed point.
FIXED_POINT_BITS = 16
FIXED_POINT_SCALE = 1 << FIXED_POINT_BITS

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

def packFloat(reg=REG_S, le=0, scnt=0, y=0, x=0):
    """Encode floating point x and y values by converting to fixed-point,
       and automatically splitting them into a 9-bit mantissa and 4-bit EXP.
       """
    xi = int(FIXED_POINT_SCALE * x + 0.5)
    yi = int(FIXED_POINT_SCALE * y + 0.5)
    absMax = 0xFF
    exp = 0
    
    while abs(xi >> exp) > absMax or abs(yi >> exp) > absMax:
        exp += 1

    if exp > EXP_MASK:
        raise ValueError("Overflow in packFloat(x=%s, y=%s)" % (x,y))

    print (xi>>exp, yi>>exp, exp)
    return pack(reg, le, scnt, exp, yi >> exp, xi >> exp)
    

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


def unpackFloat(v):
    """Unpack the coordinate in an instruction
       to a floating point (x, y) tuple.
       """
    reg, le, scnt, exp, y, x, offset = unpack(v)
    
    return ((x << exp) / float(FIXED_POINT_SCALE),
            (y << exp) / float(FIXED_POINT_SCALE))


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
            if exp != SP_NOP:
                raise NotImplementedError("VectorMachine interpreter encountered unhandled instruction 0x%08x" % word)

        for i in xrange(scnt):
            ds = (ds[0] + dds[0], ds[1] + dds[1])
            s = (s[0] + ds[0], s[1] + ds[1])
            yield Point(s[0] >> FIXED_POINT_BITS,
                        s[1] >> FIXED_POINT_BITS,
                        le, i==0)


class Encoder:
    """Convert normal vector graphics commands like 'move to',
       'line to', etc. into VectorMachine instructions.

       Instructions are appended to the 'inst' list. You may
       optionally specify the initial state of the machine. If not
       specified, we will assume the state is unknown.
       """
    def __init__(self, inst=None, s=None, ds=None, dds=None, le=False):
        self.inst = inst or []
        self.s = s
        self.ds = ds
        self.dds = dds
        self.le = le

    def setLaser(self, le):
        """Set the laser enable flag. The next instruction we emit
           will carry this new enable flag value.
           """
        self.le = le

    def setS(self, x, y):
        """Set the S (position) register, if necessary."""
        self._setReg('s', REG_S, x, y)

    def setDS(self, x, y):
        """Set the S (derivative) register, if necessary."""
        self._setReg('ds', REG_DS, x, y)

    def setDDS(self, x, y):
        """Set the DDS (second derivative) register, if necessary."""
        self._setReg('dds', REG_DDS, x, y)

    def _setReg(self, attr, reg, x, y):
        """Generic private function for setting a register.
           This has some important properties:

             - The local copy of that register is set according
               to the packed and then unpacked machine representation
               of the new vector. This means we preserve any rounding
               errors we incurred, so that they can be accounted
               for by our register simulation.

             - This function has no effect if the register already
               equals the requested value. We can't do an exact
               equality comparison, but we can encode both the
               old and the proposed new value, and test whether
               their machine encoding matches.
           """
              
        old = getattr(self, attr)
        if old is None or (packFloat(x=x, y=y) != packFloat(x=old[0], y=old[1])):
            i = packFloat(reg = reg,
                          le = self.le,
                          x = x,
                          y = y)
            setattr(self, attr, unpackFloat(i))
            self.inst.append(i)

    def emit(self, nSamples=1):
        """Emit the specified number of samples.  Normally this does
           not create a new instruction, it just adds to the SCNT of
           the last instruction in the list.
           """

        # Update the DS and S registers to account for the passage of time...
        for i in xrange(nSamples):
            self.ds = (self.ds[0] + self.dds[0], self.ds[1] + self.dds[1])
            self.s = (self.s[0] + self.ds[0], self.s[1] + self.ds[1])

        while nSamples > 0:
            reg, le, scnt, exp, y, x, offset = unpack(self.inst[-1])

            scnt += nSamples
            nSamples = max(0, scnt - SCNT_MASK)
            scnt = min(SCNT_MASK, scnt)

            self.inst[-1] = pack(reg, le, scnt, exp, y, x)

            if nSamples > 0:
                # Emit a no-op
                self.inst.append(pack(reg=REG_SP, exp=SP_NOP))

    def moveTo(self, x, y, nSamples=1):
        """Move to the specified point, and emit 'nSamples' samples
           at that point. There are two main ways to implement this
           in the VectorMachine:

            a) Zero all derivative registers, then set the S register
               to (x, y) and emit a sample.

            b) Leave the derivative registers alone, calculate how much
               they will modify S by on the next sample, then use that
               value to compensate when writing to S.

           Currently we use approach (a), but it may be worth optimizing
           using approach (b) when nSamples==1 later.              
           """
        self.setDDS(0, 0)
        self.setDS(0, 0)
        self.setS(x, y)
        self.emit(nSamples)

    def lineTo(self, x, y, nSamples=1):
        """Create a line from the current point to the specified point,
           and emit a total of 'nSamples' over the length of the line.

           Due to rounding errors, we might not end up exactly at the
           specified point. The actual point we end up at will be in
           the S register on return.

           Since the actual S register is used in calculating line
           source positions, error in consecutive lineTo commands
           will not accumulate.
           """
        a = 1.0 / nSamples
        self.setDDS(0, 0)
        self.setDS(a * (x - self.s[0]),
                   a * (y - self.s[1]))
        self.emit(nSamples)

    def qCurveTo(self, x1, y1, x2, y2, nSamples=10):
        """Generate a quadratic Bezier curve from
           the current position to (x2, y2), using
           (x1, y1) as the control point. Emit a total
           of nSamples.

           Like lineTo(), this function may not end
           up exactly at the specified destination,
           due to rounding errors. Since we use the
           simulated hardware registers as our starting
           point, rounding errors do not compound.

           Quadratic Bezier curves can be interpolated
           entirely in hardware by the VectorMachine.
           This is our most efficient primitive.
           """

        # First, some math...
        #
        # References:
        #   http://mathworld.wolfram.com/BezierCurve.html
        #   http://mathworld.wolfram.com/BernsteinPolynomial.html
        #
        # Quadratic Bezier curves use the 2nd degree Bernstein
        # polynomials as their basis functions:
        #
        #   B(0,2) = (1 - t)^2
        #   B(1,2) = 2 (1 - t) t
        #   B(2,2) = t^2
        #
        # We can compute the first and second derivatives, and
        # note that the second derivative is constant across
        # the entire curve:
        #
        #   B(0,2)' = -2 + 2t
        #   B(1,2)' = 2 - 4t
        #   B(2,2)' = 2t
        #
        #   B(0,2)'' = 2
        #   B(1,2)'' = -4
        #   B(2,2)'' = 2
        #
        # This constant second derivative means that the hardware
        # accumulators can interpolate the entire curve without any
        # approximation other than the usual fixed point and
        # instruction encoding errors.
        #
        # To program the VectorMachine, we just need to know the
        # initial value of the first derivative (at t=0), and the
        # constant value of the second derivative for this curve.

        # Use the current S register as the initial position.
        x0, y0 = self.s

        # Compute derivatives for this curve at t=0
        dx = -2 * x0 + 2 * x1
        dy = -2 * y0 + 2 * y1

        # Compute second derivatives
        ddx = 2 * x0 - 4 * x1 + 2 * x2
        ddy = 2 * y0 - 4 * y1 + 2 * y2
        
        # Step factors for each derivative
        ds = 1.0 / nSamples
        dds = ds / nSamples

        self.setDDS(ddx * dds, ddy * dds)
        self.setDS(dx * ds, dy * ds)
        self.emit(nSamples)


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
