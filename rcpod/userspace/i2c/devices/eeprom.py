#!/usr/bin/env python
import i2c

class eeprom:
    def __init__(self, i2c):
        self.i2c = i2c

    def write_byte(self, address, byte):
        i2c.start()
	i2c.writebyte(0xa0)					# device address + write
	i2c.writebyte((address >> 8) & 0xff)			# high byte
	i2c.writebyte(address & 0xff)				# low byte
	i2c.writebyte(byte)					# data
	i2c.stop()

    def random_read(self, address):
        i2c.start()
	i2c.writebyte(0xa0)					# device address + write
	i2c.writebyte((address >> 8) & 0xff)			# high byte
	i2c.writebyte(address & 0xff)				# low byte
	i2c.start()
	i2c.writebyte(0xa1)					# device address + read
	blah = i2c.readbyte()
	i2c.stop()
	return blah
