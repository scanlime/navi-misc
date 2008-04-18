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
    offset = None
    _regionCache = None
    _regionsByName = {}
    
    def __init__(self, bdaddr):
        self.sock = BluetoothSocket(RFCOMM)
        self.sock.connect((bdaddr, 1))
        self.sock.setblocking(False)

        # Finish any unfinished command, and clear out any
        # garbage from the receive buffers.
        self._send("\x00" * 255 * 4 * 2)
        self._recv(255 * 4 * 2)

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
            descriptor = self.read(self.MAX_DESCRIPTOR_LEN)

            size = struct.unpack("<I", descriptor[:4])[0]
            name = descriptor[4:].split('\x00')[0]

            if name:
                region = Region(index + 1, size, name)
                regions.append(region)
                self._regionsByName[name] = region
            
        return regions    

    def findRegion(self, name):
        """Look for a region by name. Returns its Region structure."""

        self.getRegions()
        return self._regionsByName[name]

    def close(self):
        self.sock.close()

    def _send(self, buf):
        """Blocking send.
           We use a stupid retry loop instead of using PyBlueZ's
           blocking sockets, since its blocking is non-interruptible.
           """
        while buf:
            count = self.sock.send(buf)
            assert count >= 0
            if count > 0:
                buf = buf[count:]
            else:
                time.sleep(0.01)
    
    def _recv(self, size, t=0.5):
        """Receive with a timeout.
           We use a stupid retry loop instead of using PyBlueZ's
           blocking sockets, since its blocking is non-interruptible.
           """
        deadline = time.clock() + t
        buf = self.sock.recv(size)
        while len(buf) < size and time.clock() < deadline:
            buf += self.sock.recv(size - len(buf))
            time.sleep(0)
        return buf

    def setRegion(self, index):
        self._send("\xF0" + chr(index))
        self.offset = 0

    def setRegionByName(self, name):
        self.setRegion(self.findRegion(name).index)

    def write(self, data):
        assert (len(data) % 4) == 0
        words = len(data) / 4

        if words > 255:
            self.write(data[:4*255])
            self.write(data[4*255:])
        else:
            self._send("\xD2" + chr(words) + data)
            self.offset += words

    def read(self, count):
        self._send("\xE1" + chr(count))
        self.offset += count
        return self._recv(count * 4)

    def seek(self, offset):
        self.seekRelative(offset - self.offset)

    def seekRelative(self, offset):
        self.offset += offset
        if offset < 128 and offset > -128:
            self._send("\xC3" + struct.pack("b", offset))
        else:
            self._send("\xB4" + struct.pack("<h", offset))

print "Connecting..."
bt = BluetoothConduit('03:1F:08:07:15:2E')

def textToTiles(s):
    t = [struct.pack("<H", 0x200 + (ord(c)&0xFE) + ((ord(c)&1) << 10)) for c in s]
    return ''.join(t)

bt.setRegionByName("Screen")
bt.write(textToTiles("Hello World."))

print bt.getRegions()

i = 0
t0 = time.clock()
while True:
    i = (i+1) & 0xFFFF
#    bt.setRegionByName("Screen")
    bt.seek(10)
    d = textToTiles("%04x" % i) * 10 * 13
#    d = textToTiles(''.join([random.choice("/\\-*|") for x in range(40*1)]))
    print i, len(d)
    bt.write(d)

    print "%.02f bytes/sec" % (len(d) * i / (time.clock() - t0))

    #bt.setRegionByName("Foo")
    #print struct.unpack("<I", bt.read(1))[0]
