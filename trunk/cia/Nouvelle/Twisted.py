""" Nouvelle.Twisted

Glue to help interface Nouvelle with twisted.web.
This includes a twisted.web.resource that renders a Nouvelle document.
"""
#
# Nouvelle web framework
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

import Nouvelle
from twisted.web import resource


class Page(resource.Resource):
    """A web resource that renders a tree of tag instances from its 'document' attribute"""
    serializerFactory = Nouvelle.Serializer

    def render(self, request):
        context  = {
            'owner': self,
            'request': request,
            'args': request.args,  # For compatibility across systems utilizing Nouvelle
            }
        self.preRender(context)
        return str(self.serializerFactory().render(self.document, context))

    def preRender(self, context):
        """Called prior to rendering each request, subclasses can use this to annotate
           'context' with extra information or perform other important setup tasks.
           """
        pass

### The End ###
