""" Nouvelle.Twisted

Glue to help interface Nouvelle with twisted.web.
This includes a twisted.web.resource that renders a Nouvelle document,
and support for asynchronous rendering using Deferred.
"""
#
# Nouvelle web framework
# Copyright (C) 2003-2005 Micah Dowty <micahjd@users.sourceforge.net>
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
from twisted.internet import defer
from twisted.protocols import http
from twisted.web import server, error, resource


class TwistedSerializer(Nouvelle.Serializer):
    """A subclass of Nouvelle.Serializer that understands twisted's Deferred
       objects and can render web pages asynchronously. If at any time a Deferred
       object is encountered, a new Deferred object is created to represent the
       serialized form of the original Deferred. If a Deferred is left over after
       rendering the complete document, the document is rendered in the Deferred's
       callback and our render function returns server.NOT_DONE_YET.
       """
    def render_Deferred(self, obj, context):
        """A handler for rendering Deferred instances. This returns a new Deferred
           instance representing the first deferred's serialized form.
           """
        result = defer.Deferred()
        obj.addCallbacks(self.deferredRenderCallback,
                         result.errback,
                         callbackArgs = (context, result),
                         )
        return result

    def deferredRenderCallback(self, obj, context, result):
        """A Deferred callback that renders data when it becomes available, adding the
           result to our 'result' deferred.
           """
        r = self.render(obj, context)
        # If the render result was a deferred, chain it to our result
        if isinstance(r, defer.Deferred):
            r.addCallback(result.callback).addErrback(result.errback)
        else:
            result.callback(r)
        return obj

    def render_list(self, obj, context):
        """A new version of render_list that returns a Deferred
           if any item in the list is a Deferred.
           """
        # Render each item in the list, noting whether we have any Deferreds
        renderedItems = []
        hasDeferreds = False
        for item in obj:
            rendered = self.render(item, context)
            renderedItems.append(rendered)
            if isinstance(rendered, defer.Deferred):
                hasDeferreds = True

        # If we had any deferred items, we need to create a DeferredList
        # so we get notified once all of our content is rendered. Otherwise
        # we can join them now.
        if hasDeferreds:
            result = defer.Deferred()

            # Convert every result to a Deferred, wrapping non-deferred
            # objects in deferreds that are already completed.
            deferreds = []
            for item in renderedItems:
                if not isinstance(item, defer.Deferred):
                    d = defer.Deferred()
                    d.callback(item)
                    item = d
                deferreds.append(item)

                # Propagate errors to our parent. This is necessary
                # even with an errback in our DeferredList, as the
                # DeferredList doesn't absorb errors, it only detects them.
                item.addErrback(self.listRenderErrback, result)

            # Wait until all deferreds are completed
            dl = defer.DeferredList(deferreds)
            dl.addCallbacks(self.listRenderCallback,
                            self.listRenderErrback,
                            callbackArgs = (context, result),
                            errbackArgs = (result,),
                            )
            return result
        else:
            return xml(''.join(renderedItems))

    def listRenderCallback(self, obj, context, result):
        results = []
        if result.called:
            return obj
        for success, item in obj:
            if success:
                results.append(str(item))
            else:
                # If we had any failures, our listRenderErrback
                # should have already been called. Ignore this.
                return
        result.callback(xml(''.join(results)))
        return obj

    def listRenderErrback(self, failure, result):
        # Propagate this error to our result if it hasn't
        # already seen an error. (This ignores all but the first
        # error if we had multiple failures)
        if not result.called:
            result.errback(failure)


class Page(resource.Resource):
    """A web resource that renders a tree of tag instances from its 'document' attribute"""
    serializer = TwistedSerializer()

    def render(self, request):
        context  = {
            'owner': self,
            'request': request,
            'args': request.args,  # For compatibility across systems utilizing Nouvelle
            }
        defer.maybeDeferred(self.preRender, context).addCallback(
            self._afterPreRender, context).addErrback(self.pageErrorCallback, context)
        return server.NOT_DONE_YET

    def _afterPreRender(self, preRenderResult, context):
        if preRenderResult is not None:
            if type(preRenderResult) in (str, unicode):
                context['request'].write(preRenderResult)
                context['request'].finish()
            else:
                assert preRenderResult == server.NOT_DONE_YET
            return

        request = context['request']
        defer.maybeDeferred(self.serializer.render, self.document, context).addCallback(
            self.pageFinishedCallback, context).addErrback(self.pageErrorCallback, context)

    def pageFinishedCallback(self, obj, context):
        """Callback for asynchronous page rendering from a Deferred object"""
        context['request'].write(str(obj))
        context['request'].finish()

    def pageErrorCallback(self, failure, context):
        """Error handler for pages rendered asynchronously"""
        context['request'].processingFailed(failure)

    def preRender(self, context):
        """Called prior to rendering each request, subclasses can use this to annotate
           'context' with extra information or perform other important setup tasks.
           If this returns a Deferred, rendering will be delayed until it is resolved.
           If it returns anything but None, normal rendering is aborted and render() returns
           that value.
           """
        pass

### The End ###
