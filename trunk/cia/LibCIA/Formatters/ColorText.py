""" LibCIA.Formatters.ColorText

Formatters for converting colorText messages to other formats.
This is the legacy format that old non-XML commits are converted to.
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

from LibCIA import Message, XML
import Nouvelle, re

__all__ = ['ColortextToIRC', 'ColortextTitle', 'ColortextToPlaintext', 'ColortextToXHTML']


class ColortextFormatter(Message.Formatter):
    """Abstract base class for formatters that operate on colorText messages"""
    detector = Message.Filter('<find path="/message/body/colorText"/>')


class ColortextToIRC(Message.Formatter):
    """Converts messages with colorText content to plain text
       with IRC color tags.
       """
    detector = Message.Filter('<find path="/message/body/colorText"/>')
    medium = 'irc'
    color = True

    def param_noColor(self, tag):
        self.color = False

    def __init__(self):
        from LibCIA.IRC.Formatting import ColortextFormatter
        self.formatter = ColortextFormatter()

    def format(self, message, input=None):
        if self.color:
            return self.formatter.format(message.xml.body.colorText)
        else:
            return XML.allText(message.xml.body.colorText)


class ColortextTitle(ColortextFormatter):
    """Extracts a title from colorText messages"""
    medium = 'title'
    widthLimit = 80

    def format(self, message, input=None):
        # Extract plaintext from the entire message, collapse whitespace, and truncate
        log = re.sub("\s+", " ", XML.allText(message.xml.body.colorText).strip())
        if len(log) > self.widthLimit:
            log = log[:self.widthLimit] + " ..."
        return log


class ColortextToPlaintext(ColortextFormatter):
    """Extracts uncolorized plaintext from colorText messages"""
    medium = 'plaintext'

    def format(self, message, input=None):
        return self.Parser(message.xml.body.colorText).result

    class Parser(XML.XMLObjectParser):
        requiredRootElement = 'colorText'

        def parseString(self, s):
            return s

        def element_br(self, element):
            return "\n"

        def unknownElement(self, element):
            return ''.join([self.parse(e) for e in element.children])


class ColortextToXHTML(ColortextFormatter):
    """Converts messages with colorText content to XHTML (using Nouvelle)
       with inline CSS representing the colorText formatting.
       Returns an object that can be serialized into XHTML by a Nouvelle.Serializer.
       """
    medium = 'xhtml'

    def format(self, message, input=None):
        return self.Parser(message.xml.body.colorText).result

    class Parser(XML.XMLObjectParser):
        requiredRootElement = 'colorText'

        colorTable = {
            'black':        "#000000",
            'dark-blue':    "#0000cc",
            'dark-green':   "#00cc00",
            'green':        "#00cc00",
            'red':          "#cc0000",
            'brown':        "#aa0000",
            'purple':       "#bb00bb",
            'orange':       "#ffaa00",
            'yellow':       "#eedd22",
            'light-green':  "#33d355",
            'aqua':         "#00cccc",
            'light-blue':   "#33eeff",
            'blue':         "#0000ff",
            'violet':       "#ee22ee",
            'grey':         "#777777",
            'gray':         "#777777",
            'light-grey':   "#999999",
            'light-gray':   "#999999",
            'white':        "#FFFFFF",
            }

        def element_colorText(self, element):
            return [self.parse(e) for e in element.children]

        def parseString(self, s):
            return s

        def element_b(self, element):
            return Nouvelle.tag('b')[ self.element_colorText(element) ]

        def element_u(self, element):
            # We can't use <u> here, it's been deprecated and XHTML
            # strict can't contain it. Just use some inline CSS instead.
            return Nouvelle.tag('span', style="text-decoration: underline;")[ self.element_colorText(element) ]

        def element_br(self, element):
            return Nouvelle.tag('br')

        def colorQuote(self, color):
            """Make a color name safe for inclusion into a class attribute.
               This just replaces any non-alphabetical characters with hyphens.
               """
            return re.sub("[^a-zA-Z]", "-", color)

        def element_color(self, element):
            """Convert the fg and bg attributes, if we have them, to <span> tags"""
            style = ''
            for attr, css in (
                ('fg', 'color'),
                ('bg', 'background'),
                ):
                if element.hasAttribute(attr):
                    try:
                        style = "%s%s: %s;" % (style, css, self.colorTable[element[attr]])
                    except KeyError:
                        pass
            return Nouvelle.tag('span', style=style)[self.element_colorText(element)]

### The End ###
