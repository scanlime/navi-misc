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

from twisted.web.xmlrpc import XMLRPC
from twisted.xish import domish
from Message import Message
from ColorText import ColorTextParser
import email


# List of email headers worth logging.
# This should cover all those that are really interesting
# from a security or identity point of view. Most other headers
# are redundant with other information included in the message.
interestingHeaders = (
    'From',
    'Received',
    'Message-Id',
    'Date'
    )


class MailInterface(XMLRPC):
    """An XML-RPC interface for delivering messages via an IncomingMailParser
       to the Message.Hub.
       """
    def __init__(self, hub):
        self.hub = hub

    def xmlrpc_deliver(self, message):
        """Given the raw text of an email message, log it and process it if applicable"""
        xmlMessage = IncomingMailParser().parseString(message)
        if xmlMessage:
            self.hub.deliver(xmlMessage)
        return True


class IncomingMailParser:
    """Parses commands from incoming email messages, generating an XML Message
       object representing its contents. This returned object can be dispatched
       by a Message.Hub.
       """
    def parseString(self, string):
        """Convert the given string to an email.Message, then parse it"""
        return self.parse(email.message_from_string(string))

    def parse(self, message):
        """Given an email.Message instance, determines the command it represents
           (if any) and passes control to it.
           """
        self.message = message
        subject = message['Subject']
        if not subject:
            # No subject, ignore this mail
            return None

        # The subject line is formatted like a simple command line
        subjectFields = subject.split(" ")
        command = subjectFields[0]
        args = subjectFields[1:]

        try:
            f = getattr(self, "command_" + command)
        except AttributeError:
            # Unknown command, ignore this mail
            return None

        # Pass on control to the command_* function...
        xml = f(*args)

        # If the command generated a message, perform some common postprocessing
        if xml:
            return Message(self.postprocessMessage(xml))

    def postprocessMessage(self, xml):
        """Gets a chance to modify all XML messages before they're loaded
           and dispatched to the Hub. This does the following:
             - If there is no <generator> at all, adds a generic one
             - Removes any <mailHeaders> tag that may already exist in <generator>
             - Adds a correct <mailHeaders> tag to the <generator>
           """
        # Create the <generator> tag if it doesn't exist
        if not xml.generator:
            xml.addChild(self.getLocalGenerator())
        generator = xml.generator

        # Delete an existing <mailHeaders>
        for i in xrange(len(generator.children)):
            if generator.children[i].name == "mailHeaders":
                del generator.children[i]

        # Add a new <mailHeaders>
        generator.addChild(self.getXMLMailHeaders())
        return xml

    def getLocalGenerator(self):
        """Return a <generator> tag for messages produced locally"""
        xml = domish.Element((None, "generator"))
        xml.addElement("name", content="CIA IncomingMailParser")
        return xml

    def getXMLMailHeaders(self):
        """Return a <mailHeaders> tag representing a subset of the headers
           for this message. This is placed in the <generator> tag of any
           message passing through this module, to document and log the
           message's true source.
           """
        xml = domish.Element((None, "mailHeaders"))
        for name, value in self.message.items():
            if name in interestingHeaders:
                xml.addElement("header", content=str(value))['name'] = name
        return xml

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
        return xml

### The End ###
