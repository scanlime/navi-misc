#!/usr/bin/env python
#
# Receiver prototype, implemented in python using a PC serial port
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

def receiveBits(device):
    """Receive bits from the device, yielding (bit, noise) tuples"""
    bitCounts = createBitCountTable()
    port = open(device, "rb")

    while 1:
        # Each byte read from the serial port is actually one bit
        # of data. We can measure noise by checking how closely it
        # matches the ideal bit patterns.
        b = port.read(1)
        if bitCounts[ord(b)] < 4:
            yield ("1", bitCounts[ord(b) ^ 0xC0])
        else:
            yield ("0", bitCounts[ord(b) ^ 0xFC])


def receiveFrames(device, flag="011110"):
    """Receive sequences of bits bounded by two 'flag' sequences. Yields
       (bitstring, signalStrength) tuples.
       """
    bits = ''
    noise = 0

    for bit, bitNoise in receiveBits(device):
        noise += bitNoise
        bits += bit

        # Put an upper limit on the buffer size
        if len(bits) > 1024:
            noise = 0
            bits = ''

        # If we've received a flag, examine the contents. It might be a valid packet
        if bits.endswith(flag):
            # Measure the percentage of total sub-bits received that were different from the ideal patterns
            noiseRatio = noise / (len(bits) * 8.0)
            noise = 0

            # If we're receiving completely random data, the noise ratio will be about 1/3. Scale it so
            # that 1/3 noise is a signal strength of 0, and no noise is a signal strength of 100%
            signalStrength = (1 - min(noiseRatio, 1.0/3.0) * 3) * 100.0

            yield (bits[:-len(flag)], signalStrength)
            bits = ''


def receiveContent(device):
    """Receive validated packet content. Yields (content, signalStrength) tuples"""
    for bits, signalStrength in receiveFrames(device):
        # Unstuff it. Is it too short?
        bits = unstuff(bits)
        if len(bits) < 8:
            continue

        # Check the CRC
        content = bits[:-8]
        crc = unpack(bits[-8:])
        if crc8(content) != crc:
            continue

        # Yay, it's valid
        yield (content, signalStrength)

logFile = None
def log(*values):
    """Log a tuple of values in CSV format to stdout and a log file"""
    global logFile
    if logFile is None:
        logFile = open("rx-log.csv", "w")
    line = ", ".join([str(value) for value in values])
    print line
    logFile.write(line + "\n")
    logFile.flush()


if __name__ == "__main__":
    log("time", "signalStrength", "protocol", "station", "sequence", "voltage", "temperature")
    startTime = time.time()

    for content, signalStrength in receiveContent("/dev/usb/tts/0"):
        if len(content) != 31:
            continue

        try:
            protocol = unpack(content[:2])
            station = unpack(content[2:8])
            sequence = unpack(content[8:13])
            voltage = unpack(content[13:23]) / 1024.0 * 10
            temperature = int(content[23:31], 2)
            if temperature & 0x80:
                temperature -= 0x100

            log("%.04f" % (time.time() - startTime),
                "%.04f" % signalStrength,
                protocol, station, sequence,
                "%.04f" % voltage,
                temperature)
        except:
            print sys.exc_info()[1]
