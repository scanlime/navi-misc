#!/usr/bin/env python
import rcpod, therm

pic = rcpod.Device()
eeprom = therm.eeprom(pic)

eeprom.write_byte(0x0000, 0x5a)
blah = eeprom.random_read(0x0000)
for i in range(8):
    print '0x%x' % blah[i]
