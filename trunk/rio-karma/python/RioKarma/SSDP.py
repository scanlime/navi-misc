""" RioKarma.SSDP

This is a very simple implementation of SSDP (Simple Service Discovery Protocol)
that gives us enough to detect the rio at startup or when it is first
plugged in (coldplug/hotplug)

"""
#
# Copyright (C) 2005 Micah Dowty <micah@navi.cx>
#
#  This program is free software; you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation; either version 2 of the License, or
#  (at your option) any later version.
#
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with this program; if not, write to the Free Software
#  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
#

from twisted.internet import reactor, protocol, defer
import re

class Packet:
    """Represents the data in one SSDP packet"""
    def __init__(self, request=None, data=None, sender=None):
        self.sender = sender
        self.headers = {}
        self.request = request
        if data is not None:
            self.decode(data)

    def encode(self):
        """Return a serialized representation of this packet"""
        lines = [self.request]
        for key, value in self.headers.iteritems():
            lines.append("%s: %s" % (key, value))
        lines.append("")
        return "\r\n".join(lines)

    def decode(self, data):
        """Decode a serialized packet into this instance"""
        lines = data.split("\n")
        self.request = lines[0].strip()
        for line in lines[1:]:
            fields = line.split(":", 1)
            if len(fields) == 2:
                self.headers[fields[0].strip()] = fields[1].strip()


class Protocol(protocol.DatagramProtocol):
    """This protocol sends and receives formatted Packet instances"""
    SSDP_PORT = 1900
    SSDP_ADDR = '239.255.255.250'

    def listen(self):
        self.port = reactor.listenUDP(self.SSDP_PORT, self)

    def listenMulticast(self):
        self.port = reactor.listenMulticast(self.SSDP_PORT, self, self.SSDP_ADDR)
        self.port.joinGroup(self.SSDP_ADDR)

    def datagramReceived(self, data, sender):
        self.packetReceived(Packet(data=data, sender=sender))

    def sendPacket(self, packet):
        self.transport.write(packet.encode(),
                             (self.SSDP_ADDR, self.SSDP_PORT))

    def packetReceived(self, packet):
        """Subclasses should override this to be notified of new packets"""
        pass


class Discovery(Protocol):
    """A bare-bones SSDP server that can discover a service actively
       via normal UDP or passively by listening for multicast packets.
       It invokes a callback with the USN on the discovered device,
       and its address.
       """
    def __init__(self, service, callback):
        self.service = service
        self.callback = callback

    def packetReceived(self, packet):
        if (packet.headers.get('ST') == self.service or
            packet.headers.get('NT') == self.service):
            location = re.match("http://([^:/]+):([0-9]+)(/.*)?",
                                packet.headers.get('LOCATION'))
            address = (location.group(1), int(location.group(2)))
            self.callback(packet.headers.get('USN'), address)

    def search(self):
        p = Packet("M-SEARCH * HTTP/1.1")
        p.headers['Host'] = '%s:%d' % (self.SSDP_ADDR, self.SSDP_PORT)
        p.headers['Man'] = '"ssdp:discover"'
        p.headers['ST'] = self.service
        p.headers['MX'] = 3
        self.sendPacket(p)


def findService(service, timeout=2.0):
    """Look for the given service name on this network. Returns a
       Deferred that yields a (usn, address) tuple on success, or
       None if the service isn't found within the timeout.
       """
    result = defer.Deferred()

    def timedOut():
        p.port.stopListening()
        result.callback(None)

    timer = reactor.callLater(timeout, timedOut)

    def callback(usn, addr):
        timer.cancel()
        p.port.stopListening()
        result.callback((usn, addr))

    p = Discovery(service, callback)
    p.listen()
    p.search()
    return result


if __name__ == "__main__":
    def cb(result):
        print result
    findService("urn:empeg-com:protocol2").addCallback(cb)
    reactor.run()

### The End ###
