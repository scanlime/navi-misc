""" LibCIA.Email

Functionality related to Email, of course :)
This includes an IncomingMailParser that converts emails to Messages
and delivers them to the Message.Hub.
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

from Message import Message
import email


class IncomingMailParser:
    """Parses commands from incoming email messages, generating and dispatching
       Message instances to the provided Hub. The parse() method determines
       the nature of the message and directs execution to a command_* method.
       """
    def __init__(self, hub):
        self.hub = hub

    def parseString(self, string):
        """Convert the given string to an email.Message, then parse it"""
        self.parse(email.message_from_string(string))

    def parse(self, message):
        """Given an email.Message instance, determines the command it represents
           (if any) and passes control to it.
           """
        subjectFields = message['Subject'].split(" ")
        command = subjectFields[0]
        args = subjectFields[1:]

        try:
            f = getattr(self, "command_" + command)
        except AttributeError:
            # Unknown command, ignore this mail
            return

        f(message, *args)

    def command_Announce(self, message, project):
        """Old-style announcements."""
        print project

### The End ###
