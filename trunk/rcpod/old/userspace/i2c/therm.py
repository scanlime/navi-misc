#!/usr/bin/env python
import i2c

class therm:
    def __init__(self, i2c):
        self.i2c = i2c

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
