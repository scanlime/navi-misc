""" RioKarma

This is a Python package for communicating with the Rio Karma portable
audio player. Currently it only supports the TCP/IP protocol, not USB.

Besides the 'Pearl' protocol used by the Rio, this includes higher-level
abstractions for keeping a local cache of the device's filesystem,
querying that cache, and reading/writing files on the actual device.

This whole package would have been much harder to implement without the
great reverse-engineering work already done by the Karmalib and libkarma
projects:

    http://karmalib.sourceforge.net/docs/protocol.php
    http://www.freakysoft.de/html/libkarma/

Requires Twisted. For downloading files to the Rio Karma, requires
mmpython to extract metadata from those files.

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

def connect(host, port=8302, password=''):
    """Connect to a Rio Karma device. Returns a Deferred that results in
       an AuthenticatedProtocol instance on success.
       """
    from twisted.internet import reactor
    from RioKarma.Authenticated import Factory

    factory = Factory(password)
    reactor.connectTCP(host, port, factory)
    return factory.authResult

### The End ###
