#!/usr/bin/env python
#
# LED test proggie, for use with the 'usbtest' firmware and 'usbwidget' kernel module. Requires devfs.
#

import time

f = open("/dev/usb/widget", "wb")
while True:
    for byte in (0x01, 0x02, 0x04, 0x08):
        f.write(chr(byte))
        f.flush()
        time.sleep(0.1)
