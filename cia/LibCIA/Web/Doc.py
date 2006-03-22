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
# Copyright (C) 2003-2005 Micah Dowty <micah@navi.cx>
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
from LibCIA import Cache
from LibCIA.Web import Template, Server
import os, posixpath
from twisted.internet import defer
from twisted.web import error, server
from twisted.protocols import http
from twisted.web.woven import dirlist


class Component(Server.Component):
    """A component representing the on-disk documentation tree"""
    name = "Documentation"

    def __init__(self, basePath):
        self.basePath = basePath
        self.resource = Page(self)
	self.resource.load()

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
        self.headingLevel = 0
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
        del self.stack[-1]

    def visit_Text(self, node):
        self.stack[-1].append(node.astext())

    def depart_Text(self, node):
        pass

    def visit_comment(self, node):
        raise nodes.SkipNode

    def visit_section(self, node):
        self.headingLevel += 1

    def depart_section(self, node):
        self.headingLevel -= 1

    def visit_title(self, node):
        title = node.astext()
        if self.headingLevel:
            # Make this into a heading tag
            self.stack[-1].append(tag('h%d' % self.headingLevel)[title])
        else:
            # Nope, this must be the top-level title.
            self.docTitle = title
        raise nodes.SkipNode

    def depart_title(self, node):
        pass

    def visit_subtitle(self, node):
        # We really only care about the top-level subtitle now
        if not self.headingLevel:
            self.docSubtitle = node.astext()
        raise nodes.SkipNode

    def visit_reference(self, node):
        if node.has_key('refuri'):
            if node['refuri'].startswith('mailto:'):
                self.enterTag(Template.EmailLink(node['refuri']))
            else:
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
        'literal_block':   tag('pre', _class='literal-block'),
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


class DocumentCache(Cache.AbstractObjectCache):
    """A cache that formats reStructuredText documents as Nouvelle trees,
       keyed on the document's path and its modification time.
       """
    def miss(self, path, mtime):
        w = NouvelleWriter()
        core.publish_file(source_path = path,
                          writer      = w,
                          destination = NullFile(),
                          settings_overrides = {
				'traceback': True,
				},
                          )
        w.mtime = mtime
        del w.document
        return w


class Page(Template.Page):
    """A web page holding a docutils-formatted document"""
    indexNames = ('index', 'index.txt')

    def __init__(self, component, path=''):
        self.component = component
        self.path = path
        self.fsPath = self.getFilesystemPath()
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

    def formatDoc(self, path):
        """Get a formatted document, via our cache"""
        mtime = os.stat(path).st_mtime
        return DocumentCache().get(path, mtime)

    def load(self):
        """Load our formatted document from disk, returnign a Deferred
           signalling completion.
           """
        result = defer.Deferred()

        if os.path.isfile(self.fsPath):
            self.formatDoc(self.fsPath).addCallback(
                self._load, result).addErrback(result.errback)

        elif os.path.isdir(self.fsPath):
            # If we have a directory with no index, make our title the directory name
            self.mainTitle = self.path.split('/')[-1]
            result.callback(None)

        else:
            # Nothing to load
            result.callback(None)

        return result

    def _load(self, mainDoc, result):
        # It's a normal document. Get the document itself and its sidebar,
        # and store the formatter results that we're interested in.
        self.mainDoc = mainDoc
        self.mainColumn = Template.pageBody[self.mainDoc.output]
        self.mainTitle = self.mainDoc.docTitle
        if self.mainDoc.docSubtitle:
            self.subTitle = self.mainDoc.docSubtitle
        self.leftColumn = self.loadSidebar(self.findSidebarPath(self.fsPath))

        result.callback(None)

    def getChild(self, path, request):
        if not path:
            return self
        elif path[0] == '.':
            return error.NoResource("Dotfiles are restricted")
        else:
            return self.__class__(self.component, os.path.join(self.path, path))

    def getURL(self, context):
        return posixpath.join(self.component.url, self.path)

    def parent(self):
        if self.path:
            parent = Page(self.component, posixpath.split(self.path)[0])

            # FIXME: load() may not finish immediately
            parent.load()

            return parent

    def preRender(self, context):
        """Before rendering, load our document and see if we need to abort"""
        result = defer.Deferred()
        self.load().addCallback(self._checkRender, result, context).addErrback(result.errback)
        return result

    def _checkRender(self, loaded, result, context):
        # Is this actually a directory we need to list?
        if os.path.isdir(self.fsPath):
            result.callback(dirlist.DirectoryLister(self.fsPath).render(context['request']))
            return

        # Is this a 404?
        if not os.path.isfile(self.fsPath):
            result.callback(error.NoResource("File not found.").render(context['request']))
            return

        # Support the Last-Modified and If-Modified-Since headers,
        # don't bother rendering the page if the browser already has an up to date copy.
        if context['request'].setLastModified(self.mainDoc.mtime) is http.CACHED:
            result.callback('')
            return

        # Normal render
        result.callback(None)

    def findSidebarPath(self, path, format=".%s.sidebar"):
        if os.path.isfile(format % path):
            return format % path
        else:
            return os.path.join(os.path.split(path)[0], format % 'default')

    def loadSidebar(self, path):
        """Load sidebar links from a simple text file format.
           Lines beginning with a dash specify a new section heading,
           links are made by lines of the form 'title :: URL'.
           Other lines are ignored.
           """
        sections = []
        for line in open(path).xreadlines():
            line = line.strip()
            if not line:
                continue

            if line[0] == '-':
                sections.append(Template.StaticSection(line[1:].strip()))
                continue

            pieces = line.split("::", 1)
            if len(pieces) > 1:
                title, url = pieces
                sections[-1].rows.append( tag('a', href=url.strip())[title.strip()] )

        return sections

### The End ###
