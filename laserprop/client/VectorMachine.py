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

import struct
import math
import time
import threading


# The VectorMachine's position registers are encoded in 18.14 fixed
# point.  This is basically a tradeoff between addressable output
# resolution and curve interpolation resolution.
FIXED_POINT_BITS = 14
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

# Base hardware sampling rate
LOOP_HZ = 40000

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

    # On overflow, clamp the the highest values.
    exp = min(EXP_MASK, exp)
    xi >>= exp
    yi >>= exp
    xi = min(X_MASK/2, max(-X_MASK/2, xi))
    yi = min(Y_MASK/2, max(-Y_MASK/2, yi))

    return pack(reg, le, scnt, exp, yi, xi)
    
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

def toString(words):
    return struct.pack("<%dI" % len(words), *words)

def fromString(s):
    return struct.unpack("<%dI" % (len(s) / 4), s)

def blankFrame():
    """Create a new blank frame. An empty list won't do, because it
       would create a zero-sample loop in the VectorMachine
       interpreter. We could use a a 1-sample NOP, so that we use up
       time but do nothing else. However, the safest thing to do is to
       return to the home position and explicitly zero the
       accumulators.  This is what we do.
       """

    en = Encoder()
    en.moveTo(0,0)
    return en.inst

def lerp(x, y, a):
    """Generic linear interpolation."""
    return x + (y - x) * a

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

def countSamples(words):
    """Calculate a total sample count for all VM instructions in the supplied list"""
    scnt = 0
    for word in words:
        scnt += unpack(word)[2]
    return scnt

def timeInstructions(words):
    """Measure the amount of wallclock time, in seconds, that it will
       take to execute the specified instruction words.
       """
    return countSamples(words) / LOOP_HZ


class Point:
    """Container for interpolated point data."""
    def __init__(self, x, y, laserEnable=False, isControlPoint=False):
        self.__dict__.update(locals())

    def __repr__(self):
        return "Point(%d,%d,%d)" % (self.x, self.y, self.laserEnable)


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
                self.nop()

    def nop(self, nSamples=0):
        """Emit a no-op command. If you just need to emit samples,
           or to flush the current laser state, this command does
           not modify any registers.
           """
        self.inst.append(pack(reg=REG_SP, exp=SP_NOP, le=self.le))
        self.emit(nSamples)

    def hold(self, nSamples=0):
        """Like no-op, but explicitly zero the derivative registers.
           This guarantees that the beam will hold its current position
           for 'nSamples' samples.
           """
        self.setDDS(0, 0)
        self.setDS(0, 0)
        self.nop(nSamples)

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
        """Generate a quadratic Bezier curve from the current position
           to (x2, y2), using (x1, y1) as the control point. Emit a
           total of nSamples.

           Like lineTo(), this function may not end up exactly at the
           specified destination, due to rounding errors. Since we use
           the simulated hardware registers as our starting point,
           rounding errors do not compound.

           Quadratic Bezier curves can be interpolated entirely in
           hardware by the VectorMachine.  This is our most efficient
           primitive.
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

    def cCurveTo(self, x1, y1, x2, y2, x3, y3, nSamples=40):
        """Generate a cubic Bezier curve from the current position to
           (x3, y3), using (x1, y1) and (x2, y2) as the control
           points. Emit at least nSamples. (We may emit more due
           to rounding.)

           Like lineTo(), this function may not end up exactly at the
           specified destination, due to rounding errors. Since we use
           the simulated hardware registers as our starting point,
           rounding errors do not compound.
           """
        
        # Cubic Bezier curves cannot be interpolated entirely in
        # hardware- to recognizably reproduce the curve, we must
        # subdivide it in software first.
        #
        # For simplicity, we subdivide cubic curves into quadratic
        # curves, rather than working directly with VM registers.
        # This implementation uses the "Fixed MidPoint" approach by
        # Helen Triolo and Timothee Groleau, as described at:
        #
        # http://www.timotheegroleau.com/Flash/articles/cubic_bezier_in_flash.htm
        #
        # This awesome algorithm always generates four quadratic
        # curves for every cubic bezier, but the results are nearly
        # indistinguishable from a true Bezier curve. Nifty.

        # Use the current S register as the initial position.
        x0, y0 = self.s

        # Compute samples-per-curve, rounding up
        qSamples = int((nSamples + 3) / 4)

        # Base points
        ax = lerp(x0, x1, 0.75)
        ay = lerp(y0, y1, 0.75)
        bx = lerp(x3, x2, 0.75)
        by = lerp(y3, y2, 0.75)

        # 1/16th of the segment from control point 3 to 0
        dx = (x3 - x0) / 16.0
        dy = (y3 - y0) / 16.0

        # Control point 1
        c1x = lerp(x0, x1, 0.375)
        c1y = lerp(y0, y1, 0.375)

        # Control point 2
        c2x = lerp(ax, bx, 0.375) - dx
        c2y = lerp(ay, by, 0.375) - dy
                 
        # Control point 3
        c3x = lerp(bx, ax, 0.375) + dx
        c3y = lerp(by, ay, 0.375) + dy

        # Control point 1
        c4x = lerp(x3, x2, 0.375)
        c4y = lerp(y3, y2, 0.375)

        # Anchor point 1
        a1x = lerp(c1x, c2x, 0.5)
        a1y = lerp(c1y, c2y, 0.5)

        # Anchor point 2
        a2x = lerp(ax, bx, 0.5)
        a2y = lerp(ay, by, 0.5)

        # Anchor point 3
        a3x = lerp(c3x, c4x, 0.5)
        a3y = lerp(c3y, c4y, 0.5)
        
        # Output four curves
        self.qCurveTo(c1x, c1y, a1x, a1y, qSamples)
        self.qCurveTo(c2x, c2y, a2x, a2y, qSamples)
        self.qCurveTo(c3x, c3y, a3x, a3y, qSamples)
        self.qCurveTo(c4x, c4y, x3,  y3,  qSamples)
        

class VMConduit:
    """This is a low-level interface to the VectorMachine hardware,
       via a BluetoothConduit. We can write to vector memory, inject
       commands, and perform other relatively low-level VM operations.
       """
    def __init__(self, bt, cmdRegion="vm", memRegion="vector_mem"):
        self.bt = bt
        self.cmdRegion = cmdRegion
        self.memRegion = memRegion

    def cmd(self, word):
        self.bt.write(self.cmdRegion, 0, toString((word,)))

    def write(self, addr, words):
        self.bt.write(self.memRegion, addr, toString(words))

    def read(self, addr, count, cb):
        self.bt.read(self.memRegion, addr, count, lambda s: cb(fromString(s)))

    def jump(self, offset):
        """Issue a jump instruction to the provided address."""
        self.cmd(pack(reg=REG_SP, exp=SP_JUMP, offset=offset))

    def stop(self):
        """Turn the laser off, and lock the instruction pointer
           at offset zero in a one-cycle infinite loop.
           """
        self.write(0, (pack(reg=REG_SP, exp=SP_JUMP, scnt=1),))
        self.cmd(pack(reg=REG_SP, exp=SP_RESET))

    def start(self):
        """Write a no-op instruction into offset zero, and continue
           execution there.
           """
        self.stop()
        self.write(0, (pack(reg=REG_SP, exp=SP_NOP), ))

    def getMemSize(self):
        """Return the size of the vector memory region, in longs"""
        return self.bt.findRegion(self.memRegion).size


class RemoteFrameQueue:
    """Interface to a remote queue of frames, implemented as a
       ring buffer of linked loops within the VM's memory space.

       This object should only live as long as the device connection.
       If we reconnect, this object should be recreated.
       
       Frames can be added at any time, provided there is available
       space. Each time we asynchronously notice that a frame has
       completed, we free its space and call the provided completion
       callback. The completion callback's ragument is the new front
       frame.

       The queue has a bounded maximum latency. If the
       not-yet-acknowledged frames will take more than maxLatency
       seconds to execute, we don't allow new frames to be enqueued.
       """
    def __init__(self, vm, completionCb, maxLatency=0.25):
        self.vm = vm
        self.completionCb = completionCb
        self.maxLatency = maxLatency

        self._lock = threading.Lock()
        self._lock.acquire()

        try:
            # Partition the available VM memory: leave
            # space at the top for our swap counter,
            # use the rest as ring buffer space.

            memSize = self.vm.getMemSize()
            self.ringSize = memSize - 1
            self.swapCountAddr = self.ringSize

            # Stick the VM in an infinite loop at address zero
            self.vm.stop()

            # Reset the swap counter, and start polling it
            self.swapCount = 0
            self.vm.write(self.swapCountAddr, (self.swapCount,))
            self.vm.read(self.swapCountAddr, 1, self._pollSwap)

            # The ring buffere itself is modelled as a list of RemoteFrame
            # objects.  The first one in the list is the oldest buffer, or
            # our "front" buffer.  The last one is the newest. The frames
            # may wrap, so addresses will not be strictly increasing.
            #
            # To initialize the ring, we'll add a blank frame just after
            # the infinite loop, then we'll jump to it. We don't want this
            # to count as a buffer swap, so jump directly to its frame data.

            nullFrame = RemoteFrame(blankFrame(), 1, self.swapCountAddr)
            self.queue = [ nullFrame ]
            self.queueDuration = nullFrame.duration
            nullFrame.write(self.vm)
            self.vm.jump(nullFrame.frameDataAddr)

        finally:
            self._lock.release()

    def _pollSwap(self, (swapCount,)):
        ackFrames = []

        try:
            self._lock.acquire()

            while swapCount != self.swapCount:
                # For every swap that the hardware logged, increment
                # our own simulated swapCount and retire the oldest
                # frame in the queue. Remember that frame so we can
                # pass it to completionCb, after we release our lock.

                self.swapCount = (self.swapCount + 1) & 0xFFFFFFFF

                assert len(self.queue) > 1
                self.queueDuration -= self.queue[0].duration
                del self.queue[0]

                # Ack the new front buffer
                ackFrames.append(self.queue[0])


            # Keep polling...
            self.vm.read(self.swapCountAddr, 1, self._pollSwap)

        finally:
            self._lock.release()

        for frame in ackFrames:
            self.completionCb(frame)

    def append(self, instructions):
        """Try to append the provided frame. If we are successful,
           returns True and asynchronously complete this frame
           via completionCb. If there is no room in the hardware
           buffer, returns False.
           """
        try:
            self._lock.acquire()

            # We can reject a frame because it exceeds our latency bounds
            if self.queueDuration >= self.maxLatency:
                return False

            # Start out by placing it right after the last frame in the
            # queue. Does that work? If it overlaps the front buffer,
            # the buffer is full and we have to fail. If it overlaps the
            # swap counter (at the end of memory), we may be able to
            # wrap.

            assert len(self.queue) > 0
            rf = RemoteFrame(instructions,
                             self.queue[-1].nextFreeAddr,
                             self.swapCountAddr)
            head = self.queue[0]

            if (rf.firstAddress < head.firstAddress and 
                rf.nextFreeAddr > head.firstAddress):

                # Memory is full, this overlaps the front buffer.
                return False

            if rf.nextFreeAddr > self.swapCountAddr:
                # Overlaps the swap counter. Can we wrap?

                rf = RemoteFrame(instructions, 0, self.swapCountAddr)

                if rf.nextFreeAddr > self.queue[0].firstAddress:
                    # No room to wrap.
                    return False

            # This is a good address. Add it to our queue, send the frame
            # data, and link the previous frame to this one.

            prev = self.queue[-1]
            self.queue.append(rf)
            self.queueDuration += rf.duration

            rf.write(self.vm)
            prev.linkTo(self.vm, rf)

            return True
        finally:
            self._lock.release()


class RemoteFrame:
    """One frame in a RemoteFrameQueue. This holds the original frame
       data, as a list of instructions, plus it's responsible for
       remembering ring buffer addresses and managing the linkage
       between frames.

       To know when a swap has completely finished, we need to
       increment the swap counter at the beginning of the next frame,
       rather than at the end (before jumping) of the original frame.
       
       Thus, them emory layout of one frame:
          1. Increment the swap counter.
          2. Variable-length frame data.
          3. Jump back to (3)
       
       To execute a swap, we write over the instruction in (3),
       and point it at (1) in the next frame.
       """
    def __init__(self, instructions, firstAddress, swapCountAddr):
        self.instructions = instructions
        self.swapCountAddr = swapCountAddr
        self.duration = timeInstructions(instructions)

        # Calculate addresses
        self.firstAddress = firstAddress
        self.entryPoint = firstAddress
        self.frameDataAddr = self.entryPoint + 1
        self.exitPoint = self.frameDataAddr + len(instructions)
        self.nextFreeAddr = self.exitPoint + 1

    def write(self, vm):
        """Write this frame at the chosen address,"""
        vm.write(self.entryPoint,
                 [ pack(reg=REG_SP, exp=SP_INC, offset=self.swapCountAddr) ] +
                 self.instructions +
                 [ pack(reg=REG_SP, exp=SP_JUMP, offset=self.frameDataAddr) ])
        
    def linkTo(self, vm, next):
        """When this frame finishes executing, have it jump to the next one instead of looping."""
        vm.write(self.exitPoint,
                 (pack(reg=REG_SP, exp=SP_JUMP, offset=next.entryPoint),))


class FrameBuffer:
    """Literally, a buffer for frames, in which each frame is a
       sequence of VectorMachine instruction words.

       The FrameBuffer is divided into two queues:

         - The remote queue. This holds frames that we've already
           uploaded to the device. This queue is advanced in hardware.
           If this queue ever has only a single frame in it, that
           frame will be looped. The oldest frame in the remote
           queue is our 'front buffer'.

         - The local queue. These are frames which are waiting behind
           the hardware queue. New frames always end up here
           first. When adding new frames, you may replace the queue or
           append to it. Replacing is good for interactive
           manipulation, where you need to display the newest state
           immediately, appending is good for animations where you
           want to display many frames at a constant rate.
       
       The front buffer, representing our (delayed) notion of what's
       currently on-screen, is accessable via the 'front' member, and
       it is observable.
       """

    def __init__(self, bt):
        self.vm = VMConduit(bt)
        self._frontObservers = []
        self._lock = threading.Lock()

        # Front buffer starts out blank.
        self.front = blankFrame()

        # Local queue
        self.queue = []

        bt.addCallbacks(self._btConnect, self._btDisconnect)

    def observeFront(self, cb):
        """Observe changes to the front buffer."""
        self._frontObservers.append(cb)
        cb(self.front)

    def _btConnect(self):
        """Reset the VectorMachine, and re-upload the current frame."""
        self._connected = True
        self._remoteQueue = RemoteFrameQueue(self.vm, self._frameCompleted)

    def _btDisconnect(self):
        self._remoteQueue = None
        self._connected = False
            
    def _frameCompleted(self, frame):
        # The RemoteFrameQueue has completed a swap. It's giving
        # us the new front buffer.
        self._pumpQueue()
        self.front = frame.instructions
        for cb in self._frontObservers:
            cb(self.front)

    def _pumpQueue(self):
        # Move as many items as possible from the local queue to the remote queue.
        try:
            self._lock.acquire()

            while self.queue:
                next = self.queue[0]
                if self._remoteQueue and self._remoteQueue.append(next):
                    del self.queue[0]
                else:
                    break

        finally:
            self._lock.release()

    def append(self, frame):
        """Append a new frame to the end of the queue."""
        self.queue.append(frame)
        self._pumpQueue()

    def replace(self, frame):
        """Replace the entire queue of pending frames with this one frame."""
        self.queue[:] = [frame]
        self._pumpQueue()
