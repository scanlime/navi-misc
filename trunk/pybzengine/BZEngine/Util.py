""" BZEngine.Util

Small utilities that don't seem to fit anywhere else :)
"""
#
# Python BZEngine Package
# Copyright (C) 2003-2004 Micah Dowty <micahjd@users.sourceforge.net>
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


def getSubclassDict(module, baseClass, keyAttribute, cacheName='contentsDict'):
    """Return a dictionary of all the base classes of baseClass in module,
       keyed by keyAttribute. This dictionary is cached in the module itself
       under the attribute named cacheName.
       """
    if not hasattr(module, cacheName):
        d = {}
        for key in module.__dict__:
            try:
                value = module.__dict__[key]
                if issubclass(value, baseClass):
                    key = getattr(value, keyAttribute, None)
                    if key is not None:
                        d[key] = value
            except TypeError:
                pass
        setattr(module, cacheName, d)
    return getattr(module, cacheName)


def hexDump(src, dest, bytesPerLine=16, wordSize=2):
    """A pretty standard hex dumper routine.
       Dumps the stream 'src' to the stream 'dest'
       """
    addr = 0
    while 1:
        srcLine = src.read(bytesPerLine)
        if not srcLine:
            break

        # Address
        dest.write("%04X: " % addr)
        addr += len(srcLine)

        # Hex values
        for i in xrange(bytesPerLine):
            if i < len(srcLine):
                dest.write("%02X" % ord(srcLine[i]))
            else:
                dest.write("  ")
            if not (i+1) % wordSize:
                dest.write(" ")
        dest.write(" ")

        # ASCII representation
        for byte in srcLine:
            if ord(byte) >= 32 and ord(byte) < 128:
                dest.write(byte)
            else:
                dest.write(".")
        for i in xrange(bytesPerLine - len(srcLine)):
            dest.write(" ")
        dest.write("\n")


class Proxy:
    """Set up a proxy between the server and client.
       Currently this only works correctly when there is one client
       connected to the server. It forwards all events (tcp and udp)
       from one to the other.
       """
    def __init__(self, server, client):
        self.server = server
        self.client = client

        # Let the user know when our client connects, and force
        # our server to hand out the same client ID we were given.
        client.onConnect.observe(self.onClientConnect)

        # Set up events to forward messages between
        # client and server, dumping them to stdout
        client.onUnhandledMessage.replace(self.onClientMessage)
        server.onUnhandledMessage.replace(self.onServerMessage)

    def onClientConnect(self):
        self.server.nextClientID = self.client.id
        self.server.clientIDIncrement = 0

    def onClientMessage(self, msg):
        try:
            self.server.clientsByID[self.client.id].write(msg, msg.protocol)
        except KeyError:
            pass

    def onServerMessage(self, msg):
        getattr(self.client, msg.protocol).write(msg)


def autoFile(name, mode="r"):
    """Given a name which could be a local file, URI, or
       a file-like object, return a file-like object.
       """
    if type(name) == str:
        # If it doesn't look like a URL, don't bother loading urllib2
        if name.find("://") < 0:
            import os
            return open(os.path.expanduser(name), mode)
        else:
            import urllib2
            return urllib2.urlopen(name)
    else:
        return name


def dataFile(name):
    """Given the name of a data file, return its full path.
       Currently this is hardcoded relative to our binary's path.
       """
    import os, sys
    argv0 = sys.argv[0]
    try:
        argv0 = os.path.realpath(argv0)
    except:
        pass
    return os.path.join(os.path.dirname(argv0), 'data', name)

### The End ###
