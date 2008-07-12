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

from bluetooth import *
import time

__all__ = ['BluetoothConduit']


class Region:
    def __init__(self, index, size, name):
        self.index = index
        self.size = size
        self.name = name

    def __repr__(self):
        return "<Region #%d %r, %d words>" % (self.index, self.name, self.size)


class BluetoothConduit:
    MAX_REGIONS = 32
    MAX_DESCRIPTOR_LEN = 64
    MAX_PENDING_READS = 127

    sock = None
    offset = None
    region = None
    isConnected = False
    _regionCache = None
    _regionsByName = {}

    def __init__(self):
        self.onConnect = []
        self.onDisconnect = []
        self.onPoll = []

    def connect(self, bdaddr='03:1F:08:07:15:2E'):
        try:
            self.sock = BluetoothSocket(RFCOMM)
            self.sock.connect((bdaddr, 1))
            self.sock.setblocking(False)

            self._writeBuf = []
            self._readBuf = []
            self._readLen = 0
            self._readCb = []
            self.offset = None
            self.region = None

            # Finish any unfinished command, and clear out any
            # garbage from the receive buffers. Zero is a no-op
            # bytecode, and this is the maximum length of any
            # command that may be in progress.

            self._writeBuf.append("\x00" * 255 * 4 * 2);
            self.flush(True, True)

            # XXX: Sometimes we fail to read from the RFCOMM socket,
            #      even though the device has definitely sent us a
            #      reply. I don't know why, but this seems to work
            #      around the problem.

            time.sleep(1.0)
            self.flush(True, True)

            # Populate the region cache once
            self.getRegions()

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

        regions = []
        self._regionsByName = {}

        # Iterate over all descriptors (even numbered regions)
        for index in range(0, self.MAX_REGIONS, 2):
            self.setRegion(index)

            def storeDescriptor(descriptor, index=index):
                size = struct.unpack("<I", descriptor[:4])[0]
                name = descriptor[4:].split('\x00')[0]
                if name:
                    region = Region(index + 1, size, name)
                    regions.append(region)
                    self._regionsByName[name] = region

            self.read(self.MAX_DESCRIPTOR_LEN, storeDescriptor)

        self.flush(readBlock=True)
        self._regionCache = regions
        return regions    

    def findRegion(self, name):
        """Look for a region by name. Returns its Region structure."""

        self.getRegions()
        return self._regionsByName[name]

    def disconnect(self):
        self.sock.close()
        self.sock = None
        self.isConnected = False
        for f in self.onDisconnect:
            f()

    def addCallbacks(self, onConnect=None, onDisconnect=None, onPoll=None):
        if onConnect:
            self.onConnect.append(onConnect)
        if onDisconnect:
            self.onDisconnect.append(onDisconnect)
        if onPoll:
            self.onPoll.append(onPoll)

        if self.isConnected:
            if onConnect:
                onConnect()
        else:
            if onDisconnect:
                onDisconnect()

    def setRegion(self, index):
        if self.region == index:
            self.seek(0)
        else:
            self._writeBuf.append("\xF0" + chr(index))
            self.offset = 0

    def setRegionByName(self, name):
        self.setRegion(self.findRegion(name).index)

    def seek(self, offset):
        self.seekRelative(offset - self.offset)

    def seekRelative(self, offset):
        if not offset:
            return
        self.offset += offset
        if offset < 128 and offset > -128:
            self._writeBuf.append("\xC3" + struct.pack("b", offset))
        else:
            self._writeBuf.append("\xB4" + struct.pack("<h", offset))

    def write(self, data):
        """Asynchronously write 'data' at the current position.
           Data must be a multiple of 4 bytes in length. Each 32-bit
           word will be written atomcially.

           Writes are buffered until the flush() method is called.
           """
        assert (len(data) % 4) == 0
        words = len(data) / 4

        if words > 255:
            self.write(data[:4*255])
            self.write(data[4*255:])
        else:
            self._writeBuf.append("\xD2" + chr(words) + data)
            self.offset += words

    def read(self, count, callback):
        """Read 'count' words of data from the current position.
           When the data is available, the provided callback is
           invoked with a single string argument.

           The provided callback will be called during a
           future flush().
           """
        assert len(self._readCb) <= self.MAX_PENDING_READS
        if len(self._readCb) == self.MAX_PENDING_READS:
            self.flush(readBlock=True)
            assert not self._readCb
            
        self._readCb.append((count, callback))
        self._writeBuf.append("\xE1" + chr(count))
        self.offset += count

    def poll(self):
        """Flush unsent data, and allow clients the opportunity
           to poll the hardware when necessary.
           """
        for cb in self.onPoll:
            cb()
        self.flush()

    def flush(self, writeBlock=False, readBlock=False, timeout=2.0):
        """Send and receive data, with or without blocking.
           By default, this will send and receive all data
           that can be processed without blocking. To block
           writes (and provide flow control), set writeBlock.
           To block reads (and guarantee that async reads
           have completed), set readBlock.
           """
        self._writeBuf = [''.join(self._writeBuf)]
        running = True
        deadline = time.clock() + timeout

        while running:
            running = False
            delay = True

            if time.clock() > deadline:
                raise IOError("Timeout waiting for firmware response.")
            
            if self._writeBuf[0]:
                # Try to write some data

                writeCount = self.sock.send(self._writeBuf[0])
                assert writeCount >= 0

                if writeCount > 0:
                    # Write successful. Keep going.
                    self._writeBuf[0] = self._writeBuf[0][writeCount:]
                    running = True
                    delay = False
                    deadline = time.clock() + timeout
                    
                elif writeBlock and self._writeBuf[0]:
                    # Write was unsuccessful, but we'll block until we finish.
                    running = True

            else:
                writeCount = 0

            # Try to read some data
            readStr = self.sock.recv(4096)
            if readStr:
                # Read successful. Keep going.
                running = True
                delay = False
                deadline = time.clock() + timeout

                self._readLen += len(readStr)
                self._readBuf.append(readStr)

                if not self._readCb:
                    # We're reading junk. This could happen
                    # at the first flush() during initialization.
                    self._readBuf = []
                    self._readLen = 0

                # Can we dispatch the next callback?
                while self._readCb:
                    nextCbSize, nextCb = self._readCb[0]
                    nextCbSize *= 4
                    if self._readLen >= nextCbSize:
                        del self._readCb[0]
                        block = ''.join(self._readBuf)
                        self._readBuf = [block[nextCbSize:]]
                        self._readLen -= nextCbSize
                        nextCb(block[:nextCbSize])
                    else:
                        break

            elif readBlock and self._readCb:
                # No data to read, but we'll block until we finish.
                running = True

            if delay:
                # If no actual reads/writes occurred, yield the CPU.
                time.sleep(0.01)

        if readBlock:
            assert not self._readCb


if __name__ == "__main__":
    # Connect and list the available regions
    bt = BluetoothConduit()
    print "Connecting..."
    bt.connect()
    print bt.getRegions()
