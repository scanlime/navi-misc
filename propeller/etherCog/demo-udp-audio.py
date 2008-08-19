#!/usr/bin/env python
#
# Client for UDP to Audio example for etherCog
#
# Micah Dowty <micah@navi.cx>
#

UDP_PORT    = 1234
IP_ADDR     = "192.168.1.32"
BUFFER_SIZE = 512
SAMPLE_HZ   = 44100.0
BYTE_HZ     = SAMPLE_HZ * 4

import socket, time, sys

s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
s.bind(("", 12345))
s.connect((IP_ADDR, UDP_PORT))

while True:
    f = open(sys.argv[1], "rb")

    startTime = time.time()
    bytesTotal = 0

    while True:
        block = f.read(BUFFER_SIZE)
        if not block:
            break

        s.send(block)
        bytesTotal += BUFFER_SIZE

        elapsed = bytesTotal / BYTE_HZ
        print elapsed
        deadline = startTime + elapsed
        delay = deadline - time.time()
        if delay > 0:
            time.sleep(delay)
        


    
