""" LibCIA.Formatters

A collection of Formatter subclasses that may be referred to by name
elsewhere, for example in IRC filters.
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

import Message


class ColortextToIRC(Message.Formatter):
    """Converts messages with colorText content to plain text
       with IRC color tags.
       """
    detector = '<find path="/message/body/colorText"/>'
    medium = 'irc'
    def __init__(self):
        import IRCColor
        self.formatter = IRCColor.ColortextFormatter()

    def format(self, message, input=None):
        return self.formatter.format(message.xml.body.colorText)


class IRCProjectName(Message.Formatter):
    """Prepends the project name to each line of the input message, boldinated for IRC"""
    medium = 'irc'
    def format(self, message, input):
        if message.xml.source and message.xml.source.project:
            import IRCColor
            prefix = IRCColor.format("%s:" % message.xml.source.project, 'bold') + " "
            return "\n".join([prefix + line for line in input.split("\n")])
        else:
            return input


### The End ###
