#!/usr/bin/env python
#
# Read therm data from a bunch of i2c busses
# Assumes that the therms are microchip 'tc74's
#

import rcpod

class therm:
    def __init__(self, pic):
        self.pic	= pic
        self.busmask	= 0x04	# 0000 0100 - i2c bus on RB2
	self.clockmask	= 0x02	# 0000 0010 - i2c clock on RB1
	self.b		= 0x00

    def i2c_readbit(self):
        self.b = self.busmask | ~self.clockmask			# bus high (input), clock low
	self.pic.TRISB = self.b
	self.b |= self.clockmask				# clock high
	self.pic.TRISB = self.b
	self.b &= ~self.clockmask				# clock low
	self.pic.TRISB = self.b
	return self.PORTB

    def i2c_start(self):
        self.b = self.busmask | self.clockmask			# bus high, clock high
	self.pic.TRISB = self.b
	self.b &= ~self.busmask					# bus low
	self.pic.TRISB = self.b
	self.b &= ~self.clockmask				# clock low
	self.pic.TRISB = self.b

    def i2c_stop(self):
        self.b = ~self.busmask | self.clockmask			# bus low, clock high
	self.pic.TRISB = self.b
	self.b |= self.busmask					# bus high
	self.pic.TRISB = self.b
	self.b &= ~self.clockmask				# clock low
	self.pic.TRISB = self.b

    def i2c_writebit(self, v):
        if v:
	    self.b = self.busmask | ~self.clockmask		# bus high, clock low
	    self.pic.TRISB = self.b
	else:
	    self.b = ~self.busmask | ~self.clockmask		# bus low, clock low
	    self.pic.TRISB = self.b
	self.b |= self.clockmask				# clock high
	self.pic.TRISB = self.b
	self.b &= ~self.clockmask				# clock low
	self.pic.TRISB = self.b
        pass

    def i2c_ack(self):
    	# for now, just do a clock cycle but don't read
	self.b = self.clockmask					# bus high, clock high
	self.pic.TRISB = self.b
	self.b &= ~self.clockmask				# bus high, clock low
	self.pic.TRISB = self.b

    def i2c_writebyte(self, byte):
        for i in range(7, -1, -1):
            bit = (byte >> i) & 0x01
            self.i2c_writebit(bit)

    def i2c_readbyte(self):
        # hmm, tricky. perhaps return a list of bytes?
	return 0x00

    def i2c_write(self, address, register, byte):
        self.i2c_start()
	self.i2c_writebyte(adress << 1)				# address, r/w bit is 0 (write)
	self.i2c_ack()
	self.i2c_writebyte(register)
	self.i2c_ack()
	self.i2c_writebyte(byte)
	self.i2c_ack()
	self.i2c_stop()

    def i2c_read(self, address, register):
        self.i2c_start()
	self.i2c_writebyte(address << 1)			# address, r/w bit is 0 (write)
	self.i2c_ack()
	self.i2c_writebyte(register)
	self.i2c_ack()
	return self.i2c_recieve(address)

    def i2c_recieve(self, address):
        self.i2c_start()
	self.i2c_writebyte(address << 1 | 0x01)			# address, r/w bit is 1 (read)
	self.i2c_ack()
	a = self.i2c_readbyte()
	self.i2c_writebit(0)					# nack
	self.i2c_stop()
	return a
