#!/usr/bin/env python
"""
Bluetooth RFCOMM flow control test

Usage: flowtest.py <bdaddr> <channel> <hexData> <repeats>

Examples
--------

LaserProp write flow control test. Writes NOPs as fast as possible:

   ./flowtest.py 03:1F:08:07:15:2E 1 00 4096

LaserProp read test. Solicits large replies that we never read, tries
to fill up the receive buffer on our end and the transmit buffer
remotely:

   ./flowtest.py 03:1F:08:07:15:2E 1 E1FF 100

-- Micah Elizabeth Scott <micah@navi.cx>

"""

import bluetooth, sys, time, binascii


def flowTest(bdAddr, channel, block):
    sock = bluetooth.BluetoothSocket(bluetooth.RFCOMM)
    sock.connect((bdAddr, channel))
    sock.setblocking(True)
    print "Connected."

    sentBytes = 0
    startTime = time.time()
    throttleTime = startTime
    interval = 0.1

    while True:

        sentBytes += sock.send(block)

        now = time.time()
        if now > throttleTime + interval:
            throttleTime = now
            duration = now - startTime
            sys.stderr.write("\r %12.4f s, %10d bytes, %10.02f B/s " % (
                duration, sentBytes, sentBytes / duration))


if __name__ == "__main__":
    if len(sys.argv) == 5:
        flowTest(sys.argv[1], int(sys.argv[2]),
                 binascii.a2b_hex(sys.argv[3]) * int(sys.argv[4]))
    else:
        sys.stderr.write(__doc__)
