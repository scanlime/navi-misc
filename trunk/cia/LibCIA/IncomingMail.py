""" LibCIA.IncomingMail

Includes an IncomingMailParser that converts emails to Messages
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

from twisted.xish import domish
from Message import Message
from ColorText import ColorTextParser
import email


class IncomingMailParser:
    """Parses commands from incoming email messages, generating an XML Message
       object representing its contents. This returned object can be dispatched
       by a Message.Hub.
       """
    def parseString(self, string):
        """Convert the given string to an email.Message, then parse it"""
        self.parse(email.message_from_string(string))

    def parse(self, message):
        """Given an email.Message instance, determines the command it represents
           (if any) and passes control to it.
           """
        self.message = message
        subjectFields = message['Subject'].split(" ")
        command = subjectFields[0]
        args = subjectFields[1:]

        try:
            f = getattr(self, "command_" + command)
        except AttributeError:
            # Unknown command, ignore this mail
            return None

        # Pass on control to the command_* function...
        return f(*args)

    def command_Announce(self, project):
        """Old-style announcements: Announce <project> in the subject line.
           The body of the email contained the message's text, marked up
           with {color} tags but with no metadata.
           """
        xml = domish.Element((None, "message"))

        # Convert the given project name to a <project> tag inside <source>,
        # after filtering it a bit... in the old CIA project names and IRC channel
        # names weren't particularly distinct, so preceeding "#" characters on
        # projects were ignored. We preserve this behaviour.
        if project[0] == "#":
            project = project[1:]
        xml.addElement("source").addElement("project", content=project)

        # Since old-style commits didn't have any metadata, the best we can do
        # is to represent the log in a <colorText> element
        colorText = ColorTextParser().parse(self.message.get_payload())
        xml.addElement("body").addChild(colorText)

        print xml.toXml()

### The End ###
