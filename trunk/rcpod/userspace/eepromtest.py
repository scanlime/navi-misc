#!/usr/bin/env python
import rcpod, i2c

pic = rcpod.Device()
i2c = i2c.i2c(pic)
eeprom = i2c.devices.eeprom(i2c)

eeprom.write_byte(0x0000, 0x42)
blah = eeprom.random_read(0x0000)
for i in range(8):
    print '0x%x' % blah[i]
