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

Requires Twisted, mmpython, and pysqlite.

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

def getProtocol(host, port=None, password=''):
    """Connect to a Rio Karma device. Returns a Deferred that results in
       an Authenticated.Protocol instance on success.
       """
    from twisted.internet import reactor
    from RioKarma.Authenticated import Factory

    if port is None:
        port = 8302

    factory = Factory(password)
    reactor.connectTCP(host, port, factory)
    return factory.authResult


def getFileManager(host, port=None, password=''):
    """Connect to a Rio Karma device. Returns a Deferred that
       results in a Filesystem.FileManager instance on success.
       """
    from RioKarma.Filesystem import FileManager
    from twisted.internet import defer

    result = defer.Deferred()
    getProtocol(host, port, password).addCallback(
        lambda protocol: result.callback(FileManager(protocol))
        ).addErrback(result.errback)
    return result


def connect(host, port=None, password=''):
    """Do everything necessary to establish a useful connection
       to the Rio Karma. Currently this includes creating a FileManager
       object and synchronizing its database.
       """
    from twisted.internet import defer

    result = defer.Deferred()

    def gotFileManager(fm):
        fm.synchronize().addCallback(
            synchronized, fm).addErrback(result.errback)

    def synchronized(retval, fm):
        result.callback(fm)

    getFileManager(host, port, password).addCallback(
        gotFileManager).addErrback(result.errback)

    return result


def autoConnect(keychain=None, service="urn:empeg-com:protocol2"):
    """Look for a rio karma device using SSDP, then look for
       a corresponding password with the given Keychain instance.
       If no keychain is provided, this automatically uses
       a ConsoleKeychain.
       """
    from RioKarma import SSDP, Authenticated, Pearl
    from twisted.internet import defer

    if keychain is None:
        keychain = Authenticated.ConsoleKeychain()
        keychain.open()

    result = defer.Deferred()

    def foundService((usn, (host, port))):
        keychain.lookup(usn).addCallback(
            gotPassword, host, port, usn).addErrback(
            result.errback)

    def connectionError(err, host, port, usn):
        if isinstance(err.value, Pearl.AuthenticationError):
            # Bad password, let the user try again
            keychain.lookup(usn, ignoreStored=True).addCallback(
                gotPassword, host, port, usn).addErrback(
                connectionError, host, port, usn)
        else:
            # Pass on other errors
            result.errback(err)

    def gotPassword(password, host, port, usn):
        connect(host, port, password).addCallback(
            result.callback).addErrback(
            connectionError, host, port, usn)

    SSDP.findService(service).addCallback(
        foundService).addErrback(result.errback)
    return result

### The End ###
