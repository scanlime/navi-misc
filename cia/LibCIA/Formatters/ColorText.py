""" LibCIA.Formatters.ColorText

Formatters for converting colorText messages to other formats.
This is the legacy format that old non-XML commits are converted to.
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
import Nouvelle, re
import Util

__all__ = ['ColortextToIRC', 'ColortextTitle', 'ColortextToPlaintext', 'ColortextToXHTML']


class ColortextFormatter(Message.Formatter):
    """Abstract base class for formatters that operate on colorText messages"""
    filter = '<find path="/message/body/colorText"/>'


class ColortextToIRC(ColortextFormatter):
    """Converts messages with colorText content to plain text
       with IRC color tags.
       """
    medium = 'irc'
    color = True

    def param_noColor(self, tag):
        self.color = False

    def __init__(self):
        from LibCIA.IRC.Formatting import ColortextFormatter
        self.formatter = ColortextFormatter()

    def format(self, args):
        colorText = XML.dig(args.message.xml, "message", "body", "colorText")
        if self.color:
            return self.formatter.parse(colorText)
        else:
            return XML.allText(colorText)


class ColortextTitle(ColortextFormatter):
    """Extracts a title from colorText messages"""
    medium = 'title'

    def format(self, args):
        return Util.extractSummary(XML.dig(args.message.xml, "message", "body", "colorText"))


class ColortextToPlaintext(ColortextFormatter):
    """Extracts uncolorized plaintext from colorText messages"""
    medium = 'plaintext'

    def format(self, args):
        return self.Parser(XML.dig(args.message.xml, "message", "body", "colorText")).result

    class Parser(XML.XMLObjectParser):
        requiredRootElement = 'colorText'

        def parseString(self, s):
            return s

        def element_br(self, element):
            return "\n"

        def unknownElement(self, element):
            return ''.join([s for s in self.childParser(element) if s])


class ColortextToXHTML(ColortextFormatter):
    """Converts messages with colorText content to XHTML (using Nouvelle)
       with inline CSS representing the colorText formatting.
       Returns an object that can be serialized into XHTML by a Nouvelle.Serializer.
       """
    medium = 'xhtml'

    def format(self, args):
        colorText = XML.dig(args.message.xml, "message", "body", "colorText")
        return self.Parser(colorText).result

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
            return list(self.childParser(element))

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
                attrValue = element.getAttributeNS(None, attr)
                if attrValue:
                    try:
                        style = "%s%s: %s;" % (style, css, self.colorTable[attrValue])
                    except KeyError:
                        pass
            return Nouvelle.tag('span', style=style)[self.element_colorText(element)]


class ColortextToXHTMLLong(ColortextToXHTML):
    """There isn't much extra we can say about a colortext commit, so put up
       a notice explaining why. This also might be encouragement to upgrade
       old clients that still give us colortext messages.
       """
    medium = 'xhtml-long'

    def format(self, args):
        from LibCIA.Web import Template
        return [            
            Nouvelle.tag('p')[
                ColortextToXHTML.format(self, args),
            ],
            Template.longError[
                "This message was received in CIA's legacy format, 'colorText'. "
                "To see much more detailed information about each commit, "
                "ask this project's administrators to upgrade their client script."],
            ]

### The End ###
