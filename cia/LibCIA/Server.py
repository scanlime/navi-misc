""" LibCIA.Server

Collects all the various components of CIA together into one object,
which can then be accessed using the various frontends- XML-RPC, Email,
Perspective Broker, or a combination of those.
"""
#
# CIA open source notification system
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

import Message, IRC

class Backend(object):
    """Holds all the objects that need to be controlled over various interfaces.
       This includes the message hub and IRC bot network.
       """
    def __init__(self):
        self.hub = Message.Hub()
        self.botNet = IRC.BotNetwork()


### The End ###
