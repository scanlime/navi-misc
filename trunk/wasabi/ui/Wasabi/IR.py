""" Wasabi.IR

An lircd client that handles dispatching IR events to input handlers.
"""
#
# Wasabi Project
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

from BZEngine import Event
import socket, os


class Code:
    """An object representing an lircd IR code. Can be constructed from a line
       of the form (hex repeat name remote) as sent over the lircd socket.
       """
    def __init__(self, line=None):
        self.hex = None
        self.repeat = None
        self.name = None
        self.remote = None
        if line:
            self.hex, self.repeat, self.name, self.remote = line.strip().split(" ")
            self.repeat = int(self.repeat, 16)

    def __str__(self):
        return "<IR.Code '%s' from remote '%s'>" % (self.name, self.remote)


class Client:
    """An lircd client. Polls /dev/lircd for new IR codes to dispatch"""
    def __init__(self, socketName="/dev/lircd"):
        # The onReceivedCode event is called with a Code instance
        # for every IR code received from the lircd socket
        Event.attach(self, 'onReceivedCode')

        # Open the lircd connection
        self.socket = socket.socket(socket.AF_UNIX, socket.SOCK_STREAM)
        self.socket.connect(socketName)
        self.socketf = os.fdopen(self.socket.fileno())
        self.socket.setblocking(False)

    def attachToLoop(self, eventLoop):
        """Attach this LIRC client to the given event loop, polling at each
           loop iteration for more available IR codes.
           """
        eventLoop.add(Event.ContinuousTimer(self.poll))

    def poll(self):
        # Receive all the codes available now. When there is no more,
        # instead of blocking we'll get an IOError.
        try:
            while True:
                self.onReceivedCode(self.readCode())
        except IOError:
            pass

    def readCode(self):
        """Reads the next IR code from lircd, returning a Code object"""
        return Code(self.socketf.readline())


defaultClient = None

def getDefaultClient():
    """The Client is generally a singleton. This returns the default instance,
       creating it if necessary.
       """
    global defaultClient
    if not defaultClient:
        defaultClient = Client()
    return defaultClient


class ButtonPress(Event.Event):
    """An event that fires when a specified IR remote button
       is pressed. The button name, remote, and repeat number
       are all optional. None is treated as a wildcard.
       This uses the default IR client.
       """
    def __init__(self, viewport, name=None, remote=None, repeat=None):
        Event.Event.__init__(self)
        self.name = name
        self.remote = remote
        self.repeat = repeat
        getDefaultClient().onReceivedCode.observe(self.handleCode)

    def handleCode(self, code):
        if self.name is not None and self.name != code.name:
            return
        if self.remote is not None and self.remote != code.remote:
            return
        if self.repeat is not None and self.repeat != code.repeat:
            return
        self()

### The End ###
