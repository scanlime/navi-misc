#!/usr/bin/env python
#
# Client for UDP to Audio example for etherCog.
#
# Usage:
#   python demo-udp-audio.py <filename> [<filename> ...]
#
#   Where <filename> must be a 16-bit uncompressed 44.1 kHz WAV file.
#   It will play on loop, to the etherCog device at IP_ADDR below.
#
#   This program infinitely loops one or more WAV files. If you specify
#   more than one file, they will play in sequence.
#
# Micah Dowty <micah@navi.cx>
#

UDP_PORT    = 6502
IP_ADDR     = "192.168.1.32"
BUFFER_SIZE = 1024
SAMPLE_HZ   = 44100.0
BYTE_HZ     = SAMPLE_HZ * 4

import socket, time, sys

def play(filename):
    f = open(filename, "rb")

    startTime = time.time()
    bytesTotal = 0

    print "Playing %r" % filename
    while True:
        block = f.read(BUFFER_SIZE)
        if not block:
            break

        s.send(block)
        bytesTotal += BUFFER_SIZE

        elapsed = bytesTotal / BYTE_HZ
        deadline = startTime + elapsed
        delay = deadline - time.time()
        if delay > 0:
            time.sleep(delay)

s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
s.connect((IP_ADDR, UDP_PORT))

while True:
    for filename in sys.argv[1:]:
        play(filename)
