import time, struct, fcntl

class FieldSensor:
    def __init__(self, devName="/dev/usb/efs0"):
        self.dev = open(devName)
        self.initScan()

    def resetParams(self):
        """Initialize all parameter blocks to zero, stopping all sampling"""
        for block in xrange(8):
            for param in xrange(8):
                self.setParam(block, param, 0)
        # Discard the current accumulator since we just changed the definition of our readings
        #self.readPacket()

    def initScan(self):
        """Initialize parameters to scan all TX/RX pairs"""
        block = 0
        for adcon in (0x81, 0x89):
            for xor in (0x03, 0x0C, 0x30, 0xC0):
                self.setParam(block, 0, block)    # EFS_PARAM_RESULT_NUM
                self.setParam(block, 1, xor)      # EFS_PARAM_LC_PORT_XOR
                self.setParam(block, 2, adcon)    # EFS_PARAM_ADCON_INIT
                self.setParam(block, 3, 227)      # EFS_PARAM_PERIOD
                self.setParam(block, 4, 226)      # EFS_PARAM_PHASE
                self.setParam(block, 5, 30)       # EFS_PARAM_NUM_HALF_PERIODS
                self.setParam(block, 6, 0xFF-xor) # EFS_PARAM_LC_TRIS_INIT
                self.setParam(block, 7, 0x55)     # EFS_PARAM_LC_PORT_INIT
                block += 1
        #self.readPacket()

    def readPacket(self):
        return struct.unpack("i" * 9, self.dev.read(4*9))

    def readAverages(self):
        packet = self.readPacket()
        if packet[8]:
            return [packet[i] / float(packet[8]) for i in xrange(8)]

    def setParam(self, block, param, value):
        fcntl.ioctl(self.dev, 0x3A01, struct.pack("iii", block, param, value))
