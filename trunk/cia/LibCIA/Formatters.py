""" LibCIA.Formatters

A collection of Formatter subclasses that can be searched and
instantiated via the 'factory' object here.
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

import Message, XML, TimeUtil
import Nouvelle
import re, os, posixpath


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
        from IRC.Formatting import ColortextFormatter
        self.formatter = ColortextFormatter()

    def format(self, message, input=None):
        if self.color:
            return self.formatter.format(message.xml.body.colorText)
        else:
            return XML.allText(message.xml.body.colorText)


class CommitFormatter(Message.Formatter):
    """Base class for formatters that operate on commit messages.
       Includes a detector for commit messages, and utilities for
       extracting useful information from the commits.
       """
    detector = Message.Filter('<find path="/message/body/commit"/>')

    # Subclasses can set this to limit the length of log messages, in lines
    lineLimit = None

    # Lines in the log longer than this are wrapped to wrapWidth
    widthLimit = None
    wrapWidth = None

    # If the list of files ends up longer than this many characters, summarize it
    filesWidthLimit = 60

    def param_lineLimit(self, tag):
        self.lineLimit = int(str(tag))

    def param_widthLimit(self, tag):
        self.widthLimit = int(str(tag))
        if self.wrapWidth > self.widthLimit:
            self.wrapWidth = self.widthLimit

    def param_wrapWidth(self, tag):
        self.wrapWidth = int(str(tag))

    def param_filesWidthLimit(self, tag):
        self.filesWidthLimit = int(str(tag))

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

	# If we only have one file, return it as the prefix.
	# This prevents the below regex from deleting the filename
	# itself, assuming it was a partial filename.
	if len(files) == 1:
	    return files[0], []

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
            metadata.append(self.format_author(commit.author))
        if message.xml.source and message.xml.source.branch:
            metadata.append(self.format_branch(message.xml.source.branch))
        metadata.append(self.format_separator())
        if commit.version:
            metadata.append(self.format_version(commit.version))
        if commit.revision:
            metadata.append(self.format_revision(commit.revision))
        metadata.append(self.format_moduleAndFiles(message))
        return self.joinMessage(metadata, self.format_log(commit.log))

    def joinMessage(self, metadata, log):
        """Join a list of formatted metadata and a formatted log message
           to form a final formatted commit.
           """
        return "%s: %s" % (" ".join(metadata), log)

    def format_separator(self):
        """Format an separator that goes between the author + branch and the
           rest of the message, to enhance the message visually.
           """
        return "*"

    def format_moduleAndFiles(self, message):
        """Format the module name and files, joined together if they are both present."""
        if message.xml.body.commit.files:
            formattedFiles = self.format_files(message.xml.body.commit.files)
        else:
            formattedFiles = ""

        if message.xml.source and message.xml.source.module:
            formattedModule = self.format_module(str(message.xml.source.module).strip())
        else:
            formattedModule = ""
        return formattedModule + '/' + formattedFiles

    def format_files(self, files):
        """Break up our list of files into a common prefix and a sensibly-sized
           list of filenames after that prefix. Prepend the module name if we have one.
           """
        prefix, endings = self.consolidateFiles(files)
        endingStr = " ".join(endings)
        if len(endingStr) > self.filesWidthLimit:
            # If the full file list is too long, give a file summary instead
            endingStr = self.summarizeFiles(endings)
        if prefix.startswith('/'):
            prefix = prefix[1:]
        if endingStr:
            return "%s (%s)" % (prefix, endingStr)
        else:
            return prefix

    def summarizeFiles(self, files):
        """Given a list of strings representing file paths, return
           a summary of those files and/or directories. This is used
           in place of a full file list when that would be too long.
           """
        # Count the number of distinct directories we have
        dirs = {}
        for file in files:
            dirs[posixpath.split(file)[0]] = True

        if len(dirs) <= 1:
            return "%d files" % len(files)
        else:
            return "%d files in %d dirs" % (len(files), len(dirs))

    def wrapLine(self, line, width):
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

    def format_log(self, log):
        # Break the log string into wrapped lines
        lines = []
        for line in str(log).strip().split("\n"):
            # Ignore blank lines
            if not line.strip():
                continue

            # Expand tabs before we try wrapping
            line = line.replace("\t", " "*8)

            # Wrap long lines
            if self.widthLimit and len(line) > self.widthLimit:
                lines.extend(self.wrapLine(line, self.wrapWidth))
            else:
                lines.append(line)

        # If our lineLimit is 1, don't bother starting long logs on the
        # next line since there will be no long logs. Instead of the
        # long (log message trimmed), just add an ellipsis.
        if self.lineLimit == 1:
            if len(lines) > 1:
                lines[0] += ' ...'
                del lines[1:]

        # Multiline logs shouldn't start on the same line as the metadata
        elif len(lines) > 1:
            lines.insert(0, '')

            # Truncate long log messages if we have a limit
            if self.lineLimit and len(lines) > self.lineLimit + 1:
                lines[0] = "(log message trimmed)"
                del lines[self.lineLimit + 1:]

        # Reassemble the log message and send it to the default formatter
        return "\n".join(lines)

    def format_module(self, module):
        return str(module).strip()

    def format_author(self, author):
        return str(author).strip()

    def format_branch(self, branch):
        return str(branch).strip()

    def format_version(self, version):
        return str(rev).strip()

    def format_revision(self, rev):
        return 'r' + str(rev).strip()


class CommitToIRC(CommitFormatter):
    """Converts commit messages to plain text with IRC color tags"""
    medium = 'irc'
    lineLimit = 6
    widthLimit = 220
    wrapWidth = 80

    def __init__(self):
        """By default, use the IRC color formatter"""
        from IRC.Formatting import format
        self.colorFormatter = format

    def noColorFormatter(self, text, *tags):
        """A replacement formatter that ignores colors"""
        return text

    def param_noColor(self, tag):
        """The <noColor> tag disables colors, naturally"""
        self.colorFormatter = self.noColorFormatter

    def format_author(self, author):
        return self.colorFormatter(CommitFormatter.format_author(self, author), 'green')

    def format_version(self, version):
        return self.colorFormatter(str(version).strip(), 'bold')

    def format_revision(self, rev):
        return 'r' + self.colorFormatter(str(rev).strip(), 'bold')

    def format_module(self, module):
        return self.colorFormatter(CommitFormatter.format_module(self, module), 'aqua')

    def format_branch(self, branch):
        return self.colorFormatter(CommitFormatter.format_branch(self, branch), 'orange')

    def joinMessage(self, metadata, log):
        return "%s%s %s" % (" ".join(metadata), self.colorFormatter(':', 'bold'), log)


class CommitToXHTML(CommitFormatter):
    """Converts commit messages to XHTML, represented as a Nouvelle tag tree."""
    medium = 'xhtml'

    def joinMessage(self, metadata, log):
        """Join the metadata and log message into a CSS-happy box"""
        return [
            Nouvelle.tag('div', style=
                         "border: 1px solid #888; "
                         "background-color: #DDD; "
                         "padding: 0.25em 0.5em;"
                         "margin: 0.5em 0em; "
                         )[ metadata ],
            Nouvelle.tag('p', style=
                         "padding: 0em; "
                         "margin: 0em; "
                         )[ log ],
            ]

    def format_log(self, log):
        """Convert the log message to HTML by replacing newlines with <br> tags.
           Remember that Nouvelle handles quoting automagically for us.
           """
        content = []
        log = str(log).strip()
        if log:
            for line in log.split("\n"):
                if content:
                    content.append(Nouvelle.tag('br'))
                content.append(line)
        else:
            content.append(Nouvelle.tag('i')["No log message"])
        return content

    def format_author(self, author):
        return [
            " Commit by ",
            Nouvelle.tag('strong')[ str(author) ],
            " ",
            ]

    def format_separator(self):
        return Nouvelle.tag('span', style="color: #888;")[" :: "]

    def format_revision(self, rev):
        return [' r', Nouvelle.tag('b')[str(rev).strip()], ' ']

    def format_version(self, ver):
        return [' ', Nouvelle.tag('b')[str(ver)], ' ']

    def format_branch(self, branch):
        return [' on ', str(branch), ' ']

    def format_module(self, module):
        return Nouvelle.tag('b')[str(module).strip()]

    def format_moduleAndFiles(self, message):
        """Format the module name and files, joined together if they are both present."""
        items = [' ']
        if message.xml.source and message.xml.source.module:
            items.append(self.format_module(message.xml.source.module))
        if message.xml.body.commit.files:
            if items:
                items.append("/")
            items.append(self.format_files(message.xml.body.commit.files))
        items.append(' ')
        return items


class CommitTitle(CommitFormatter):
    """Extracts a title from commit messages"""
    medium = 'title'
    widthLimit = 80

    def format(self, message, input=None):
        # Get the log message, collapse whitespace, and truncate
        log = re.sub("\s+", " ", str(message.xml.body.commit.log))
        if len(log) > self.widthLimit:
            log = log[:self.widthLimit] + " ..."
        return log


class ColortextTitle(Message.Formatter):
    """Extracts a title from colorText messages"""
    detector = Message.Filter('<find path="/message/body/colorText"/>')
    medium = 'title'
    widthLimit = 80

    def format(self, message, input=None):
        # Extract plaintext from the entire message, collapse whitespace, and truncate
        log = re.sub("\s+", " ", XML.allText(message.xml.body.colorText))
        if len(log) > self.widthLimit:
            log = log[:self.widthLimit] + " ..."
        return log


class ColortextToXHTML(Message.Formatter):
    """Converts messages with colorText content to XHTML (using Nouvelle)
       with colors represented by CSS 'class' attributes on <span> tags,
       and with bold and underline converted to <b> and <u> tags.
       Returns an object that can be serialized into XHTML by a Nouvelle.Serializer.
       """
    detector = Message.Filter('<find path="/message/body/colorText"/>')
    medium = 'xhtml'

    def format(self, message, input=None):
        return self.Parser(message.xml.body.colorText).result

    class Parser(XML.XMLObjectParser):
        requiredRootElement = 'colorText'

        def element_colorText(self, element):
            return [self.parse(e) for e in element.children]

        def parseString(self, s):
            return s

        def element_b(self, element):
            return Nouvelle.tag('b')[ self.element_colorText(element) ]

        def element_u(self, element):
            return Nouvelle.tag('u')[ self.element_colorText(element) ]

        def element_br(self, element):
            return Nouvelle.tag('br')

        def colorQuote(self, color):
            """Make a color name safe for inclusion into a class attribute.
               This just replaces any non-alphabetical characters with hyphens.
               """
            return re.sub("[^a-zA-Z]", "-", color)

        def element_color(self, element):
            """Convert the fg and bg attributes, if we have them, to <span> tags"""
            s = self.element_colorText(element)
            if element.hasAttribute('fg'):
                s = Nouvelle.tag('span', _class='fgColor-'+self.colorQuote(element['fg']))[s]
            if element.hasAttribute('bg'):
                s = Nouvelle.tag('span', _class='bgColor-'+self.colorQuote(element['bg']))[s]
            return s


class IRCProjectName(Message.Formatter):
    """Prepends the project name to each line of the input message, boldinated for IRC"""
    medium = 'irc'
    def format(self, message, input):
        if not input:
            return
        if message.xml.source and message.xml.source.project:
            from IRC.Formatting import format
            prefix = format("%s:" % message.xml.source.project, 'bold') + " "
            return "\n".join([prefix + line for line in input.split("\n")])
        else:
            return input


# This indexes the formatters in this module and provide a
# higher level interface for picking one and instantiating it.
factory = Message.FormatterFactory(globals())

### The End ###
