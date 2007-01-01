""" LibCIA.Formatters.Util

Utilities shared by several formatters
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

from LibCIA import XML
import re


def getCrunchedLog(xml):
    """Given the DOM node for a <log> tag, return the log as
       a string with all groups of one or more whitespace
       characters replaced with a single space.
       """
    return re.sub("\s+", " ", XML.shallowText(xml)).strip()


def getNormalizedLog(xml, tabWidth=8):
    """Given the DOM node for a <log> tag, return a list of
       text lines with whitespace normalized appropriately.
       This strips all whitespace from the right side, and homogeneously
       strips whitespace from the left side as much as possible.
       Leading and trailing blank lines are removed, but internal
       blank lines are not.
       """
    if not xml:
        return []

    lines = []
    maxLeftStrip = None

    for line in XML.shallowText(xml).split("\n"):
        # Expand tabs and strip righthand whitespace
        line = line.replace("\t", " "*tabWidth).rstrip()
        strippedLine = line.lstrip()

        # Blank lines don't count in determining the left strip amount
        if strippedLine:
            # Determine how much we can strip from the left side
            leftStrip = len(line) - len(strippedLine)

            # Determine the maximum amount of space we can strip
            # from the left side homogeneously across the whole text
            if maxLeftStrip is None or leftStrip < maxLeftStrip:
                maxLeftStrip = leftStrip

        # Skip leading blank lines
        if lines or strippedLine:
            lines.append(line)

    # Remove trailing blank lines
    while lines and not lines[-1].strip():
        del lines[-1]

    # Homogeneous left strip
    if maxLeftStrip is None:
        return lines
    else:
        return [line[maxLeftStrip:] for line in lines]


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
            oldLine = oldLine.rstrip()
            if oldLine:
                lines.append(oldLine)
            newLine = word
    if newLine:
        lines.append(newLine.rstrip())
    return lines


def extractSummary(element, widthLimit=80):
    """Extract all text from the given XML element, remove extra
       whitespace, and truncate it to no longer than the given width.
       """
    # Extract all text, eating extra whitespace
    text = re.sub("\s+", " ", XML.allText(element)).strip()

    # Use wrapLine to cleanly break it if possible, but
    # truncate it if necessary- wrapLine will not break words in
    # half if they are longer than the wrap width.
    lines = wrapLine(text, widthLimit)
    if lines:
        summary = lines[0][:widthLimit]
        if len(summary) < len(text):
            summary += "..."
        return summary

### The End ###
