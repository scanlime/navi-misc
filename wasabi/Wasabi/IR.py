""" Wasabi.IR

An lircd client that handles dispatching IR events to input handlers.
"""
#
# Wasabi Project
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

from BZEngine import Event
import socket, os


class LircError(Exception):
    """An error in communication with lircd"""
    pass

class LircTransmitError(Exception):
    """An error in communication with lircd, while transmitting"""
    pass


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
       is pressed. The button name and remote are all optional-
       None is treated as a wildcard. This uses the default IR
       client. If an IR device has not been successfully
       initialized, this event never fires.

       Note that the 'viewport' parameter is not required, but is
       accepted for compatibility with events in the Input module.

       The 'repeat' parameter can take on several types of values:
          - If it is None, this event only fires on the first press of the button
          - If it is an integer, this event only fires every n'th codes received
          - If it is a 2-tuple (m,n), the first code received always generates an event,
            then the next 'm' codes generate no event, then every n'th code generates
            an event. This is similar to normal keyboard repeat.
       """
    def __init__(self, viewport=None, name=None, remote="wasabi", repeat=(7,2)):
        global defaultClient
        Event.Event.__init__(self)
        self.name = name
        self.remote = remote
        self.repeat = repeat
        if defaultClient:
            defaultClient.onReceivedCode.observe(self.handleCode)

    def handleCode(self, code):
        if self.name is not None and self.name != code.name:
            return
        if self.remote is not None and self.remote != code.remote:
            return
        if self.repeat is None:
            # only trigger on the first received code
            if code.repeat != 0:
                return
        elif type(self.repeat) == type(()) or type(self.repeat) == type([]):
            # keyboard-style repeat
            if code.repeat != 0 and (code.repeat < self.repeat[0] or code.repeat % self.repeat[1]):
                return
        else:
            # repeat rate divisor
            if code.repeat % self.repeat:
                return

        self()


class Transmitter:
    """A simple class for transmitting IR codes via lircd. Doesn't require
       an event loop. Runs on a separate socket so this doesn't have to be
       nonblocking, and so the responses from lircd don't get mixed in
       with the received codes.
       """
    def __init__(self, socketName="/dev/lircd"):
        # Open the lircd connection
        self.socket = socket.socket(socket.AF_UNIX, socket.SOCK_STREAM)
        self.socket.connect(socketName)
        self.socketf = os.fdopen(self.socket.fileno())

    def sendStart(self, remote, codeName):
        """Start sending a code repeatedly"""
        self.write("send_start %s %s\n" % (remote, codeName))

    def sendStop(self, remote, codeName):
        """Stop sending a code started with sendStart"""
        self.write("send_stop %s %s\n" % (remote, codeName))

    def sendOnce(self, remote, codeName):
        """Send a code only once"""
        self.write("send_once %s %s\n" % (remote, codeName))

    def write(self, command):
        """Write a preformatted command line to lircd"""
        os.write(self.socket.fileno(), command)

        # Read the response packet lircd sends back, everything between BEGIN and END
        response = []
        while self.socketf.readline().strip() != "BEGIN":
            pass
        while True:
            line = self.socketf.readline().strip()
            if line == "END":
                break
            response.append(line)

        if response[0].strip() != command.strip():
            raise LircTransmitError("Command was not echoed")

        if response[1] != "SUCCESS":
            raise LircTransmitError(response[-1])

### The End ###
