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

import Message, XML
import re, os


class ColortextToIRC(Message.Formatter):
    """Converts messages with colorText content to plain text
       with IRC color tags.
       """
    detector = Message.Filter('<find path="/message/body/colorText"/>')
    medium = 'irc'
    def __init__(self):
        import IRC
        self.formatter = IRC.ColortextFormatter()

    def format(self, message, input=None):
        return self.formatter.format(message.xml.body.colorText)


class CommitFormatter(Message.Formatter):
    """Base class for formatters that operate on commit messages.
       Includes a detector for commit messages, and utilities for
       extracting useful information from the commits.
       """
    detector = Message.Filter('<find path="/message/body/commit"/>')

    def consolidateFiles(self, xmlFiles):
        """Given a <files> element, find the directory common to all files
           and return a 2-tuple with that directory followed by
           a list of files within that directory.
           """
        files = []
        if xmlFiles:
            for fileTag in xmlFiles.elements():
                if fileTag.name == 'file':
                    files.append(str(fileTag))

        # Start with the prefix found by os.path.commonprefix,
        # then actually make it end with a directory rather than
        # possibly ending with part of a filename.
        prefix = re.sub("[^/]*$", "", os.path.commonprefix(files))

        endings = []
        for file in files:
            endings.append(file[len(prefix):])
        return prefix, endings

    def format(self, message, input=None):
        """Break the commit message up into pieces that are each formatted with
           one of our format_* member functions.
           """
        commit = message.xml.body.commit
        metadata = []
        if commit.author:
            metadata.append(self.format_author(str(commit.author)))
        if commit.revision:
            metadata.append(self.format_revision(str(commit.revision).strip()))
        if commit.files:
            metadata.append(self.format_files(commit.files))
        return self.joinMessage(metadata, self.format_log(str(commit.log)))

    def joinMessage(self, metadata, log):
        """Join a list of formatted metadata and a formatted log message
           to form a final formatted commit.
           """
        return "%s: %s" % (" ".join(metadata), log)

    def format_default(self, str):
        """A hook for formatting that should be applied to all text"""
        return str

    def format_files(self, files):
        """Break up our list of files into a common prefix and a sensibly-sized
           list of filenames after that prefix.
           """
        prefix, endings = self.consolidateFiles(files)
        endingStr = " ".join(endings)
        if len(endingStr) > 20:
            endingStr = "%d files" % len(endings)
        if endingStr:
            return self.format_default("%s (%s)" % (prefix, endingStr))
        else:
            return self.format_default(prefix)

    def format_log(self, logString):
        return self.format_default(logString)

    def format_author(self, author):
        return self.format_default(author)

    def format_revision(self, rev):
        return self.format_default('r' + rev)


class CommitToIRC(CommitFormatter):
    """Converts commit messages to plain text with IRC color tags"""
    medium = 'irc'

    def format_author(self, author):
        import IRC
        return IRC.format(CommitFormatter.format_author(self, author), 'green')

    def format_revision(self, rev):
        import IRC
        return 'r' + IRC.format(str(rev), 'bold')

    def format_files(self, files):
        import IRC
        return IRC.format(CommitFormatter.format_files(self, files), 'bold')

    def join_message(self, metadata, log):
        import IRC
        return "%s%s %s" % (" ".join(metadata), IRC.format(':', 'bold'), log)


class CommitToXHTML(CommitFormatter):
    """Converts commit messages to XHTML"""
    medium = 'xhtml'

    def format_default(self, obj):
        return XML.domish.escapeToXml(obj)


class ColortextToXHTML(Message.Formatter):
    """Converts messages with colorText content to XHTML
       with colors represented by CSS 'class' attributes
       on <span> tags, and with bold and underline converted
       to <b> and <u> tags.
       """
    detector = Message.Filter('<find path="/message/body/colorText"/>')
    medium = 'xhtml'

    def format(self, message, input=None):
        return self.Parser(message.xml.body.colorText).result

    class Parser(XML.XMLObjectParser):
        requiredRootElement = 'colorText'

        def element_colorText(self, element):
            """Parse all child elements and glue together the resulting strings"""
            return ''.join([self.parse(e) for e in element.children])

        def parseString(self, s):
            """Quote strings and pass them straight through"""
            return XML.domish.escapeToXml(s)

        def element_b(self, element):
            return "<b>" + self.element_colorText(element) + "</b>"

        def element_u(self, element):
            return "<u>" + self.element_colorText(element) + "</u>"

        def element_br(self, element):
            return "<br/>"

        def colorQuote(self, color):
            """Make a color name safe for inclusion into a class attribute.
               This just replaces any non-alphabetical characters with hyphens.
               """
            return re.sub("[^a-zA-Z]", "-", color)

        def element_color(self, element):
            """Convert the fg and bg attributes, if we have them, to <span> tags"""
            s = self.element_colorText(element)
            if element.hasAttribute('fg'):
                s = '<span class="fgColor-%s">%s</span>' % (self.colorQuote(element['fg']), s)
            if element.hasAttribute('bg'):
                s = '<span class="bgColor-%s">%s</span>' % (self.colorQuote(element['bg']), s)
            return s


class IRCProjectName(Message.Formatter):
    """Prepends the project name to each line of the input message, boldinated for IRC"""
    medium = 'irc'
    def format(self, message, input):
        if not input:
            return
        if message.xml.source and message.xml.source.project:
            import IRC
            prefix = IRC.format("%s:" % message.xml.source.project, 'bold') + " "
            return "\n".join([prefix + line for line in input.split("\n")])
        else:
            return input

### The End ###
