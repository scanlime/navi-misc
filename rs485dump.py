#!/usr/bin/env python
#
# Hex dump all data received over RS-485.
# Designed for the rcpod-485 board.
#
# -- Micah Dowty <micah@picogui.org>
#
import pyrcpod, time

baudrate = 9600
bytesPerLine = 16
timeout  = 0.5   # Flush the line buffer and insert a blank line to
                 # separate blocks of incoming data when a pause of
                 # this many seconds occurs.


def dumpLine(line):
    """Given a list of byte values, display them in hex, decimal, and ASCII"""
    for byte in line:
        print "%02X" % byte,
    print


def main():
    dev = pyrcpod.devices[0].open(pyrcpod.Rcpod485)
    dev.serialInit(baudrate)
    dev.serialRxStart()
    line = []
    timestamp = None
    print "Listening at %d baud..." % baudrate

    while True:
        newData = dev.serialRxRead()

        if newData:
            timestamp = time.time()
            line.extend(newData)
            if len(line) >= bytesPerLine:
                # We have a complete line, display it
                dumpLine(line[:bytesPerLine])
                line = line[bytesPerLine:]

        elif timestamp and time.time() > timestamp + timeout:
            # It's been a while since the last data received, flush
            # any partial line we may have and print a blank line
            timestamp = None
            dumpLine(line)
            line = []
            print

if __name__ == "__main__":
    main()
