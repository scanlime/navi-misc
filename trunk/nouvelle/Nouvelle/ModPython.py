""" Nouvelle.ModPython

Glue for using Nouvelle with mod_python. Page objects here, once instantiated,
can be rendered by the publisher handler using their 'publish' method or 'handler'
can be used to implement a standalone request handler.
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
from mod_python import apache

class Page:
    """A web resource that renders a tree of tag instances from its 'document'
       attribute when called.
       """
    serializerFactory = Nouvelle.Serializer
    contentType = "text/html"
    isLeaf = True

    def publish(self, **kwargs):
        """Render this page via the publisher handler"""
        request = kwargs['req']
        del kwargs['req']
        context = dict(owner=self, request=request, args=kwargs)
        self.preRender(context)
        return self.render(context)

    def handler(self, req):
        """Implement a standalone request handler. This can handle child requests
           through the getChild() method.
           """
        # Recurse into child pages as necessary
        current = self
        for seg in req.path_info.split('/'):
            if not seg:
                continue
            try:
                current = current.getChild(seg)
            except KeyError:
                return apache.HTTP_NOT_FOUND
            if not current:
                return apache.HTTP_NOT_FOUND

        # FIXME: support for arguments
        context = dict(owner=current, request=req, args={})
        current.preRender(context)
        return current.render(context)

    def render(self, context):
        """Render the current page, writing the results via the context's request object"""
        req = context['request']

        # If this isn't a leaf resource and doesn't end with a slash, redirect there
        if not self.isLeaf and not req.uri.endswith("/"):
            self.redirect(req, req.uri + "/")

        page = str(self.serializerFactory().render(self.document, context))
        req.content_type = self.contentType
        req.write(page)
        return apache.OK

    def redirect(self, req, url, temporary=False, seeOther=False):
        """Immediately redirects the request to the given url. If the
           seeOther parameter is set, 303 See Other response is sent, if the
           temporary parameter is set, the server issues a 307 Temporary
           Redirect. Otherwise a 301 Moved Permanently response is issued.

           This function is from Stian Soiland's post to mod_python's mailing list at:
           http://www.modpython.org/pipermail/mod_python/2004-January/014865.html
           """
        from mod_python import apache

        if seeOther:
            status = apache.HTTP_SEE_OTHER
        elif temporary:
            status = apache.HTTP_TEMPORARY_REDIRECT
        else:
            status = apache.HTTP_MOVED_PERMANENTLY

        req.headers_out['Location'] = url
        req.status = status
        raise apache.SERVER_RETURN, status

    def preRender(self, context):
        """Called prior to rendering each request, subclasses can use this to annotate
           'context' with extra information or perform other important setup tasks.
           """
        pass

    def getChild(self, name):
        """Return a child resource. The default implementation looks in a 'children' database,
           but this can be overridden by subclasses to implement more dynamic behavior.
           """
        d = getattr(self, 'children', {})
        return d.get(name)

### The End ###
