""" LibCIA.IRCColor

Utilities for creating text containing IRC color codes, and converting
XML colorText documents to IRC-friendly strings.
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

import XML
import types
import ColorText


formattingCodes = {
    # Colors
    "black"       : "\x0301",
    "dark blue"   : "\x0302",
    "dark green"  : "\x0303",
    "green"       : "\x0303",
    "red"         : "\x0304",
    "brown"       : "\x0305",
    "purple"      : "\x0306",
    "orange"      : "\x0307",
    "yellow"      : "\x0308",
    "light green" : "\x0309",
    "aqua"        : "\x0310",
    "light blue"  : "\x0311",
    "blue"        : "\x0312",
    "violet"      : "\x0313",
    "grey"        : "\x0314",
    "gray"        : "\x0314",
    "light grey"  : "\x0315",
    "light gray"  : "\x0315",
    "white"       : "\x0316",

    # Other formatting
    "normal"      : "\x0F",
    "bold"        : "\x02",
    "reverse"     : "\x16",
    "underline"   : "\x1F",
    }


def format(text, *codeNames):
    """Apply each formatting code from the given list of code names
       to the given text, returnging a string ready for consumption
       by an IRC client.
       """
    if codeNames:
        return "".join([formattingCodes[codeName] for codeName in codeNames]) + text + formattingCodes['normal']
    else:
        return text


class FormattingCode(object):
    """Represents a formattingCode in a way that can be distinguished from plain text"""
    def __init__(self, name):
        self.name = name
        self.value = formattingCodes[name]

    def __str__(self):
        return self.value


class ColortextFormatter(object):
    r"""Given a domish.Element tree with <colorText>-formatted text
        generate an equivalent message formatted for IRC.

        >>> f = ColortextFormatter()
        >>> f.format(XML.parseString('<colorText><u><b>Hello</b> World</u></colorText>'))
        '\x1f\x02Hello\x0f\x1f World\x0f'

        >>> f.format(XML.parseString(
        ...    "<colorText>" +
        ...        "<color bg='dark blue'><color fg='yellow'>" +
        ...            "<b>hello</b>" +
        ...        "</color></color>" +
        ...        "<u> world</u>" +
        ...    "</colorText>"))
        '\x0302\x16\x0308\x02hello\x0f\x1f world\x0f'

        """
    def format(self, xml):
        """Format the entire given XML tree, returns a string with the finished
           text ready to send to IRC.
           """
        return self.flatten(self.formatElement(xml))

    def formatElement(self, xml, codeStack=[]):
        """Recursively format the given XML tree, passing elements on to their
           respective handlers and letting text fall straight through.
           codeStack is used to keep track of the formatting codes we're 'inside'
           so they can be restored if we have to issue a 'normal' code.
           Returns a list of strings and/or FormattingCode instances.
           """
        if isinstance(xml, XML.domish.Element):
            # It's an element, act on it based on its name
            try:
                f = getattr(self, "element_" + xml.name)
            except AttributeError:
                raise XML.XMLValidityError("Unexpected element %r" % xml.name)
            return f(xml, codeStack)
        elif type(xml) in types.StringTypes:
            # It's a string, pass it through
            return xml
        else:
            raise TypeError("XML elements or strings required")

    def flatten(self, codes):
        """Return a flat string representing the FormattingCode instances and/or strings
           in the given codes list.
           """
        return "".join([str(code) for code in codes])

    def formatChildren(self, xml, codeStack):
        """Format each child of the given node, appending the results"""
        codes = []
        for child in xml.children:
            codes.extend(self.formatElement(child, codeStack))
        return codes

    def element_colorText(self, xml, codeStack):
        """Ignore the root node of the <colorText> document"""
        return self.formatChildren(xml, codeStack)

    def codeWrap(self, xml, codeStack, *codeNames):
        """Wrap the children of the given xml element with the given formatting codes.
           This prepends the code list and appends a 'normal' tag, using codeStack
           to restore any codes we don't want to disable with the 'normal' tag.
           """
        # Apply all the codes we're given
        codes = [FormattingCode(name) for name in codeNames]

        # Insert the children here
        codes.extend(self.formatChildren(xml, codeStack + codes))

        # An important optimization- since we're about to insert new codes
        # for everything in codeStack, remove codes from our 'codes' list until
        # we hit actual text. This prevents sequences from appearing in the output
        # where several codes are applied then immediately erased by a 'normal' code.
        # This also handles optimizing out formatting codes with no children.
        while isinstance(codes[-1], FormattingCode):
            del codes[-1]

        # Now stick on a 'normal' code and the contents of our codeStack
        # to revert the codes we were given.
        codes.append(FormattingCode('normal'))
        codes.extend(codeStack)
        return codes

    def element_b(self, xml, codeStack):
        """Just wrap our contents in a bold tag"""
        return self.codeWrap(xml, codeStack, 'bold')

    def element_u(self, xml, codeStack):
        """Just wrap our contents in an underline tag"""
        return self.codeWrap(xml, codeStack, 'underline')

    def element_br(self, xml, codeStack):
        """Insert a literal newline"""
        return "\n"

    def element_color(self, xml, codeStack):
        """Generates formatting codes appropriate to represent a foreground and/or background color"""
        codes = []
        try:
            if xml['bg'] in ColorText.allowedColors:
                codes.append(xml['bg'])
                codes.append('reverse')
            else:
                raise XMLValidityError("%r is not a color" % xml['bg'])
        except KeyError:
            pass
        try:
            if xml['fg'] in ColorText.allowedColors:
                codes.append(xml['fg'])
            else:
                raise XMLValidityError("%r is not a color" % xml['fg'])
        except KeyError:
            pass
        return self.codeWrap(xml, codeStack, *codes)


def _test():
    import doctest, IRCColor
    return doctest.testmod(IRCColor)

if __name__ == "__main__":
    _test()

### The End ###
