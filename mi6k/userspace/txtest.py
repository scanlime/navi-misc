#!/usr/bin/env python
#
# Tiny IR transmitter test
#
f = open("/dev/lirc", "w")

def t(*a):
    for v in a:
        f.write(chr(v & 0xFF) + chr(v >> 8) + chr(0) + chr(0))

while True:
    t(1,1, 1000,1000, 3000,2000, 1000,1000,)

