""" Nouvelle.ModPython

Glue for using Nouvelle with mod_python. Page objects here, once instantiated,
can be rendered by mod_python's publisher handler.
"""
#
# Nouvelle web framework
# Copyright (C) 2003-2004 Micah Dowty <micahjd@users.sourceforge.net>
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

import Nouvelle
from Nouvelle import tag

class Page:
    """A web resource that renders a tree of tag instances from its 'document'
       attribute when called.
       """
    serializerFactory = Nouvelle.Serializer

    def __call__(self, **kwargs):
        request = kwargs['req']
        del kwargs['req']
        context = dict(owner=self, request=request, args=kwargs)
        self.preRender(context)
        return str(self.serializerFactory().render(self.document, context))

    def preRender(self, context):
        """Called prior to rendering each request, subclasses can use this to annotate
           'context' with extra information or perform other important setup tasks.
           """
        pass

### The End ###
