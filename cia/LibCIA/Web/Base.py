""" LibCIA.Web.Base

A very simple web objects framework, in the spirit of nevow
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

from twisted.xish import domish
from twisted.web import resource


class place:
    """A placeholder for data that can be rendered by a document's owner.
       For example, place('title') calls render_title() in the object owning
       the current document, context['owner'].
       """
    def __init__(self, name, *args, **kwargs):
        self.name = name
        self.args = args
        self.kwargs = kwargs

    def render(self, context):
        return getattr(context['owner'], 'render_' + self.name)(context, *self.args, **self.kwargs)


class xml(str):
    """A marker indicating data that is already represented in raw XML and
       needs no further processing.
       """
    __slots__ = []


class Serializer:
    """Convert arbitrary objects to xml markers recursively. Renderable objects
       are allowed to render themselves, other types must have renderers looked
       up in this class's render_* methods.
       """
    def render(self, obj, context):
        """Look up a render_* function based on the object's type"""
        if hasattr(obj, 'render'):
            return self.render_renderable(obj, context)
        try:
            f = getattr(self, 'render_' + type(obj).__name__)
        except AttributeError:
            f = self.render_other
        return f(obj, context)

    def render_xml(self, obj, context):
        return obj

    def render_list(self, obj, context):
        return xml(''.join([self.render(o, context) for o in obj]))

    def render_tuple(self, obj, context):
        return xml(''.join([self.render(o, context) for o in obj]))

    def render_function(self, obj, context):
        return self.render(obj(context), context)

    def render_instancemethod(self, obj, context):
        return self.render(obj(context), context)

    def render_renderable(self, obj, context):
        return self.render(obj.render(context), context)

    def render_other(self, obj, context):
        return xml(domish.escapeToXml(str(obj)))


class tag:
    """A renderable XHTML tag, containing other renderable objects.
       If an object enclosed doesn't have a 'render' method, it is casted
       to a string and quoted.

       This class was inspired by nevow, but much simpler. In CIA, dynamically
       rebuilding the code to implement changes at runtime is more important
       than speed, and this avoids much of the complexity (and power) of
       nevow's ISerializable.

       Attributes are quoted and converted to tag attributes. Leading underscores
       are removed, so they can be used to define attributes that are reserved
       words in Python.
       """
    def __init__(self, name, **attributes):
        self.name = name
        self.content = []
        self.setAttributes(attributes)

    def setAttributes(self, attributes):
        """Change this tag's attributes, rerendering the opening and closing text"""
        opening = '<' + self.name
        for key, value in attributes.iteritems():
            if key[0] == '_':
                key = key[1:]
            if value:
                opening += ' %s="%s"' % (key, domish.escapeToXml(value, True))
        opening += '>'
        self.renderedOpening = xml(opening)
        self.renderedClosing = xml('</%s>' % self.name)

    def __getitem__(self, content):
        """Overloads the [] operator used, in Tag, to set the tag's contents"""
        self.content = content
        return self

    def render(self, context=None):
        return [self.renderedOpening, self.content, self.renderedClosing]


class DocumentOwner(object):
    """A base class defining a 'render' function for objects that own a document"""
    def render(self, context={}):
        myContext = dict(context)
        myContext['owner'] = self
        return Serializer().render(self.document, myContext)


class Page(resource.Resource):
    """A web resource that renders a tree of tag instances from its 'document' attribute"""
    def render(self, request):
        context  = {
            'owner': self,
            'request': request,
            }
        self.preRender(context)
        return str(Serializer().render(self.document, context))

    def preRender(self, context):
        """Called prior to rendering each request, subclasses can use this to annotate
           'context' with extra information or perform other important setup tasks.
           """
        pass

### The End ###
