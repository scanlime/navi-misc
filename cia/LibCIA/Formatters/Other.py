""" LibCIA.Formatters.Other

Formatters that don't fit into the other categories- mostly
formatters that can be referred to by name to modify a message
rather than being automatically applied.
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

from LibCIA import Message, XML


class IRCProjectName(Message.Formatter):
    """Prepends the project name to each line of the input message, boldinated for IRC"""
    medium = 'irc'
    def format(self, args):
        if not args.input:
            return
        project = XML.dig(args.message.xml, "message", "source", "project")
        if project:
            from LibCIA.IRC.Formatting import format
            prefix = format("%s:" % XML.shallowText(project), 'bold') + " "
            return "\n".join([prefix + line for line in args.input.split("\n")])
        else:
            return args.input


class IRCFormat(Message.Formatter):
    """Apply arbitrary IRC formatting to the input. A color name or other
       formatting code may be supplied inside this formatter's XML tag.
       """
    medium = 'irc'
    formattingCode = 'normal'

    def format(self, args):
        if input:
            from LibCIA.IRC.Formatting import format
            return format(args.input, self.formattingCode)

    def loadParametersFrom(self, xml):
        self.formattingCode = XML.shallowText(xml).strip()


### The End ###
