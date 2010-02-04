"""
BluetoothConduit.py

A Python client for the BluetoothConduit.spin object.
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

try:
    # Use PyBlueZ if we can (Linux/Windows). Otherwise (Mac OS) use a raw device node
    # that the user must have already mapped to the right RFCOMM device.
    import bluetooth
except ImportError:
    bluetooth = None

import time
import threading
import Queue
import os
import struct

__all__ = ['BluetoothConduit']

# Bluetooth device
BD_ADDR    = '03:1F:08:07:15:2E'
BD_DEVNODE = '/dev/tty.TestDevice-SerialPort'

# Hardware constants
MAX_REGIONS = 32
MAX_DESCRIPTOR_LEN = 64
MAX_PENDING_READS = 127
MAX_COMMAND_LENGTH = 255 * 4 * 2

# Max read/write size.
MAX_RW_SIZE = 255

# Opcodes
OP_NOP         = '\x00'
OP_SET_REGION  = '\xF0'
OP_READ_LONGS  = '\xE1'
OP_WRITE_LONGS = '\xD2'
OP_SEEK8       = '\xC3'
OP_SEEK16      = '\xB4'

# XXX: I don't know where the problem is yet, but someone is losing
#      data if we send too much at once. Limit the max data size for
#      one send().
MAX_SEND_SIZE  = 64

# Command assembly functions

def opSetRegion(region):
    return OP_SET_REGION + chr(region)

def opReadLongs(count):
    if count > MAX_RW_SIZE:
        return (opReadLongs(MAX_RW_SIZE) +
                opReadLongs(count - MAX_RW_SIZE))
    else:
        return OP_READ_LONGS + chr(count)

def opWriteLongs(data):
    assert (len(data) % 4) == 0
    words = len(data) / 4

    if words > MAX_RW_SIZE:
        return (opWriteLongs(data[:4*MAX_RW_SIZE]) +
                opWriteLongs(data[4*MAX_RW_SIZE:]))
    else:
        return OP_WRITE_LONGS + chr(words) + data

def opSeek(offset):
    if offset == 0:
        return ''
    elif offset < 128 and offset > -128:
        return OP_SEEK8 + struct.pack("b", offset)
    else:
        return OP_SEEK16 + struct.pack("<h", offset)

def debug(s):
    print "[BT] %s" % s


class Region:
    def __init__(self, index, size, name):
        self.index = index
        self.size = size
        self.name = name

    def __repr__(self):
        return "<Region #%d %r, %d words>" % (self.index, self.name, self.size)


def drainQueue(q):
    """Drain items from a queue. If any items are
       available, returns a list of items. If no items
       are avilable, blocks until some are.

       If we encounter a None in the queue, it is special:
       it is not added to the list, instead we immediately
       return with all items read so far.
       """
    buf = []
    while True:
        # Get as much as possible without blocking
        try:
            while True:
                item = q.get_nowait()
                if item is None:
                    return buf
                else:
                    buf.append(item)
        except Queue.Empty:
            pass

        if buf:
            return buf

        # Nothing in the queue. Block for
        # one item, then go back and get any
        # that we can without blocking.
        item = q.get()
        if item is None:
            return buf
        else:
            buf.append(item)


class SendThread(threading.Thread):
    """Send thread: Just write strings to the device.
       """
    def __init__(self, sendFn):
        self.running = True
        self.queue = Queue.Queue()
        self.sendFn = sendFn
        threading.Thread.__init__(self)

    def run(self):
        while self.running:
            self.sendall(''.join(drainQueue(self.queue)))

    def stop(self):
        self.running = False
        self.queue.put(None)
        self.join()

    def sendall(self, data):
        """Our owm implementation of sendall(), since PyBlueZ
           doesn't have it on Windows hosts.
           """
        print "TX: %r" % data
        while data and self.running:
            ret = self.sendFn(data[:MAX_SEND_SIZE])
            assert ret > 0
            data = data[ret:]


class RecvThread(threading.Thread):
    """Recv thread: Read data from the device, and process
       queued callbacks.
       """
    def __init__(self, recvFn):
        self.running = True
        self.discardData = True
        self.queue = Queue.Queue(MAX_PENDING_READS - 1)
        self.recvFn = recvFn
        threading.Thread.__init__(self)

    def run(self):
        buf = ''

        while self.running:
            if self.discardData:
                # Receive and discard (all but the last iteration,
                # since that will be the first byte received after
                # discardData was cleared)
                buf = self.recvFn(1)
                continue

            item = self.queue.get()
            if item is None:
                continue

            # Receive data for this callback
            size, cb = item
            size *= 4

            # Keep receiving until we get enough data
            while len(buf) < size:
                if not self.running:
                    return
                buf += self.recvFn(size - len(buf))
            print "RX: %r" % buf

            assert len(buf) == size
            cb(buf)
            buf = ''

            self.queue.task_done()

    def stop(self):
        self.running = False
        self.queue.put(None)
        self.join()


class BluetoothConduit:
    """Low-level full duplex communication with shared memory objects
       in our hardware, over a Bluetooth link.

       This object provides a file-like interface for reading and
       writing memory regions. Regions are named. We can seek within
       regions, and asynchronously read or write words. Each 32-bit
       word we read or write is atomic.

       Writes happen asynchronously, with no completion notification.
       Reads happen asynchronously, and data is provided to a callback
       on a separate thread.
       """

    sock = None
    isConnected = False
    _regionCache = None
    _regionsByName = {}

    def __init__(self):
        self.onConnect = []
        self.onDisconnect = []
        self.lock = threading.Lock()

    def connect(self):
        try:
            if bluetooth:
                debug("Connecting to %s" % BD_ADDR)
                self.sock = bluetooth.BluetoothSocket(bluetooth.RFCOMM)
                self.sock.connect((BD_ADDR, 1))
                self.sock.setblocking(True)
                self._sockSend = self.sock.send
                self._sockRecv = self.sock.recv
            else:
                # Big hack for Mac OS...
                import serial
                debug("Opening RFCOMM device %s" % BD_DEVNODE)
                self.sock = serial.Serial(BD_DEVNODE)
                self._sockSend = self.sock.write
                self._sockRecv = self.sock.read

            debug("Socket connected.")
            self.send = SendThread(self._sockSend)
            self.send.start()
            self.recv = RecvThread(self._sockRecv)
            self.recv.start()

            # Finish any unfinished command.
            self.send.queue.put(OP_NOP * MAX_COMMAND_LENGTH)

            # Let any garbage flush out of the receive pipe, then enable it.
            time.sleep(0.5)
            self.recv.discardData = False

            # Populate the region cache once
            debug("Retrieving region list...")
            self.getRegions()
            debug("Region list retrieved.")

        except:
            self.sock.close()
            self.sock = None
            raise

        self.isConnected = True
        for f in self.onConnect:
            f()

    def getRegions(self, clearCache=False):
        """Enumerate the regions that are available on the device.
           Returns a sequence of Region objects.
           """
        if clearCache:
            self._regionCache = None
        if self._regionCache is not None:
            return self._regionCache

        self.lock.acquire()

        regions = []
        self._regionsByName = {}

        # Iterate over all descriptors (even numbered regions)
        for index in range(0, MAX_REGIONS, 2):
            def storeDescriptor(descriptor, index=index):
                size = struct.unpack("<I", descriptor[:4])[0]
                name = descriptor[4:].split('\x00')[0]
                if name:
                    region = Region(index + 1, size, name)
                    regions.append(region)
                    self._regionsByName[name] = region

            # Send the command the low-level way, since we already have the lock.
            self.recv.queue.put((MAX_DESCRIPTOR_LEN, storeDescriptor))
            self.send.queue.put(opSetRegion(index) + opReadLongs(MAX_DESCRIPTOR_LEN))

        self.recv.queue.join()
        self._regionCache = regions

        self.lock.release()
        return regions    

    def findRegion(self, nameOrIndex):
        """Look for a region by name or index. Returns its Region structure."""
        self.getRegions()
        if isinstance(nameOrIndex, int):
            return self._regionCache[nameOrIndex]
        else:
            return self._regionsByName[nameOrIndex]

    def disconnect(self):
        self.isConnected = False

        self.send.stop()
        self.send.join()
        self.send = None

        self.recv.stop()
        self.recv.join()
        self.recv = None

        self.sock.close()
        self.sock = None

        for f in self.onDisconnect:
            f()

    def addCallbacks(self, onConnect=None, onDisconnect=None):
        if onConnect:
            self.onConnect.append(onConnect)
        if onDisconnect:
            self.onDisconnect.append(onDisconnect)

        if self.isConnected:
            if onConnect:
                onConnect()
        else:
            if onDisconnect:
                onDisconnect()

    def command(self, cmd, recvSize=0, callback=None):
        """This is a primitive for an atomic read/write command.
           All commands in the provided string buffer 'cmd' are
           guaranteed to happen in an unbroken sequence.

           The BluetoothConduit intentionally contains a fairly
           limited set of operations, because there are few high-level
           operations which are re-entrant. If you want to perform a
           sequence of otherwise non-atomic operations, you can submit
           them as a single command() and they will always be atomic.
           """
        if not self.isConnected:
            # If we're shutting down, break the chain of polling callbacks...
            return

        if callback and recvSize:
            self.lock.acquire()
            self.recv.queue.put((recvSize, callback))
            self.send.queue.put(cmd)
            self.lock.release()
        elif cmd:
            self.send.queue.put(cmd)

    def write(self, region, offset, data):
        """Seek to 'offset' within 'region', and write 'data'.
           """
        self.command(opSetRegion(self.findRegion(region).index) +
                     opSeek(offset) +
                     opWriteLongs(data))

    def read(self, region, offset, words, cb):
        """Seek to 'offset' within 'region', and read the specified
           number of words. When the data is ready, cb(data) will be
           invoked on the receive thread.
           """
        self.command(opSetRegion(self.findRegion(region).index) +
                     opSeek(offset) +
                     opReadLongs(words),
                     words, cb)


if __name__ == "__main__":
    # Connect and list the available regions
    bt = BluetoothConduit()
    bt.connect()
    print bt.getRegions()
    bt.disconnect()
