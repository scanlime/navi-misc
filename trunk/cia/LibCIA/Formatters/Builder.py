""" LibCIA.Formatters.Builder

Formatters for converting builder messages to other formats.
Builder messages contain a set of success/failure results and messages
from some automated build or test process.
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

__all__ = ['BuilderToPlaintext', 'BuilderToIRC']


class BuilderFormatter(Message.Formatter):
    """Abstract base class for formatters that operate on builder results."""
    detector = Message.Filter('<find path="/message/body/builder"/>')

    def format(self, message, input=None):
        # Format each package inside each result set
        packages = []
        for results in message.xml.body.builder.elements():
            if results.name == 'results':
                for package in results.elements():
                    if package.name == 'package':
                        packages.append(self.format_package(package))
        return self.joinMessage(message, packages)

    def format_package(self, package):
        """Format the results associated with one package,
           given the XML <package> element
           """
        pass

    def format_results(self, package):
        """Given a package, returns a formatted representation of all results for that package"""
        results = []
        for element in package.elements():
            f = getattr(self, 'result_' + element.name, None)
            if f:
                results.append(f(element))

    def joinMessage(self, message, packages):
        """Join the results for each package into a final result"""
        content = "builder"
        if message.xml.source.branch:
            content += " " + self.format_branch(str(message.xml.source.branch).strip())

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
        return "%s (%s)" % (package.getAttribute('name'), package.getAttribute('arch'))


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
        return "%s (%s)" % (package.getAttribute('name'),
                            self.colorFormatter(package.getAttribute('arch'), 'green'))

### The End ###
