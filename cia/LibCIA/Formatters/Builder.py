""" LibCIA.Formatters.Builder

Formatters for converting builder messages to other formats.
Builder messages contain a set of success/failure results and messages
from some automated build or test process.
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
import Nouvelle

__all__ = ['BuilderToPlaintext', 'BuilderToIRC', 'BuilderToXHTML']


class BuilderFormatter(Message.Formatter):
    """Abstract base class for formatters that operate on builder results."""
    filter = '<find path="/message/body/builder"/>'

    def format(self, args):
        # Format each package inside each result set
        packages = []
        for results in XML.getChildElements(XML.dig(args.message.xml, "message", "body", "builder")):
            if results.nodeName == 'results':
                for package in XML.getChildElements(results):
                    if package.nodeName == 'package':
                        packages.append(self.format_package(package))
        return self.joinMessage(args.message, packages)

    def format_package(self, package):
        """Format the results associated with one package,
           given the XML <package> element
           """
        pass

    def format_results(self, package):
        """Given a package, returns a formatted representation of all results for that package"""
        results = []
        for element in XML.getChildElements(package):
            f = getattr(self, 'result_' + element.nodeName, None)
            if f:
                results.append(f(element))

    def joinMessage(self, message, packages):
        """Join the results for each package into a final result"""
        content = "builder"

        branch = XML.digValue(message.xml, str, "message", "source", "branch")
        if branch:
            content += " " + self.format_branch(branch.strip())

        # If we have only one package, put it on the same line as the heading
        if len(packages) <= 1:
            content += ": " + packages[0]
        else:
            content += "\n" + "\n".join(packages)
        return content

    def format_branch(self, branch):
        return branch


class BuilderToPlaintext(BuilderFormatter):
    """Converts builder messages to plain text"""
    medium = 'plaintext'

    def format_package(self, package):
        return "%s (%s)" % (package.getAttributeNS(None, 'name'), package.getAttributeNS(None, 'arch'))


class BuilderToXHTML(BuilderFormatter):
    """Converts builder messages to plain text"""
    medium = 'xhtml'

    def format_package(self, package):
        return "%s (%s)" % (package.getAttributeNS(None, 'name'), package.getAttributeNS(None, 'arch'))

    def joinMessage(self, message, packages):
        content = []

        branch = XML.digValue(message.xml, str, "message", "source", "branch")
        if branch:
            content.append(Nouvelle.tag('strong')[ self.format_branch(branch.strip()) ])

        for package in packages:
            if content:
                content.append(Nouvelle.tag('br'))
            content.append(package)

        return content


class BuilderToIRC(BuilderFormatter):
    """Converts builder messages to IRC colorized text"""
    medium = 'irc'

    def __init__(self):
        """By default, use the IRC color formatter"""
        from LibCIA.IRC.Formatting import format
        self.colorFormatter = format

    def format_branch(self, branch):
        return self.colorFormatter(branch, 'orange')

    def format_package(self, package):
        return "%s (%s)" % (package.getAttributeNS(None, 'name'),
                            self.colorFormatter(package.getAttributeNS(None, 'arch'), 'green'))

### The End ###
