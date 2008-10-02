#!/usr/bin/env python
#
# This is a Python module for The Energy Detective: A low-cost whole
# house energy monitoring system. For more information on TED, see
# http://theenergydetective.com
#
# This module was not created by Energy, Inc. nor is it supported by
# them in any way. It was created using information from two sources:
# David Satterfield's TED module for Misterhouse, and my own reverse
# engineering from examining the serial traffic between TED Footprints
# and my RDU.
#
# I have only tested this module with the model 1001 RDU, with
# firmware version 9.01U. The USB port is uses the very common FTDI
# USB-to-serial chip, so the RDU will show up as a serial device on
# Windows, Mac OS, or Linux.
#
# The most recent version of this module can be obtained at:
#   http://svn.navi.cx/misc/trunk/python/ted.py
#
# Copyright (c) 2008 Micah Dowty <micah@navi.cx>
# 
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
# THE SOFTWARE.
#

import serial
import time
import binascii
import sys
import struct


# Special bytes

PKT_REQUEST = "\xAA"
ESCAPE      = "\x10"
PKT_BEGIN   = "\x04"
PKT_END     = "\x03"

class ProtocolError(Exception):
    pass


class TED(object):
    def __init__(self, device):
        self.port = serial.Serial(device, 19200, timeout=0)
        self.escape_flag = False

        # None indicates that the packet buffer is invalid:
        # we are not receiving any valid packet at the moment.
        self.packet_buffer = None

    def poll(self):
        """Request a packet from the RDU, and flush the operating
           system's receive buffer. Any complete packets we've
           received will be decoded. Returns a list of Packet
           instances.

           Raises ProtocolError if we see anything from the RDU that
           we don't expect.
           """

        # Request a packet. The RDU will ignore this request if no
        # data is available, and there doesn't seem to be any harm in
        # sending the request more frequently than once per second.
        self.port.write(PKT_REQUEST)

        return self.decode(self.port.read(4096))

    def decode(self, raw):
        """Decode some raw data from the RDU. Updates internal
           state, and returns a list of any valid Packet() instances
           we were able to extract from the raw data stream.
           """

        packets = []

        # The raw data from the RDU is framed and escaped. The byte
        # 0x10 is the escape byte: It takes on different meanings,
        # depending on the byte that follows it. These are the
        # escape sequence I know about:
        #
        #    10 10: Encodes a literal 0x10 byte.
        #    10 04: Beginning of packet
        #    10 03: End of packet
        #
        # This code illustrates the most straightforward way to
        # decode the packets. It's best in a low-level language like C
        # or Assembly. In Python we'd get better performance by using
        # string operations like split() or replace()- but that would
        # make this code much harder to understand.

        for byte in raw:
            if self.escape_flag:
                self.escape_flag = False
                if byte == ESCAPE:
                    if self.packet_buffer is not None:
                        self.packet_buffer += ESCAPE
                elif byte == PKT_BEGIN:
                    self.packet_buffer = ''
                elif byte == PKT_END:
                    if self.packet_buffer is not None:
                        packets.append(Packet(self.packet_buffer))
                        self.packet_buffer = None
                else:
                    raise ProtocolError("Unknown escape byte %r" % byte)

            elif byte == ESCAPE:
                self.escape_flag = True
            elif self.packet_buffer is not None:
                self.packet_buffer += byte

        return packets


class Packet(object):
    """Decoder for TED packets. We use a lookup table to find individual
       fields in the packet, convert them using the 'struct' module,
       and scale them. The results are available in the 'fields'
       dictionary, or as attributes of this object.
       """
    
    # We only support one packet length. Any other is a protocol error.
    _protocol_len = 278

    _protocol_table = (
        # TODO: Fill in the rest of this table.
        #
        # It needs verification on my firmware version, but so far the
        # offsets in David Satterfield's code match mine. Since his
        # code does not handle packet framing, his offsets are 2 bytes
        # higher than mine. These offsets start counting at the
        # beginning of the packet body. Packet start and packet end
        # codes are omitted.

        # Offset,  name,             fmt,     scale
        (82,       'kw_rate',        "<H",    0.0001),
        (108,      'house_code',     "<B",    1),
        (247,      'kw',             "<H",    0.01),
        (251,      'volts',          "<H",    0.1),
        )

    def __init__(self, data):
        self.data = data
        self.fields = {}
        if len(data) != self._protocol_len:
            raise ProtocolError("Unsupported packet length %r" % len(data))

        for offset, name, fmt, scale in self._protocol_table:
            size = struct.calcsize(fmt)
            field = data[offset:offset+size]
            value = struct.unpack(fmt, field)[0] * scale

            setattr(self, name, value)
            self.fields[name] = value


def main():
    t = TED(sys.argv[1])
    while True:
        for packet in t.poll():
            print
            print "%d byte packet: %r" % (
                len(packet.data), binascii.b2a_hex(packet.data))
            print
            for name, value in packet.fields.items():
                print "%s = %s" % (name, value)

        time.sleep(1.0)

if __name__ == "__main__":
    main()
