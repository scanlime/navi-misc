class MultiBus:
    """Multiple I2C busses, with data lines all connected to one port (currently PORTB)
       and clock shared (pin RA0)

       busMask: mask of bits on PORTB being used for I2C busses
       """
    def __init__(self, pic, busMask=0xFF):
        self.pic	= pic
        self.busmask	= busMask
	self.pic.poke('trisa', 0xFE)				# clock line set to output

    def start(self):
        self.pic.poke('trisb', self.busmask)			# bus high
        self.pic.poke('porta', 0x01)				# clock high
        self.pic.poke('trisb', ~self.busmask)			# bus low
        self.pic.poke('porta', 0x00)				# clock low

    def stop(self):
        self.pic.poke('trisb', ~self.busmask)			# bus low
        self.pic.poke('porta', 0x01)				# clock high
        self.pic.poke('trisb', self.busmask)			# bus high
        self.pic.poke('porta', 0x00)				# clock low

    def writebit(self, v):
        if v:
            self.pic.poke('trisb', self.busmask)		# bus high
        else:
            self.pic.poke('trisb', ~self.busmask)		# bus low
        self.pic.poke('porta', 0x01)				# clock high
        self.pic.poke('porta', 0x00)				# clock low

    def readbit(self):
        self.pic.poke('trisb', self.busmask)			# bus high (input)
        self.pic.poke('porta', 0x01)				# clock high
        bit = self.pic.peek('portb')				# read when the clock is high
        self.pic.poke('porta', 0x00)				# clock low
        return bit

    def ack(self):
        # for now, just do a clock cycle - should really read the
        # value and check for correctness
        self.pic.poke('trisb', self.busmask)			# bus high (input)
        self.pic.poke('porta', 0x01)				# clock high
        self.pic.poke('porta', 0x00)				# clock low

    def nack(self):
        self.writebit(0)

    def writebyte(self, byte):
        for i in range(8):
            self.writebit(byte & 0x80 != 0)
            byte <<= 1
        self.ack()

    def readbyte(self, ack=False):
        # just returns a sequence of stuff on trisb - would be
        # nice to slice out only the active i2c busses
        a = [0, 0, 0, 0, 0, 0, 0, 0]
        for i in range(8):
            bits = self.readbit()
            for j in range(8):
                a[j] <<= 1
                a[j] |= (bits >> j) & 0x01
        if ack:
            self.ack()
        else:
            self.nack()
        return a
