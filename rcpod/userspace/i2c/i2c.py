#!/usr/bin/env python
#
# Read therm data from a bunch of i2c busses
# Assumes that the therms are microchip 'tc74's
#

import rcpod

class i2c:
    def __init__(self, pic):
        self.pic	= pic
        self.busmask	= 0x04	# 0000 0100 - i2c bus on RB2
	self.clockmask	= 0x02	# 0000 0010 - i2c clock on RB1
	self.b		= 0x00

    def readbit(self):
        self.b = self.busmask & ~self.clockmask			# bus high (input), clock low
	self.pic.TRISB = self.b
	self.b |= self.clockmask				# clock high
	self.pic.TRISB = self.b
        bit = self.pic.PORTB & self.busmask			# read when clock is high (data valid)
	self.b &= ~self.clockmask				# clock low
	self.pic.TRISB = self.b
	return bit

    def start(self):
        self.b = self.busmask | self.clockmask			# bus high, clock high
	self.pic.TRISB = self.b
	self.b &= ~self.busmask					# bus low
	self.pic.TRISB = self.b
	self.b &= ~self.clockmask				# clock low
	self.pic.TRISB = self.b

    def stop(self):
        self.b = ~self.busmask & self.clockmask			# bus low, clock high
	self.pic.TRISB = self.b
	self.b |= self.busmask					# bus high
	self.pic.TRISB = self.b
	self.b &= ~self.clockmask				# clock low
	self.pic.TRISB = self.b

    def writebit(self, v):
        if v:
	    self.b = self.busmask & ~self.clockmask		# bus high, clock low
	    self.pic.TRISB = self.b
	else:
	    self.b = ~self.busmask & ~self.clockmask		# bus low, clock low
	    self.pic.TRISB = self.b
	self.b |= self.clockmask				# clock high
	self.pic.TRISB = self.b
	self.b &= ~self.clockmask				# clock low
	self.pic.TRISB = self.b
        pass

    def ack(self):
    	# for now, just do a clock cycle but don't read
	self.b = self.clockmask					# bus high, clock high
	self.pic.TRISB = self.b
	self.b &= ~self.clockmask				# bus high, clock low
	self.pic.TRISB = self.b

    def nack(self):
        self.writebit(0)

    def writebyte(self, byte):
        for i in range(8):
            self.writebit(byte & 0x80 != 0)
            byte <<= 1
	self.ack()

    def readbyte(self):
        # just returns a sequence of stuff on portb - would be
	# nice to slice out only the active i2c busses
	a = [0, 0, 0, 0, 0, 0, 0, 0]
	for i in range(8):
	    bits = self.readbit()
	    for j in range(8):
	        a[j] <<= 1
		a[j] |= (bits >> j) & 0x01
	self.nack()
	return a

class therm(i2c):
    def write(self, address, register, byte):
        self.start()
	self.writebyte(adress << 1)				# address, r/w bit is 0 (write)
	self.writebyte(register)
	self.writebyte(byte)
	self.stop()

    def read(self, address, register):
        self.start()
	self.writebyte(address << 1)				# address, r/w bit is 0 (write)
	self.writebyte(register)
	return self.recieve(address)

    def recieve(self, address):
        self.start()
	self.writebyte(address << 1 | 0x01)			# address, r/w bit is 1 (read)
	a = self.readbyte()
	self.stop()
	return a
