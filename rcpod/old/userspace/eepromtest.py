#!/usr/bin/env python
import rcpod, i2c

pic = rcpod.Device()
i2cb = i2c.i2c_bus(pic)
eeprom = i2c.eeprom(i2cb)

eeprom.write_byte(0x0000, 0x42)
blah = eeprom.random_read(0x0000)
for i in range(8):
    print '0x%x' % blah[i]
