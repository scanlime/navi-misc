""" LibCIA.Formatters.Commit

Formatters used for converting commit messages to other formats.
Note that this only handles real XML commit messages. The legacy
'colorText' messages are handled by a separate module.
"""
#
# CIA open source notification system
# Copyright (C) 2003-2004 Micah Dowty <micah@navi.cx>
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
from Nouvelle import tag
import re, posixpath
from twisted.python.util import OrderedDict
import Util

__all__ = ['CommitToIRC', 'CommitToPlaintext', 'CommitToXHTML',
           'CommitTitle', 'CommitToXHTMLLong']


class CommitFormatter(Message.Formatter):
    """Base class for formatters that operate on commit messages.
       Includes a filter for commit messages, and utilities for
       extracting useful information from the commits.
       """
    filter = '<find path="/message/body/commit"/>'
    defaultComponentTree = """<format>
    <author/> <branch/> * <version/> <autoHide>r<revision/></autoHide> <module/>/<files/>: <log/>
    </format>"""

    # Subclasses can set this to limit the length of log messages, in lines
    lineLimit = None

    # Lines in the log longer than this are wrapped to wrapWidth
    widthLimit = None
    wrapWidth = None

    # If the list of files ends up longer than this many characters, summarize it
    filesWidthLimit = 60

    def param_lineLimit(self, tag):
        self.lineLimit = int(XML.shallowText(tag))

    def param_widthLimit(self, tag):
        self.widthLimit = int(XML.shallowText(tag))
        if self.wrapWidth > self.widthLimit:
            self.wrapWidth = self.widthLimit

    def param_wrapWidth(self, tag):
        self.wrapWidth = int(XML.shallowText(tag))

    def param_filesWidthLimit(self, tag):
        self.filesWidthLimit = int(XML.shallowText(tag))

    def component_author(self, element, args):
        return self.textComponent(element, args, "message", "body", "commit", "author")

    def component_version(self, element, args):
        return self.textComponent(element, args, "message", "body", "commit", "version")

    def component_revision(self, element, args):
        return self.textComponent(element, args, "message", "body", "commit", "revision")

    def component_branch(self, element, args):
        return self.textComponent(element, args, "message", "source", "branch")

    def component_module(self, element, args):
        return self.textComponent(element, args, "message", "source", "module")

    def component_project(self, element, args):
        return self.textComponent(element, args, "message", "source", "project")

    def component_files(self, element, args):
        """Break up our list of files into a common prefix and a sensibly-sized
           list of filenames after that prefix.
           """
        files = XML.dig(message.xml, "message", "body", "commit", "files")
        if not files:
            return []

        prefix, endings = self.consolidateFiles(files)
        endingStr = " ".join(endings)
        if len(endingStr) > self.filesWidthLimit:
            # If the full file list is too long, give a file summary instead
            endingStr = self.summarizeFiles(endings)
        if prefix.startswith('/'):
            prefix = prefix[1:]

        if endingStr:
            return ["%s (%s)" % (prefix, endingStr)]
        else:
            return [prefix]

    def component_log(self, element, args):
        log = XML.dig(message.xml, "message", "body", "commit", "log")
        if not log:
            return []

        # Break the log string into wrapped lines
        lines = []
        for line in Util.getNormalizedLog(log):
            # Ignore blank lines
            if not line:
                continue

            # Wrap long lines
            if self.widthLimit and len(line) > self.widthLimit:
                lines.extend(Util.wrapLine(line, self.wrapWidth))
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
        return ["\n".join(lines)]

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

    def consolidateFiles(self, xmlFiles):
        """Given a <files> element, find the directory common to all files
           and return a 2-tuple with that directory followed by
           a list of files within that directory.
           """
        files = []
        if xmlFiles:
            for fileTag in XML.getChildElements(xmlFiles):
                if fileTag.nodeName == 'file':
                    files.append(XML.shallowText(fileTag))

        # If we only have one file, return it as the prefix.
        # This prevents the below regex from deleting the filename
        # itself, assuming it was a partial filename.
        if len(files) == 1:
            return files[0], []

        # Start with the prefix found by commonprefix,
        # then actually make it end with a directory rather than
        # possibly ending with part of a filename.
        prefix = re.sub("[^/]*$", "", posixpath.commonprefix(files))

        endings = []
        for file in files:
            endings.append(file[len(prefix):])
        return prefix, endings


class CommitToIRC(CommitFormatter):
    """Converts commit messages to plain text with IRC color tags"""
    medium = 'irc'
    lineLimit = 6
    widthLimit = 220
    wrapWidth = 80

    defaultComponentTree = """<format>
    <color fg="green"><author/> <branch/> * <version/> <autoHide>r<revision/></autoHide> <module/>/<files/>: <log/>
    </format>"""

    def __init__(self):
        """By default, use the IRC color formatter"""
        CommitFormatter.__init__(self)
        from LibCIA.IRC.Formatting import format
        self.colorFormatter = format

    def noColorFormatter(self, text, *tags):
        """A replacement formatter that ignores colors"""
        return text

    def param_noColor(self, tag):
        """The <noColor> parameter disables all colors, naturally"""
        self.colorFormatter = self.noColorFormatter

    def component_color(self, element, args):
        """This formatter component lets you use colorText-style
           colorization tags inside the <format> parameter.
           """
        pass

    def format_author(self, author):
        return self.colorFormatter(CommitFormatter.format_author(self, author), 'green')

    def format_version(self, version):
        return self.colorFormatter(XML.shallowText(version).strip(), 'bold')

    def format_revision(self, rev):
        return 'r' + self.colorFormatter(XML.shallowText(rev).strip(), 'bold')

    def format_module(self, module):
        return self.colorFormatter(CommitFormatter.format_module(self, module), 'aqua')

    def format_branch(self, branch):
        return self.colorFormatter(CommitFormatter.format_branch(self, branch), 'orange')

    def joinMessage(self, metadata, log):
        return "%s%s %s" % (" ".join(metadata), self.colorFormatter(':', 'bold'), log)


class CommitToPlaintext(CommitFormatter):
    """Converts commit messages to plain text. Currently this is the same as
       the default commit formatting.
       """
    medium = 'plaintext'


class CommitTitle(CommitFormatter):
    """Extracts a title from commit messages"""
    medium = 'title'

    def format(self, args):
        log = XML.dig(args.message.xml, "message", "body", "commit", "log")
        if log:
            return Util.extractSummary(log)


class CommitToXHTML(CommitFormatter):
    """Converts commit messages to XHTML, represented as a Nouvelle tag tree."""
    medium = 'xhtml'

    def __init__(self):
        from LibCIA.Web import RegexTransform
        self.hyperlinker = RegexTransform.AutoHyperlink()

    def joinMessage(self, metadata, log):
        """Join the metadata and log message into a CSS-happy box"""
        return [
            tag('div', style=
                         "border: 1px solid #888; "
                         "background-color: #DDD; "
                         "padding: 0.25em 0.5em;"
                         "margin: 0em;"
                         )[ metadata ],
            tag('p', style=
                         "padding: 0em; "
                         "margin: 0.5em 0em; "
                         )[ log ],
            ]

    def format_log(self, log):
        """Convert the log message to HTML. If the message seems to be preformatted
           (it has some lines with indentation) it is stuck into a <pre>. Otherwise
           it is converted to HTML by replacing newlines with <br> tags and converting
           bulletted lists.
           """
        content = []
        lines = Util.getNormalizedLog(log)
        nonListItemLines = []
        listItems = []

        if lines:
            # Scan the message, applying a few heuristics. If we see lines
            # that are still starting with a space after getNormalizedLog
            # has done its thing, assume the text is preformatted. Also
            # look for lines that appear to be list items.
            isPreFormatted = False
            for line in lines:
                if line and line[0] == ' ':
                    isPreFormatted = True

                if line.startswith("* ") or line.startswith("- "):
                    # Assume this is a list item, and convert the bullets to
                    # a proper XHTML list.
                    listItems.append(line[2:])
                else:
                    if listItems:
                        # It's a continuation of the last item
                        listItems[-1] = listItems[-1] + " " + line.strip()
                    else:
                        # If we haven't seen a list yet, stick this in nonListItemLines.
                        # If this log message isn't a list at all, everything will end
                        # up there but it will be safely ignored
                        nonListItemLines.append(line)

            if listItems:
                # It looks like a bulleted list. First output the nonListItemLines,
                # then stick the items inside a list.
                for line in nonListItemLines:
                    if content:
                        content.append(tag('br'))
                    content.append(line)
                content.append(tag('ul')[[ tag('li')[item] for item in listItems ]])

            elif isPreFormatted:
                # This is probably a preformatted message, stick it in a <pre>
                content.append(tag('pre')[ "\n".join(lines) ])

            else:
                # Plain old text, just stick <br>s between the lines
                for line in lines:
                    if content:
                        content.append(tag('br'))
                    content.append(line)
        else:
            content.append(tag('i')["No log message"])

        return self.hyperlinker.apply(content)

    def format_author(self, author):
        return [
            " Commit by ",
            tag('strong')[ XML.shallowText(author) ],
            " ",
            ]

    def format_separator(self):
        return tag('span', style="color: #888;")[" :: "]

    def format_revision(self, rev):
        return [' r', tag('b')[XML.shallowText(rev).strip()], ' ']

    def format_version(self, ver):
        return [' ', tag('b')[XML.shallowText(ver)], ' ']

    def format_branch(self, branch):
        return [' on ', XML.shallowText(branch), ' ']

    def format_module(self, module):
        return tag('b')[XML.shallowText(module).strip()]

    def format_moduleAndFiles(self, message):
        """Format the module name and files, joined together if they are both present."""
        items = [' ']
        module = XML.dig(message.xml, "message", "source", "module")
        if module:
            items.append(self.format_module(module))
        files = XML.dig(message.xml, "message", "body", "commit", "files")
        if files:
            if items:
                items.append("/")
            items.append(self.format_files(files))
        items.append(' ')
        return items


class CommitToXHTMLLong(CommitToXHTML):
    """Builds on the xhtml formatter to generate a longer representation of the commit,
       suitable for a full page rather than just an item in a listing.
       """
    medium = 'xhtml-long'

    def format(self, args):
        from LibCIA.Web import Template

        message   = args.message
        commit    = XML.dig(message.xml, "message", "body", "commit")
        source    = XML.dig(message.xml, "message", "source")
        author    = XML.dig(commit, "author")
        version   = XML.dig(commit, "version")
        revision  = XML.dig(commit, "revision")
        diffLines = XML.dig(commit, "diffLines")
        url       = XML.dig(commit, "url")
        log       = XML.dig(commit, "log")
        project   = XML.dig(source, "project")
        module    = XML.dig(source, "module")
        branch    = XML.dig(source, "branch")
        headers   = OrderedDict()

        if author:
            headers['Author'] = XML.shallowText(author)
        if project:
            headers['Project'] = XML.shallowText(project)
        if module:
            headers['Module'] = XML.shallowText(module)
        if branch:
            headers['Branch'] = XML.shallowText(branch)
        if version:
            headers['Version'] = XML.shallowText(version)
        if revision:
            headers['Revision'] = XML.shallowText(revision)
        if diffLines:
            headers['Changed Lines'] = XML.shallowText(diffLines)
        if url:
            headers['URL'] = tag('a', href=XML.shallowText(url))[ Util.extractSummary(url) ]

        content = [
            tag('h1')[ "Commit Message" ],
            Template.MessageHeaders(headers),
            tag('p', _class="messageBody")[ self.format_log(log) ],
            ]

        files = XML.dig(message.xml, "message", "body", "commit", "files")
        if files and XML.hasChildElements(files):
            content.extend([
                tag('h1')[ "Modified Files" ],
                self.format_files(files),
                ])
        return content

    def format_files(self, xmlFiles):
        """Format the contents of our <files> tag as a tree with nested lists"""
        from LibCIA.Web import Template

        # First we organize the files into a tree of nested dictionaries.
        # The dictionary we ultimately have FileTree render maps each node
        # (file or directory) to a dictionary of its contents. The keys
        # in these dictionaries can be any Nouvelle-renderable object
        # produced by format_file.
        #
        # As a first step, we build a dictionary mapping path segment to
        # [fileTag, children] lists. We then create a visual representation
        # of each fileTag and generate the final dictionary.
        fileTree = {}
        if xmlFiles:
            for fileTag in XML.getChildElements(xmlFiles):
                if fileTag.nodeName == 'file':
                    # Separate the file into path segments and walk into our tree
                    node = [None, fileTree]
                    for segment in XML.shallowText(fileTag).split('/'):
                        node = node[1].setdefault(segment, [None, {}])
                    # The leaf node owns this fileTag
                    node[0] = fileTag

        return Template.FileTree(self.format_file_tree(fileTree))

    def format_file_tree(self, fileTree):
        """This is the second half of format_files- it recursively
           converts a tree of [fileTag,children] style dictionaries
           into a tree of Template.FileTree() compatible dicts,
           using format_file() to render each fileTag.
           """
        result = {}
        for name, t in fileTree.iteritems():
            fileTag, children = t
            result[self.format_file(name, fileTag)] = self.format_file_tree(children)
        return result

    def format_file(self, name, fileTag=None):
        """Given the short name of a file, and optionally its XML tag,
           return a Nouvelle-serializable representation.
           """
        if fileTag:
            
            # If we have a 'uri' attribute, make this file a hyperlink
            uri = fileTag.getAttributeNS(None, 'uri')
            if uri:
                return tag('a', href=uri)[ name ]
              
        return name

### The End ###
