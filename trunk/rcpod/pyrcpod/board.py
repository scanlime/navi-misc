""" rcpod.board

Contains subclasses of RcpodDevice that implement special features
of particular boards built around the rcpod. Construct these classes
with an AvailableRcpod instance, or pass them as an argument to
AvailableRcpod's open() method.
"""
#
# Remote Controlled PIC of Doom
# Copyright (C) 2003 Micah Dowty <micahjd@users.sourceforge.net>
#
#  This library is free software; you can redistribute it and/or
#  modify it under the terms of the GNU Lesser General Public
#  License as published by the Free Software Foundation; either
#  version 2.1 of the License, or (at your option) any later version.
#
#  This library is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
#  Lesser General Public License for more details.
#
#  You should have received a copy of the GNU Lesser General Public
#  License along with this library; if not, write to the Free Software
#  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
#

import device, time

# These are the symbols that will be pulled from this module
# into the 'pyrcpod' package.
__all__ = ['Rcpod485']


class MnetError(Exception):
    """A communications error on the RS-485 'micronet'"""
    pass


class Rcpod485(device.OpenedRcpod):
    """Implements the special features of the rcpod-485 board.
       This board includes two LEDs and an RS-485 transceiver.
       """
    def __init__(self, *args, **kwargs):
        device.OpenedRcpod.__init__(self, *args, **kwargs)

        # Set up LEDs
        self.led1 = self.rc2
        self.led2 = self.rc1
        self.led1.output().assert_()
        self.led2.output().assert_()

        # Set up transmit enable pin
        self.txe = self.rd4
        self.txe.output().assert_()
        self.serialSetTxEnable(self.txe)

    def mnetSend(self, data, source=10, destination=2, timeout=0.2):
        """Sends a packet in the Citizen Explorer 'micronet' format,
           receives and validates a response, and returns the data.
           All data is expressed as a list of byte values.

           This will wait 'timeout' seconds for the response.

           Anatomy of a micronet packet:
            - First byte is a destination device ID
            - Second byte is a source device ID
            - Third byte is the number of following data bytes
            - Data bytes
            - An 8-bit checksum byte such that all bytes in the packet
              add up to 255, modulo 256
           """
        # Assemble the packet, calculating a checksum
        packet = [destination, source, len(data)] + list(data)
        csum = 0xFF
        for byte in packet:
            csum = (csum - byte) & 0xFF
        packet.append(csum)

        # Set the baud rate and send our packet, receiving
        # up to the maximum number of bytes our scratchpad can hold
        self.serialInit(9600)
        self.serialTxRxStart(packet, self.scratchpadSize)
        time.sleep(timeout)
        retPacket = self.serialRxFinish()

        # Make a string describing both the sent and the received packets,
        # to be used in case of error.
        pktInfo = "[%s] -> [%s]" % (
            " ".join(["%02X" % byte for byte in packet]),
            " ".join(["%02X" % byte for byte in retPacket]))

        # Validate the received packet
        if not retPacket:
            raise MnetError("No response: %s" % pktInfo)
        if len(retPacket) < 4:
            raise MnetError("Packet too short: %s" % pktInfo)
        if retPacket[0] != source or retPacket[1] != destination:
            raise MnetError("Incorrect source/destination: %s" % pktInfo)
        if len(retPacket) > retPacket[2]+4:
            raise MnetError("Received too many bytes: %s" % pktInfo)
        if len(retPacket) < retPacket[2]+4:
            raise MnetError("Received too few bytes: %s" % pktInfo)
        csum = 0
        for byte in retPacket:
            csum += byte
        if csum != 255:
            raise MnetError("Incorrect checksum: %s" % pktInfo)

        # Return only the data portion of the packet
        return retPacket[3:-1]

### The End ###

