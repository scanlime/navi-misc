#!/usr/bin/env python
import i2c

class eeprom:
    def __init__(self, i2c):
        self.i2c = i2c

    def write_byte(self, address, byte):
        self.i2c.start()
	self.i2c.writebyte(0xa0)					# device address + write
	self.i2c.writebyte((address >> 8) & 0xff)			# high byte
	self.i2c.writebyte(address & 0xff)				# low byte
	self.i2c.writebyte(byte)					# data
	self.i2c.stop()

    def page_write(self, address, data*):
        self.i2c.start()
	self.i2c.writebyte(0xa0)					# device address + write
	self.i2c.writebyte((address >> 8) & 0xff)			# high byte
	self.i2c.writebyte(address & 0xff)				# low byte
	for byte in data:
	    self.i2c.writebyte(byte)
	self.i2c.stop()

    def random_read(self, address):
        self.i2c.start()
	self.i2c.writebyte(0xa0)					# device address + write
	self.i2c.writebyte((address >> 8) & 0xff)			# high byte
	self.i2c.writebyte(address & 0xff)				# low byte
	self.i2c.start()
	self.i2c.writebyte(0xa1)					# device address + read
	blah = self.i2c.readbyte()
	self.i2c.stop()
	return blah

    def current_read(self):
        self.i2c.start()
	self.i2c.writebyte(0xa1)					# device address + read
	blah = self.i2c.readbyte()
	self.i2c.stop()
	return blah

    def sequential_read(self, n):
        blah = []
        self.i2c.start()
	self.i2c.writebyte(0xa1)					# device address + read
	for i in range(n - 1):
	    blah.append(self.i2c.readbyte_ack())
	blah.append(self.i2c.readbyte())
	self.i2c.stop()
	return blah
