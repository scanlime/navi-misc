""" LibCIA.Web.Doc

A web interface for browsing our documentation, converting from
reStructuredText to Nouvelle trees using docutils. For more
information on docutils and reStructuredText, see the docutils web site:

   http://docutils.sourceforge.net

reStructuredText has the nice quality of being easy to format
automatically while still looking nice in its unformatted state.
The plaintext documents in our 'doc' directory therefore can serve
as both our web site and our package's standalone documentation.
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

from docutils import core, writers, nodes
from Nouvelle import tag, place
import Template
import os
from twisted.web import error
from twisted.web.woven import dirlist


class NouvelleTranslator(nodes.NodeVisitor):
    """A visitor that converts docutils documents to Nouvelle
       serializable objects. Every node entered must push the
       Nouvelle object representing it onto self.stack, while
       every tag departed pops an item off of self.stack.
       The document root ends up at self.root. The top item
       on our stack should always be a list, and represents
       the current insertion point.
       """
    def __init__(self, document):
        self.root = []
        self.docTitle = None
        self.docSubtitle = None
        self.stack = [self.root]
        self.sectionStack = []

        nodes.NodeVisitor.__init__(self, document)

    def enterTag(self, tag):
        """Add the given tag at the current insertion point, and
           move the insertion point inside the tag. Returns the
           tag instance we actually insert.
           """
        arg = []
        t = tag[arg]
        self.stack[-1].append(t)
        self.stack.append(arg)
        return t

    def unknown_visit(self, node):
        """If we have a mapping from this node to a Nouvelle tag,
           toss one of those on the stack. (They're immutable, no
           need to copy or anything).
           If not, toss on a duplicate of the current top of the
           stack, so we don't have to do anything special in departure.
           """
        name = node.__class__.__name__
        if name in self.tagMap:
            self.enterTag(self.tagMap[name])
        else:
            self.stack.append(self.stack[-1])

    def unknown_departure(self, node):
        del self.stack[-1]

    def visit_Text(self, node):
        self.stack[-1].append(str(node))

    def depart_Text(self, node):
        pass

    def visit_comment(self, node):
        raise nodes.SkipNode

    def visit_section(self, node):
        if not self.sectionStack:
            # This is a top-level section. Convert it to a Template.Section
            self.sectionStack.append(self.enterTag(Template.StaticSection()))
        else:
            # Inner section, make a heading tag
            s = tag('h%d' % len(self.sectionStack))
            self.stack[-1].append(s)
            self.stack.append(self.stack[-1])
            self.sectionStack.append(s)

    def depart_section(self, node):
        del self.stack[-1]
        del self.sectionStack[-1]

    def visit_title(self, node):
        title = ''.join(map(str, node.children))

        if self.sectionStack:
            # We're inside a section, assign it this title.
            # Note that this viciously breaks the tag immutability
            # rule, but at the moment I can't think of a cleaner
            # way to do this.
            section = self.sectionStack[-1]
            if hasattr(section, 'title'):
                section.title = title
            elif hasattr(section, 'content'):
                section.content = title

        else:
            # Nope, this must be the top-level title
            self.docTitle = title

        raise nodes.SkipNode

    def visit_subtitle(self, node):
        # We really only care about the top-level subtitle now
        if not self.sectionStack:
            self.docSubtitle = ''.join(map(str, node.children))
        raise nodes.SkipNode

    def visit_reference(self, node):
        if node.has_key('refuri'):
            self.enterTag(tag('a', href=node['refuri']))
        else:
            # Internal links not supported yet
            self.stack.append(self.stack[-1])

    tagMap = {
        'paragraph':       tag('p'),
        'enumerated_list': tag('ol'),
        'bullet_list':     tag('ul'),
        'list_item':       tag('li'),
        'block_quote':     tag('blockquote'),
        'emphasis':        tag('em'),
        'strong':          tag('strong'),
        'literal_block':   tag('pre'),
        'definition':      tag('dd'),
        'definition_list': tag('dl'),
        'caption':         tag('p', _class='caption'),
        'system_message':  Template.error,
        'problematic':     Template.error,
        }


class NouvelleWriter(writers.Writer):
    """A writer.Writer implementation that converts a docutils
       document to a Nouvelle serializable object. This object
       is stored in self.output, not its serialized form.
       """
    def translate(self):
        visitor = NouvelleTranslator(self.document)
        self.document.walkabout(visitor)
        self.output = visitor.root
        self.docTitle = visitor.docTitle
        self.docSubtitle = visitor.docSubtitle


class NullFile:
    def write(self, data):
        pass
    def close(self):
        pass


class Page(Template.Page):
    """A web page holding a docutils-formatted document"""
    def __init__(self, path):
        self.path = path
        Template.Page.__init__(self)

    def getChild(self, path, request):
        if path and path[0] != '.':
            return self.__class__(os.path.join(self.path, path))
        else:
            return self

    def render(self, request):
        """Overrides the default render() function for pages. This checks
           whether our path is really a file or a directory- if it's a
           directory, we show a directory listing page. If it's a file,
           this formats our document and passes control on to the usual
           rendering processes.
           """
        path = self.path

        # Is this actually a directory?
        if os.path.isdir(path):
            # If we have an 'index' file, show that.. otherwise, a directory listing
            if os.path.isfile(os.path.join(path, 'index')):
                path = os.path.join(path, 'index')
            else:
                return dirlist.DirectoryLister(path).render(request)
        elif not os.path.isfile(path):
            return error.NoResource("File not found.").render(request)

        # Find a sidebar document and format it
        self.leftColumn = self.formatDocument(self.findSidebarPath(path)).output

        # Format the main document, storing its body and our titles
        mainDoc = self.formatDocument(path)
        self.mainColumn = mainDoc.output
        self.mainTitle = mainDoc.docTitle
        if mainDoc.docSubtitle:
            self.subTitle = mainDoc.docSubtitle
        return Template.Page.render(self, request)

    def findSidebarPath(self, path):
        if os.path.isfile(path + '.sidebar'):
            return path + '.sidebar'
        else:
            return os.path.join(os.path.split(path)[0], 'default.sidebar')

    def formatDocument(self, path):
        """Format a document, returning the NouvelleWriter instance
           containing the Nouvelle tree and other document information.
           """
        w = NouvelleWriter()
        core.publish_file(source_path = path,
                          writer      = w,
                          destination = NullFile(),
                          settings_overrides = {'output_encoding': 'unicode'}
                          )
        return w

### The End ###
