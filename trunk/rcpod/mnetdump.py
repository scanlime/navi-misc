#!/usr/bin/env python
#
# Disassembles, validates, and dumps all packets received over the RS-485 port.
# Designed for the rcpod-485 board and the CX-1 micronet.
#
# Anatomy of a micronet packet:
#   - First byte is a destination device ID
#   - Second byte is a source device ID
#   - Third byte is the number of following data bytes
#   - Data bytes
#   - An 8-bit checksum byte such that all bytes in the packet add up to 255, modulo 256
#
# -- Micah Dowty <micah@picogui.org>
#
import pyrcpod, time, sys

logfile = open("mnetdump.log", "a")
timeout = 0.5


def log(msg):
    """Log the given line to stdout and to disk, including a timestamp.
       The provided message can be multiple lines."""
    heading = time.strftime("%b %d %Y %H:%M:%S -- ", time.localtime())
    lines = msg.split("\n")
    logEntry = heading + lines[0] + "\n"
    for line in lines[1:]:
        logEntry += " " * len(heading) + line + "\n"
    for dest in (logfile, sys.stdout):
        dest.write(logEntry)
        dest.flush()


def hexDump(bytes, bytesPerLine=16):
    """Convert the given list of bytes to a multiline hexdump string"""
    lines = []
    while bytes:
        line = bytes[:bytesPerLine]
        del bytes[:bytesPerLine]
        lines.append(" ".join(["%02X" % byte for byte in line]))
    return "\n".join(lines)


def checkForPackets(buffer):
    """Determine whether the buffer contains a valid packet yet,
       and if so, begin the process of dissecting it and logging it
       properly.
       """
    if len(buffer) < 4:
        return
    packetLen = buffer[2] + 4
    if len(buffer) >= packetLen:
        packet = buffer[:packetLen]
        del buffer[:packetLen]
        dumpPacket(packet)


def decodeAddress(address):
    """Given a micronet address, adds extra information about what
       device it represents if possible.
       """
    devTable = {
        2: 'Critical Decoder',
        4: 'Power',
        7: 'ADCS',
        8: 'Science',
        10: 'mnetsend',
        }
    devName = devTable.get(address, "Unknown")
    return "%s (%d)" % (devName, address)


def dumpPacket(packet):
    """Validate the given packet and log its contents"""
    destination = decodeAddress(packet[0])
    source = decodeAddress(packet[1])
    dataLen = packet[2]
    data = packet[3:-1]

    checksum = 0
    for byte in packet:
        checksum += byte
    if (checksum & 0xFF) == 0xFF:
        status = "Valid packet received"
    else:
        status = "Packet received with INVALID CHECKSUM"

    log("%s\nFrom %s, to %s\n%s" % (status, source, destination, hexDump(data)))


def main():
    dev = pyrcpod.devices[0].open(pyrcpod.Rcpod485)
    dev.serialInit(9600)
    dev.serialRxStart()
    buffer = []
    timestamp = None
    log("Listening for packets...")

    while True:
        newData = dev.serialRxRead()

        if newData:
            timestamp = time.time()
            buffer.extend(newData)
            checkForPackets(buffer)

        elif timestamp and time.time() > timestamp + timeout:
            # It's been a while since the last data received,
            # if we have any leftover cruft in our buffer, dump it
            timestamp = None
            if buffer:
                log("Expired partial packet:\n" + hexDump(buffer))
                buffer = []


if __name__ == "__main__":
    main()
