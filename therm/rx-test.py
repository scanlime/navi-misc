#!/usr/bin/env python
#
# Receiver prototype, implemented in python using a PC serial port
#

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

def unstuff(bits):
    return bits.replace("1110", "111")

def unpack(bits):
    """Convert an LSB-first arbitrary width binary string to an integer"""
    i = 0
    mask = 1
    for bit in bits:
        if bit == "1":
            i |= mask
        mask <<= 1
    return i

def createBitCountTable():
    """Return a lookup table for counting '1' bits"""
    table = []
    for byte in xrange(256):
        count = 0
        for bit in xrange(8):
            if byte & 1:
                count += 1
            byte >>= 1
        table.append(count)
    return table

def receiveBits():
    bitCounts = createBitCountTable()
    port = open("/dev/usb/tts/0", "rb")
    bits = ''
    noise = 0
    flag = "011110"

    while 1:
        # Each byte read from the serial port is actually one bit
        # of data. We can measure noise by checking how closely it
        # matches the ideal bit patterns.
        b = port.read(1)
        if bitCounts[ord(b)] < 4:
            noise += bitCounts[ord(b) ^ 0xC0]
            bits += "1"
        else:
            noise += bitCounts[ord(b) ^ 0xFC]
            bits += "0"

        # Put an upper limit on the buffer size
        if len(bits) > 1024:
            bits = ''
            noise = 0

        # If we've received a flag, examine the contents. It might be a valid packet
        if bits.endswith(flag):
            # Measure the percentage of total sub-bits received that were different from the ideal patterns
            noiseRatio = noise / (len(bits) * 8.0)
            noise = 0

            # If we're receiving completely random data, the noise ratio will be about 1/3. Scale it so
            # that 1/3 noise is a signal strength of 0, and no noise is a signal strength of 100%
            signalStrength = (1 - min(noiseRatio, 1.0/3.0) * 3) * 100.0

            receivedRawPacket(bits[:-len(flag)], signalStrength)
            bits = ''

def receivedRawPacket(bits, signalStrength):
    """We received a bit sequence between two flags. It might be a valid packet"""
    # Unstuff it. Is it too short?
    bits = unstuff(bits)
    if len(bits) < 8:
        return

    # Check the CRC
    content = bits[:-8]
    crc = unpack(bits[-8:])
    if crc8(content) != crc:
        return

    # Yay, it's valid
    print content, signalStrength

if __name__ == "__main__":
    receiveBits()
