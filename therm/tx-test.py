#!/usr/bin/env python
#
# Transmitter prototype, implemented in python using a PC serial port
#

import time, sys

def crc8(bits):
    reg = 0x00
    for bit in bits + "00000000":
        # Shift in a bit
        reg <<= 1
        if bit == "1":
            reg |= 1

        # If a one came out, XOR with the polynomial
        if reg & 0x100:
            reg &= 0xFF
            reg ^= 0x07
    return reg

def stuff(bits):
    return bits.replace("111", "1110")

def pack(i, width):
    """Convert an integer to an arbitrary-width binary string"""
    bits = ''
    mask = 1
    for j in xrange(width):
        if i & mask:
	    bits += "1"
	else:
	    bits += "0"
        mask <<= 1
    return bits

port = open("/dev/tts/1", "wb")

def sendBits(bits):
    for bit in bits:
        if bit == "1":
	    port.write("\xC0")
	else:
	    port.write("\xFC")
    port.flush()

def sendFrame(frame, flag="011110"):
    """Receive sequences of bits bounded by two 'flag' sequences. Yields
       (bitstring, signalStrength) tuples.
       """
    preamble = "10"*100 + flag + flag
    sendBits(preamble + flag + stuff(frame) + flag)

def sendContent(content):
    sendFrame(content + pack(crc8(content), 8))

sequence = 0
def sendPacket(protocol=0, station=2, voltage=1.234, temperature=19):
    global sequence
    sequence += 1
    sendContent(pack(protocol, 2) +
                pack(station, 6) +
		pack(sequence, 5) +
		pack(int(voltage / 10.0 * 1024.0 + 0.5), 10) + 
		pack(temperature, 8))

while 1:
    sendPacket()
    time.sleep(1)
