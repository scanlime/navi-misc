""" LibCIA.Formatters.Util

Utilities shared by several formatters
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

from LibCIA import XML


def wrapLine(line, width):
    """Given a long line, wrap it if possible to the given width,
       returning a list of lines.
       """
    lines = []
    newLine = ''
    for word in line.split(" "):
        oldLine = newLine
        if newLine:
            newLine = newLine + ' ' + word
        else:
            newLine = word
        if len(newLine) > width:
            lines.append(oldLine.rstrip())
            newLine = word
    if newLine:
        lines.append(newLine.rstrip())
    return lines


def extractSummary(self, element, widthLimit=80):
    """Extract all text from the given XML element, remove extra
       whitespace, and truncate it to no longer than the given width.
       """
    return wrapLine(re.sub("\s+", " ", XML.allText(element)).strip(), widthLimit)[0]

### The End ###
