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
    def __init__(self, eventLoop, socketName="/dev/lircd"):
        # The onReceivedCode event is called with a Code instance
        # for every IR code received from the lircd socket
        Event.attach(self, 'onReceivedCode')

        # Open the lircd connection
        self.socket = socket.socket(socket.AF_UNIX, socket.SOCK_STREAM)
        self.socket.connect(socketName)
        self.socketf = os.fdopen(self.socket.fileno())
        self.socket.setblocking(False)

        # Add ourselves to the main loop. It uses our getSelectable method.
        eventLoop.add(self)

    def getSelectable(self):
        """Called by the main loop to see what items we have for its select()"""
        return self.socket

    def needsWrite(self):
        """Called by the main loop to see if we need to poll for writing.
           We don't, since we only read from lircd.
           """
        return False

    def pollRead(self, eventLoop):
        """Receive all the codes available now. When there is no more,
           instead of blocking we'll get an IOError. This is called by
           the main loop when it detects activity on our socket.
           """
        try:
            while True:
                self.onReceivedCode(self.readCode())
        except IOError:
            pass

    def readCode(self):
        """Reads the next IR code from lircd, returning a Code object"""
        return Code(self.socketf.readline())


defaultClient = None

def initDefaultClient(eventLoop):
    """Initialize the singleton default client. Requires an event loop.
       """
    global defaultClient
    defaultClient = Client(eventLoop)


class ButtonPress(Event.Event):
    """An event that fires when a specified IR remote button
       is pressed. The button name, remote, and repeat number
       are all optional. None is treated as a wildcard.
       This uses the default IR client. If an IR device has not
       been successfully initialized, this event never fires.
       """
    def __init__(self, viewport, name=None, remote=None, repeat=0):
        global defaultClient
        Event.Event.__init__(self)
        self.name = name
        self.remote = remote
        self.repeat = repeat
        defaultClient.onReceivedCode.observe(self.handleCode)

    def handleCode(self, code):
        if self.name is not None and self.name != code.name:
            return
        if self.remote is not None and self.remote != code.remote:
            return
        if self.repeat is not None and self.repeat != code.repeat:
            return
        self()

### The End ###
