import serial, time

class sensor:
    def __init__(self, serialPort=0):
        self.port = serial.Serial(
            port = serialPort,
            baudrate = 19200,
            stopbits = serial.STOPBITS_ONE,
            timeout = 2.0)
        self.buffer = ''
        self.reset()

    def reset(self):
        # Reset the board
        self.port.setRTS(0)
        self.port.setDTR(1)
        time.sleep(0.005)
        self.port.setDTR(0)

        # Get WLoader to start our app
        self.sendSlowly('0000hg');

    def sendSlowly(self, str):
        for chr in str:
            time.sleep(0.05)
            self.port.write(chr)

    def read(self):
        while True:
            self.buffer += self.port.read()
            self.buffer = self.buffer[-18:]
            # Synchronize to 0x80 synchronization byte
            if self.buffer[0] != chr(0x80):
                self.buffer = ''
            # Expect an 18-byte packet
            elif len(self.buffer) == 18:
                # Verify the checksum byte
                checksum = 0
                for c in self.buffer[1:-1]:
                    checksum += ord(c)
                checksum %= 256
                # Decode the 16-byte packet of 8 16-bit values
                if checksum == ord(self.buffer[17]):
                    return ( (ord(self.buffer[1])*256 + ord(self.buffer[2]))/65535.0,
                             (ord(self.buffer[3])*256 + ord(self.buffer[4]))/65535.0,
                             (ord(self.buffer[5])*256 + ord(self.buffer[6]))/65535.0,
                             (ord(self.buffer[7])*256 + ord(self.buffer[8]))/65535.0,
                             (ord(self.buffer[9])*256 + ord(self.buffer[10]))/65535.0,
                             (ord(self.buffer[11])*256 + ord(self.buffer[12]))/65535.0,
                             (ord(self.buffer[13])*256 + ord(self.buffer[14]))/65535.0,
                             (ord(self.buffer[15])*256 + ord(self.buffer[16]))/65535.0 )


