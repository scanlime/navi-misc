""" LibCIA.Web.Stats.Metadata

Viewers and editors for the metadata associated with each stats target
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

from twisted.internet import defer
from twisted.web import resource, server, error
from LibCIA.Web import Template
from LibCIA import Units
from Nouvelle import tag, subcontext
import Nouvelle
import Link


class Info(Template.Section):
    """A section that displays a StatsTarget's miscellaneous metadata"""
    title = "information"

    def __init__(self, target):
        self.target = target
        self.metadata = target.metadata

    def render_rows(self, context):
        # Grab the metadata keys we'll need and wait for them to become available
        result = defer.Deferred()
        defer.gatherResults([
            self.metadata.getValue('url'),
            self.metadata.getValue('description'),
            self.metadata.has_key('photo'),
            ]).addCallback(self._render_rows, context, result).addErrback(result.errback)
        return result

    def _render_rows(self, metadata, context, result):
        url, description, hasPhoto = metadata
        rows = []
        if url:
            rows.append(tag('a', href=url)[url])
        if hasPhoto:
            rows.append(Template.Photo(Link.MetadataLink(self.target, 'photo').getURL(context)))
        if description:
            rows.append(description)
        result.callback(rows)


class MetadataKeyColumn(Nouvelle.Column):
    """A column that displays a metadata key as a hyperlink to the proper MetadataValuePage.
       Rows are expected to be (name, (value, type)) tuples.
       """
    heading = 'key'

    def getValue(self, item):
        return item[0]

    def render_data(self, context, item):
        return Link.MetadataLink(context['target'], item[0])


class MetadataValueColumn(Nouvelle.Column):
    """A column that displays a metadata key's associated value, formatted appropriately.
       Rows are expected to be (name, (value, type)) tuples.
       """
    heading = 'value'

    def getValue(self, item):
        """This is used for sorting and such, so don't bother
           returning anything if we're not dealing with text.
           """
        value, mime = item[1]
        if mime.startswith("text/"):
            return value

    def render_data(self, context, item):
        """Farm this off to an appropriate handler for the data's MIME type"""
        value, mime = item[1]
        try:
            # First look for a handler for one particular MIME type after
            # replacing the / with an underscore
            f = getattr(self, 'renderData_' + mime.replace('/', '_'))
        except AttributeError:
            try:
                # Nope, try looking for a handler for only the content type,
                # not the subtype
                f = getattr(self, 'renderData_' + mime.split('/')[0])
            except AttributeError:
                # Nothing, use a generic handler
                f = self.renderData_other
        return f(context, item[0], value, mime)

    def renderData_text(self, context, name, value, mime):
        return value

    def renderData_image(self, context, name, value, mime):
        """Return an <img> tag linking to the key's value"""
        return tag('img', src=Link.MetadataLink(context['target'], name).getURL(context))

    def renderData_other(self, context, name, value, mime):
        return Template.unableToFormat


class MetadataTypeColumn(Nouvelle.Column):
    """A column that displays a metadata key's MIME type.
       Rows are expected to be (name, (value, type)) tuples.
       """
    heading = 'type'

    def getValue(self, item):
        return item[1][1]


class MetadataSizeColumn(Nouvelle.Column):
    """A column that displays the size of a metadata key's value in bytes.
       Rows are expected to be (name, (value, type)) tuples.
       """
    heading = 'size'

    def __init__(self):
        self.formatter = Units.StorageUnits().format

    def getValue(self, item):
        return len(item[1][0])

    def render_data(self, context, item):
        return self.formatter(self.getValue(item))


class MetadataSection(Template.Section):
    """A section displaying a table of metadata keys for one stats target"""
    title = "metadata"

    columns = [
        MetadataKeyColumn(),
        MetadataValueColumn(),
        MetadataTypeColumn(),
        MetadataSizeColumn(),
        ]

    def __init__(self, target):
        self.target = target

    def render_rows(self, context):
        # Look up all the metadata first
        result = defer.Deferred()
        self.target.metadata.dict().addCallback(
            self._render_rows, context, result).addErrback(result.errback)
        return result

    def _render_rows(self, metadict, context, result):
        # Each 'row' in our table is a (name, (value, type)) tuple
        rows = metadict.items()
        result.callback([
            subcontext(target=self.target)[
                Template.Table(rows, self.columns, id='metadata')
            ]])


class MetadataValuePage(resource.Resource):
    """A web resource that returns the raw value of a metadata key, with the proper MIME type"""
    def __init__(self, target, key):
        self.target = target
        self.key = key
        resource.Resource.__init__(self)

    def render(self, request):
        # Retrieve the metadata value, rendering the page once it arrives
        self.target.metadata.get(self.key).addCallback(self._render, request).addErrback(request.processingFailed)
        return server.NOT_DONE_YET

    def _render(self, t, request):
        if t:
            value, mimeType = t
            request.setHeader('content-type', mimeType)
            request.write(value)
            request.finish()
        else:
            request.write(error.NoResource("No such metadata key %r" % self.key).render(request))
            request.finish()


class MetadataPage(Template.Page):
    """A web page providing an interface for viewing and editing a StatsTarget's
       metadata. Children of this page are pages that render individual metadata keys
       with no extra formatting.
       """
    subTitle = "Tabloid footprints in your hair, Tabloid footprints everywhere."

    def __init__(self, statsPage):
        self.statsPage = statsPage

    def parent(self):
        return self.statsPage

    def preRender(self, context):
        context['component'] = self.statsPage.component

    def render_mainTitle(self, context):
        return "Metadata for stats://%s" % "/".join(self.statsPage.target.pathSegments)

    def render_mainColumn(self, context):
        return [
            MetadataSection(self.statsPage.target),
            ]

    def getChildWithDefault(self, name, request):
        """Part of IResource, called by twisted.web to retrieve pages for URIs
           below this one. Children of a MetadataPage are MetadataValuePages
           """
        if not name:
            # Ignore empty path sections
            return self
        else:
            return MetadataValuePage(self.statsPage.target, name)

    def render_headingTabs(self, context):
        tabs = self.statsPage.render_headingTabs(context)
        tabs.append(Link.StatsLink(self.statsPage.target, Template.headingTab))
        return tabs

    leftColumn = [
        Template.StaticSection('information', [
        "This page lists all the metadata associated with a particular stats target.",
        "A stats target is anything that can keep track of a particular category of "
        "messages and/or holds other stats targets. Metadata for these stats targets "
        "control how they are displayed in the stats browser.",
        ])]

### The End ###
