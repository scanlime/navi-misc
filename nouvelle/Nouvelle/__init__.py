""" Nouvelle

Nouvelle is a simple web framework, similar to the 'nevow' package
in Quotient. Nouvelle has many similarities to nevow, but several
key differences:

  - nevow is based on Twisted, while Nouvelle can work in any web
    server environment without requiring more than Python's standard
    library. Nouvelle supports twisted.web and deferred rendering,
    but this is in an add-on rather than in the core of Nouvelle.

  - While nevow is a relatively simple web framework, Nouvelle is
    even simpler, making it very easy to understand and quick to load.

  - nevow keeps as much separation of content and presentation as
    possible by using a registry of ISerializable adaptors for various
    data types. Nouvelle takes a simpler approach in which a Serializer
    object looks up a method to handle each type. The default Serializer
    includes methods for important builtin types, but instead of
    encouraging the user to create ISerializable adaptors for their
    data, Nouvelle encourages the user to create objects with a 'render'
    method that may wrap a data model object if necessary.

  - When a tag from nevow's 'stan' module is called or indexed into, to
    change its attributes or content, the tag is modified and returns
    itself. This causes problems when you want to create one tag as
    a template then create tags based on it without modifying the template.
    Nouvelle's tags are normally immutable, so this sort of template
    system is quite easy.

Like nevow's 'stan' module, Nouvelle has a simple pure-python syntax
for expressing XHTML documents efficiently. The 'tag' object is a class,
which must be instantiated with a tag name. Keyword arguments are converted
to tag attributes. Leading underscores are stripped, so it's still possible
to represent attributes that are reserved words in Python. Once a tag object
is instantiated, it can be called again to instantiate a new tag with a name
and/or attributes based on the original tag. Finally, using square brackets
a new tag may be instantiated with the same name and attributes but new
content, described in the brackets. Some examples...

    uvLink = tag('a', _class='ultraviolet')
    warning = tag('strong')[
                  'Your spam is on ',
                  uvLink(href='fire_safety.html')[ 'fire' ],
              ]

    page = tag('html')[
               tag('head')[
                   tag('title')[ 'Slinkies Around the World' ],
               ],
               tag('body')[
                   warning,
               ],
           ]

All tag objects are renderable- that is, they have a 'render' method that
can be called to convert that tag and its contents recursively to an 'xml'
object. Normally, Nouvelle quotes all strings appropriately. xml objects
are just special string objects that signify data that's already in XML.

Any python object can be embedded inside a tag. Renderable objects will
have their 'render' method called, and the returned data will be processed
in place of the renderable objects. Lists and tuples will have their contents
concatenated. Strings are quoted and inserted into the resulting document.
Other objects are casted to strings.

It is common to include tag trees as class attributes. This is efficient,
as the tag tree only needs to be constructed once, and during initialization
the 'tag' object can pre-serialize itself. However, it also means the tags
can't contain any dynamic content. This is where the 'place' renderable
comes in handy. It acts as a placeholder for dynamic information.

A place instance is created with the name of a piece of information, and
optionally with extra parameters. If the object owning the current tag tree
(context['owner']) has a render_* method for the given name, it is called
to return a value for the placeholder. If not, the placeholder will look
for an attribute by the given name and call it. This is helpful when you
want to insert information that is usually static, but should be overridable
by subclasses. For example:

class WebThingy(DocumentOwner):
    title = 'Default Title'

    def render_body(self, context):
        return fetchSomeData()

    document = [
        tag('h3')[ place('title') ],
        tag('p')[ place('body') ],
        ]

The DocumentOwner class our hypothetical WebThingy subclasses can be anything
that context['owner'] to itself before serializing self.document. The 'context'
dictionary is passed to all render() functions, starting with that of the
Serializer. It keeps track of important extra data, without requiring rendering
functions to know what that data might be. It can be used to hold information
about the current web request, for example.
"""
#
# Nouvelle web framework
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

# Check the python version here before we proceed further
requiredPythonVersion = (2,2,1)
import sys, string
if sys.version_info < requiredPythonVersion:
    raise Exception("%s requires at least Python %s, found %s instead." % (
        name,
        string.join(map(str, requiredPythonVersion), "."),
        string.join(map(str, sys.version_info), ".")))
del sys
del string

# Convenience imports
import Serial, Table
from Serial import *
from Table import *

### The End ###
