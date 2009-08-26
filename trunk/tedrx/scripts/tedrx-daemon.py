#!/usr/bin/env python
"""
Listener daemon for the serial tedrx receiver.

usage: tedrx-daemon.py <datadir>

For each data channel, this daemon updates a corresponing rrd file
with the history of that channel, and writes its current state to an
xml file. Channels are named "tedrx-hc<housecode>-volts/kw".

Micah Dowty <micah@navi.cx>
"""

import os
import sys
import time
import serial
import rrdtool


class TedReceiver:
    def __init__(self, port="/dev/ttyUSB0"):
        self.port = serial.Serial(port, 9600)

    def read(self):
        """Wait for the next packet, and return it as a dictionary."""

        while True:
            line = self.port.readline()
            if not line:
                raise IOError("Disconnected")
            line = line.strip()
            if line:
                p = self.decodePacket(line)
                if 'HC' in p and 'KW' in p and 'V' in p:
                    p['timestamp'] = time.time()
                    return p

    def readMultiple(self, timeout=60.0):
        """Read as many packets as we can during 'timeout' seconds."""
        deadline = time.time() + timeout
        packets = []
        while time.time() < deadline:
            packets.append(self.read())
        return packets

    def decodePacket(self, line):
        d = {}
        for token in line.split():
            if '=' in token:
                k, v = token.split('=', 1)

                try:
                    v = int(v)
                except:
                    try:
                        v = float(v)
                    except:
                        pass
        
                d[k] = v
        return d


def updateChannelRRD(dataDir, name, values, lastTS={}):
    filename = os.path.join(dataDir, '%s.rrd' % name)

    if os.path.isfile(filename):
        updates = []
        for ts, v in values:
            ts = int(ts + 0.5)

            # Throw out updates that are less than a second apart.
            if ts != lastTS.get(name, 0):
                lastTS[name] = ts
                updates.append('%s:%s' % (ts, v))

        print updates
        rrdtool.update(filename, *updates)
    
    else:
        rra = []
        for cf in 'AVERAGE', 'MIN', 'MAX':
            rra.extend([
                    "RRA:%s:0.99:1:172800" % cf,
                    "RRA:%s:0.99:60:2880" % cf,
                    "RRA:%s:0.5:420:2880" % cf,
                    "RRA:%s:0.5:1860:2880" % cf,
                    "RRA:%s:0.5:21900:2880" % cf,
                    ])
        rrdtool.create(filename,
                       "DS:value:GAUGE:120:U:U",
                       "-s 1", *rra)


def updateChannelXML(dataDir, name, latestValue):
    open(os.path.join(dataDir, '%s.xml' % name), 'w').write(
        "<tedrx-data>\n\t<timestamp>%s</timestamp>\n\t<value>%s</value>\n</tedrx-data>\n"
        % latestValue)


def main(dataDir):
    rx = TedReceiver()
    while True:
        # Read a bundle of packets, and resort them into
        # data points for each channel.

        packets = rx.readMultiple()
        channels = {}
        for packet in packets:
            hc = packet['HC']
            if 0:
                # XXX: Voltage temporarily disabled, it's causing floating point exceptions in rrdtool
                channels.setdefault("tedrx-hc%03d-volts" % hc, []).append((packet['timestamp'],
                                                                           packet['V']))
            channels.setdefault("tedrx-hc%03d-kw" % hc, []).append((packet['timestamp'],
                                                                    packet['KW']))

        # Update each channel, processing multiple packets at once to
        # save disk wear.

        for channel, values in channels.iteritems():
            updateChannelRRD(dataDir, channel, values)
            updateChannelXML(dataDir, channel, values[-1])


if __name__ == "__main__":
    if len(sys.argv) == 2:
        main(sys.argv[1])
    else:
        print __doc__
        sys.exit(1)
