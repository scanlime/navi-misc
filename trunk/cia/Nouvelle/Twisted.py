""" Nouvelle.Twisted

Glue to help interface Nouvelle with twisted.web.
This includes a twisted.web.resource that renders a Nouvelle document,
and support for asynchronous rendering using Deferred.
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
from Nouvelle import xml
from twisted.web import resource
from twisted.internet import defer
from twisted.web import server


class TwistedSerializer(Nouvelle.Serializer):
    """A subclass of Nouvelle.Serializer that understands twisted's Deferred
       objects and can render web pages asynchronously. If at any time a Deferred
       object is encountered, a new Deferred object is created to represent the
       serialized form of the original Deferred. If a Deferred is left over after
       rendering the complete document, the document is rendered in the Deferred's
       callback and our render function returns server.NOT_DONE_YET.
       """
    def renderPage(self, obj, context):
        """A wrapper around render() that handles deferred rendering of the web page"""
        result = self.render(obj, context)
        if isinstance(result, defer.Deferred):
            result.addCallback(self.deferredRenderPage, context)
            return server.NOT_DONE_YET
        else:
            return str(result)

    def deferredRenderPage(self, obj, context):
        """Callback for asynchronous page rendering from a Deferred object"""
        context['request'].write(obj)
        context['request'].finish()

    def render(self, obj, context):
        # Deferreds aren't new-style classes (yet?) so we have to detect
        # them here rather than writing a render_* handler for them.
        if isinstance(obj, defer.Deferred):
            # Render the deferred when it produces a result, returning our own Deferred
            result = defer.Deferred()
            obj.addCallback(self.deferredRender, context, result)
            return result
        else:
            return Nouvelle.Serializer.render(self, obj, context)

    def deferredRender(self, obj, context, result):
        """A Deferred callback that renders data when it becomes available, adding the
           result to our 'result' deferred.
           """
        result.callback(self.render(obj, context))

    def render_list(self, obj, context):
        """A new version of render_list that returns a Deferred
           if any item in the list is a Deferred.
           """
        results = []
        hasDeferreds = False
        for item in obj:
            result = self.render(item, context)
            results.append(result)
            if isinstance(result, defer.Deferred):
                hasDeferreds = True

        # If we had any deferred items, we need to create a DeferredList
        # so we get notified once all of our content is rendered. Otherwise
        # we can join them now.
        if hasDeferreds:
            deferreds = []
            for result in results:
                # Wrap non-deferred objects in deferreds
                if not isinstance(result, defer.Deferred):
                    d = defer.Deferred()
                    d.callback(result)
                    result = d
                deferreds.append(result)
            dl = defer.DeferredList(deferreds)

            # Now once all items in our DeferredList are ready, join them
            result = defer.Deferred()
            dl.addCallback(self.deferredRenderList, context, result)
            return result
        else:
            return xml(''.join(results))

    def deferredRenderList(self, obj, context, result):
        result.callback(xml(''.join([result for success, result in obj])))


class Page(resource.Resource):
    """A web resource that renders a tree of tag instances from its 'document' attribute"""
    serializer = TwistedSerializer()

    def render(self, request):
        context  = {
            'owner': self,
            'request': request,
            'args': request.args,  # For compatibility across systems utilizing Nouvelle
            }
        self.preRender(context)
        return self.serializer.renderPage(self.document, context)

    def preRender(self, context):
        """Called prior to rendering each request, subclasses can use this to annotate
           'context' with extra information or perform other important setup tasks.
           """
        pass

### The End ###
