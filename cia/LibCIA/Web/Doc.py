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


class ObjectOutput:
    """A File-like object that just saves the object written to it as
       self.data. This is a hack that seems to be necessary to output
       Nouvelle trees from docutils.
       """
    def write(self, data):
        self.data = data

    def close(self):
        pass


class Page(Template.Page):
    """A web page holding a docutils-formatted document"""
    writerFactory = NouvelleWriter

    def __init__(self, source_path=None, source=None):
        self.source = source
        self.source_path = source_path
        self.writer = self.writerFactory()

        out = ObjectOutput()
        core.publish_file(source      = self.source,
                          source_path = self.source_path,
                          writer      = self.writer,
                          destination = out,
                          settings_overrides = {'output_encoding': 'unicode'}
                          )

        self.body = out.data
        self.mainTitle = self.writer.docTitle
        if self.writer.docSubtitle:
            self.subTitle = self.writer.docSubtitle

    mainColumn = [
        place('body'),
        ]

### The End ###
