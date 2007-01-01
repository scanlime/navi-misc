""" LibCIA.IncomingMail

Includes an IncomingMailParser that converts emails to Messages
and delivers them to the Message.Hub.
"""
#
# CIA open source notification system
# Copyright (C) 2003-2007 Micah Dowty <micah@navi.cx>
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

from Message import Message
from ColorText import ColorTextParser
import XML, RpcServer
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


class MailInterface(RpcServer.Interface):
    """An XML-RPC interface for delivering messages via an IncomingMailParser
       to the Message.Hub.
       """
    def __init__(self, hub):
        self.hub = hub
        RpcServer.Interface.__init__(self)

    def xmlrpc_deliver(self, message):
        """Given the raw text of an email message, log it and process it if applicable."""
        parsed = IncomingMailParser().parseString(message)
        if parsed:
            self.hub.deliver(parsed)


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
        # No subject, ignore this mail
        if not subject:
            return None
        subject = subject.strip()
        if not subject:
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
        if not XML.dig(xml, "message", "generator"):
            xml.documentElement.appendChild(self.getLocalGenerator(xml))
        generator = XML.dig(xml, "message", "generator")

        # Delete an existing <mailHeaders>
        for child in list(XML.getChildElements(generator)):
            if child.nodeName == "mailHeaders":
                generator.removeChild(child)

        # Add a new <mailHeaders>
        generator.appendChild(self.getXMLMailHeaders(xml))
        return xml

    def getLocalGenerator(self, document):
        """Return a <generator> tag for messages produced locally"""
        node = document.createElementNS(None, "generator")
        XML.addElement(node, "name", content="CIA IncomingMailParser")
        return node

    def getXMLMailHeaders(self, document):
        """Return a <mailHeaders> tag representing a subset of the headers
           for this message. This is placed in the <generator> tag of any
           message passing through this module, to document and log the
           message's true source.
           """
        node = document.createElementNS(None, "mailHeaders")
        for name, value in self.message.items():
            if name in interestingHeaders:
                XML.addElement(node, "header", content=str(value)).setAttributeNS(None, 'name', name)
        return node

    def command_Announce(self, project):
        """Old-style announcements: Announce <project> in the subject line.
           The body of the email contained the message's text, marked up
           with {color} tags but with no metadata.
           """
        xml = XML.createRootNode()

        # Convert the given project name to a <project> tag inside <source>,
        # after filtering it a bit... in the old CIA project names and IRC channel
        # names weren't particularly distinct, so preceeding "#" characters on
        # projects were ignored. We preserve this behaviour.
        if project[0] == "#":
            project = project[1:]
        XML.buryValue(xml, project, "message", "source", "project")

        # Since old-style commits didn't have any metadata, the best we can do
        # is to represent the log in a <colorText> element
        colorText = ColorTextParser().parse(self.message.get_payload()).documentElement
        XML.bury(xml, "message", "body").appendChild(xml.importNode(colorText, True))
        return xml

    def command_DeliverXML(self):
        """Deliver a message already formatted in XML"""
        return XML.parseString(self.message.get_payload())


### The End ###
