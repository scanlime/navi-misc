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
import Template, Server
import os, posixpath
from twisted.web import error
from twisted.web.woven import dirlist


class Component(Server.Component):
    """A component representing the on-disk documentation tree"""
    name = "Documentation"

    def __init__(self, basePath):
        self.basePath = basePath
        self.resource = Page(self)

    def __contains__(self, page):
        return isinstance(page, Page)


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
        self.inTableHeading = False

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
        # HTML sucks, paragraphs give us extra whitespace.
        # Remove paragraphs if we have only one of them.
        if len(self.stack[-1]) == 1:
            obj = self.stack[-1][0]
            if isinstance(obj, tag) and obj.name == 'p':
                self.stack[-1][0] = obj.content

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

    def visit_thead(self, node):
        self.inTableHeading = True

    def depart_thead(self, node):
        self.inTableHeading = False

    def visit_entry(self, node):
        if self.inTableHeading:
            self.enterTag(tag('th'))
        else:
            self.enterTag(tag('td'))

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
        'table':           tag('table'),
        'row':             tag('tr'),
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


formattedDocCache = {}

def getFormattedDoc(path):
    """Returns a NouvelleWriter instance representing a formatted version of the
       given path. Formatted documents are cached, but the cache is invalidated
       when the source document's modification date changes.
       """
    global formattedDocCache
    mtime = os.stat(path).st_mtime
    try:
        w = formattedDocCache[path]
        if w.mtime == mtime:
            return w
    except KeyError:
        pass
    w = NouvelleWriter()
    core.publish_file(source_path = path,
                      writer      = w,
                      destination = NullFile(),
                      settings_overrides = {'output_encoding': 'unicode'}
                      )
    w.mtime = mtime
    formattedDocCache[path] = w
    return w


class Page(Template.Page):
    """A web page holding a docutils-formatted document"""
    indexNames = ('index', 'index.txt')

    def __init__(self, component, path=''):
        self.component = component
        self.path = path
        self.fsPath = self.getFilesystemPath()
        self.load()
        Template.Page.__init__(self)

    def getFilesystemPath(self):
        """Determine the path on disk that this document should load from,
           looking for an index file if this page refers to a directory.
           """
        p = os.path.join(self.component.basePath, *self.path.split("/"))
        if os.path.isdir(p):
            # If this is a directory, try to find an index file for it
            for indexName in self.indexNames:
                if os.path.isfile(os.path.join(p, indexName)):
                    return os.path.join(p, indexName)
        return p

    def load(self):
        """Load our formatted document from disk, via getFormattedDoc's cache"""
        if os.path.isfile(self.fsPath):
            # It's a normal document. Get the document itself and its sidebar,
            # and store the formatter results that we're interested in.
            self.mainDoc = getFormattedDoc(self.fsPath)
            self.mainColumn = self.mainDoc.output
            self.mainTitle = self.mainDoc.docTitle
            if self.mainDoc.docSubtitle:
                self.subTitle = self.mainDoc.docSubtitle
            self.leftColumn = getFormattedDoc(self.findSidebarPath(self.fsPath)).output

        elif os.path.isdir(self.fsPath):
            # If we have a directory with no index, make our title the directory name
            self.mainTitle = self.path.split('/')[-1]

    def getChild(self, path, request):
        if path and path[0] != '.':
            return self.__class__(self.component, os.path.join(self.path, path))
        else:
            return self

    def getURL(self, context):
        return posixpath.join(self.component.url, self.path)

    def parent(self):
        if self.path:
            return Page(self.component, posixpath.split(self.path)[0])

    def render(self, request):
        """Intercept the normal rendering process if necessary to display
           a 'file not found' error or a directory listing page.
           """
        # This is redundant if we've just created this Page, but
        # for pages that stick around for a while like the root document
        # this is necessary to let the page update at all. A bit messy, but
        # not really slow because of the cache.
        self.load()

        if os.path.isdir(self.fsPath):
            return dirlist.DirectoryLister(self.fsPath).render(request)
        elif not os.path.isfile(self.fsPath):
            return error.NoResource("File not found.").render(request)
        return Template.Page.render(self, request)

    def findSidebarPath(self, path):
        if os.path.isfile(path + '.sidebar'):
            return path + '.sidebar'
        else:
            return os.path.join(os.path.split(path)[0], 'default.sidebar')

### The End ###
