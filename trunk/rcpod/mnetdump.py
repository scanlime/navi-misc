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


class MnetLogger:
    def __init__(self, rcpod, logFile="mnetdump.log", timeout=0.5):
        self.timeout = 0.5
        self.logFiles = (sys.stdout, open(logFile, "a"))
        self.rcpod = rcpod
        self.rcpod.serialInit(9600)
        self.buffer = []
        self.timestamp = None

        self.devTable = {
            2: 'Critical Decoder',
            4: 'Power',
            7: 'ADCS',
            8: 'Science',
            10: 'mnetsend',
            }

    def run(self):
        """Start receiving data, logging it until interrupted"""
        dev.serialRxStart()
        self.log("Starting log")
        try:
            try:
                while True:
                    self.poll()
            except KeyboardInterrupt:
                pass
        finally:
            self.log("Ending log")

    def poll(self):
        """Poll for new data, logging what's received"""
        newData = dev.serialRxRead()

        if newData:
            self.timestamp = time.time()
            self.buffer.extend(newData)
            self.checkForPackets()

        elif self.timestamp and time.time() > self.timestamp + self.timeout:
            # It's been a while since the last data received,
            # if we have any leftover cruft in our buffer, dump it
            self.timestamp = None
            if self.buffer:
                self.log("Expired partial packet:\n" + self.hexDump(self.buffer))
                self.buffer = []

    def log(self, msg):
        """Log the given line to stdout and to disk, including a timestamp.
           The provided message can be multiple lines."""
        heading = time.strftime("%b %d %Y %H:%M:%S -- ", time.localtime())
        lines = msg.split("\n")
        logEntry = heading + lines[0] + "\n"
        for line in lines[1:]:
            logEntry += " " * len(heading) + line + "\n"
        for dest in self.logFiles:
            dest.write(logEntry)
            dest.flush()

    def hexDump(self, bytes, bytesPerLine=16):
        """Convert the given list of bytes to a multiline hexdump string"""
        lines = []
        while bytes:
            line = bytes[:bytesPerLine]
            del bytes[:bytesPerLine]
            lines.append(" ".join(["%02X" % byte for byte in line]))
        return "\n".join(lines)

    def checkForPackets(self):
        """Determine whether the buffer contains a valid packet yet,
           and if so, begin the process of dissecting it and logging it
           properly.
           """
        if len(self.buffer) < 4:
            return
        packetLen = self.buffer[2] + 4
        if len(self.buffer) >= packetLen:
            packet = self.buffer[:packetLen]
            del self.buffer[:packetLen]
            self.dumpPacket(packet)

    def decodeAddress(self, address):
        """Given a micronet address, adds extra information about what
           device it represents if possible.
           """
        devName = self.devTable.get(address, "Unknown")
        return "%s (%d)" % (devName, address)

    def dumpPacket(self, packet):
        """Validate the given packet and log its contents"""
        destination = self.decodeAddress(packet[0])
        source = self.decodeAddress(packet[1])
        dataLen = packet[2]
        data = packet[3:-1]

        checksum = 0
        for byte in packet:
            checksum += byte
        if (checksum & 0xFF) == 0xFF:
            status = "Valid packet received"
        else:
            status = "Packet received with INVALID CHECKSUM"

        self.log("%s\nFrom %s, to %s\n%s" % (status, source, destination, self.hexDump(data)))


if __name__ == "__main__":
    dev = pyrcpod.devices[0].open()
    MnetLogger(dev).run()
