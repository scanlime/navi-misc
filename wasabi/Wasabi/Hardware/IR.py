""" Wasabi.Hardware.IR

This module implements Twisted protocols and convenience classes
for talking to lircd. With it, you can send and receive IR codes
under Linux with any lirc-supported hardware.
"""
#
# Wasabi Project
# Copyright (C) 2003-2004 Micah Dowty and David Trowbridge
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

from twisted.protocols import basic


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


class CodeReceiver(basic.LineOnlyReceiver):
    """The protocol used to talk to lircd. lircd sends lines of ASCII text
       with information about each code. This decodes the received data and
       invokes codeReceived with it.
       """
    def lineReceived(self, line):
        self.codeReceived(self, Code(line))

    def codeReceived(self, code):
        print code


### The End ###
