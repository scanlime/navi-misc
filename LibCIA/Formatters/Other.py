""" LibCIA.Formatters.Other

Formatters that don't fit into the other categories- mostly
formatters that can be referred to by name to modify a message
rather than being automatically applied.
"""
#
# CIA open source notification system
# Copyright (C) 2003-2004 Micah Dowty <micahjd@users.sourceforge.net>
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

from LibCIA import Message


class IRCProjectName(Message.Formatter):
    """Prepends the project name to each line of the input message, boldinated for IRC"""
    medium = 'irc'
    def format(self, message, input):
        if not input:
            return
        if message.xml.source and message.xml.source.project:
            from LibCIA.IRC.Formatting import format
            prefix = format("%s:" % message.xml.source.project, 'bold') + " "
            return "\n".join([prefix + line for line in input.split("\n")])
        else:
            return input

### The End ###
