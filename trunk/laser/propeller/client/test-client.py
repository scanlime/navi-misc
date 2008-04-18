from bluetooth import *
import sys, time, random

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

    offset = None
    _regionCache = None
    _regionsByName = {}
    
    def __init__(self, bdaddr='03:1F:08:07:15:2E'):
        self.sock = BluetoothSocket(RFCOMM)
        self.sock.connect((bdaddr, 1))
        self.sock.setblocking(False)

        self._writeBuf = []
        self._readBuf = []
        self._readLen = 0
        self._readCb = []

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

    def getRegions(self, clearCache=False):
        """Enumerate the regions that are available on the device.
           Returns a sequence of Region objects.
           """
        if clearCache:
            self._regionCache = None
        if self._regionCache is not None:
            return self._regionCache

        self._regionCache = regions = []
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
        return regions    

    def findRegion(self, name):
        """Look for a region by name. Returns its Region structure."""

        self.getRegions()
        return self._regionsByName[name]

    def close(self):
        self.sock.close()

    def setRegion(self, index):
        self._writeBuf.append("\xF0" + chr(index))
        self.offset = 0

    def setRegionByName(self, name):
        self.setRegion(self.findRegion(name).index)

    def seek(self, offset):
        self.seekRelative(offset - self.offset)

    def seekRelative(self, offset):
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

                # Can we dispatch the next callback?
                assert self._readCb
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


print "Connecting..."
bt = BluetoothConduit()
print "Enumerating..."
print bt.getRegions()

def textToTiles(s):
    t = [struct.pack("<H", 0x200 + (ord(c)&0xFE) + ((ord(c)&1) << 10)) for c in s]
    return ''.join(t)

bt.setRegionByName("Screen")
bt.write(textToTiles("Hello World."))
bt.flush(True, True)

i = 0
t0 = time.clock()
while True:
    i = (i+1) & 0xFFFF
    bt.setRegionByName("Screen")
    d = textToTiles("%04x" % i) * 10 * 4
#    d = textToTiles(''.join([random.choice("/\\-*|") for x in range(40*13)]))
#    print i, len(d)
    bt.write(d)

    print "%.02f bytes/sec" % (len(d) * i / (time.clock() - t0))

    bt.setRegionByName("Foo")
    def cb(s):
        print "Reading: 0x%08x" % struct.unpack("<I", s)[0]
    bt.read(1, cb)

    # Flow control
    bt.flush(writeBlock=True)
