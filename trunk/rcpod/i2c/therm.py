#!/usr/bin/env python

class therm:
    def __init__(self, i2c):
        self.i2c = i2c

    def write(self, address, register, byte):
        self.i2c.start()
        self.i2c.writebyte(adress << 1)				# address, r/w bit is 0 (write)
        self.i2c.writebyte(register)
        self.i2c.writebyte(byte)
        self.i2c.stop()

    def read(self, address, register):
        self.i2c.start()
        self.i2c.writebyte(address << 1)			# address, r/w bit is 0 (write)
        self.i2c.writebyte(register)
        return self.recieve(address)

    def recieve(self, address):
        self.i2c.start()
        self.i2c.writebyte(address << 1 | 0x01)			# address, r/w bit is 1 (read)
        a = self.i2c.readbyte()
        self.i2c.stop()
        return a
