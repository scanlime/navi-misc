""" LibCIA.Web.Stats.Metadata

Viewers and editors for the metadata associated with each stats target
"""
#
# CIA open source notification system
# Copyright (C) 2003-2007 Micah Dowty <micah@navi.cx>
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
from twisted.web import resource, server, error, static
from twisted.protocols import http
from LibCIA.Web import Template, Keyring
from LibCIA import Units, Stats, RpcServer
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
            rows.append(Template.Photo(Link.ThumbnailLink(self.target, 'photo', (250,400)).getURL(context)))
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
        # The alt attribute here is rather useless, but required by XHTML
        return Link.ThumbnailLink(context['target'], name, (128,128))

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


class MetadataDeleteColumn(Nouvelle.Column):
    """A column that, when a key is set, shows a delete button for each item"""
    heading = 'delete'

    def isVisible(self, context):
        return Keyring.getKeyring(context).hasKey

    def render_data(self, context, item):
        return tag('form', method="post", action=Keyring.getSecureURL(context))[
            tag('input', _type='hidden', _name='metadata-key', value=item[0]),
            tag('input', _type='hidden', _name='metadata-delete', value=1),
            tag('input', _type='submit', value='Delete'),
            ]


class MetadataViewSection(Template.Section):
    """A section displaying a table of metadata keys for one stats target"""
    title = "view metadata"

    columns = [
        MetadataKeyColumn(),
        MetadataValueColumn(),
        MetadataDeleteColumn(),
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


class MetadataUploadSection(Template.Section):
    """A section providing a way to upload metadata, only visible if
       a key has been set with the Keyring.
       """
    title = "upload metadata"

    def __init__(self, target):
        self.target = target

    def render_rows(self, context):
        keyring = Keyring.getKeyring(context)
        if not keyring.hasKey:
            return []

        return [tag('form', method="post", enctype="multipart/form-data",
                    action=Keyring.getSecureURL(context))[
                    tag('table')[
                        tag('tr')[
                            tag('td')[ "Metadata key:" ],
                            tag('td')[ tag('input', _type='text', _name='metadata-key', size=30) ],
                        ],
                        tag('tr')[
                            tag('td')[ "MIME type:" ],
                            tag('td')[ tag('input', _type='text', _name='metadata-type', size=30, value="text/plain") ],
                        ],
                        tag('tr')[
                            tag('td')[ "Set value to:" ],
                            tag('td')[ tag('textarea', _name='metadata-value', rows=6, cols=50)[ [""] ]],
                        ],
                        tag('tr')[
                            tag('td')[ "Upload value from file:" ],
                            tag('td')[ tag('input', _type='file', _name='metadata-value-file') ],
                        ],
                        tag('tr')[
                            tag('td')[ " " ],
                            tag('td')[ tag('input', _type='submit', value="Set Metadata") ],
                        ],
                    ]
                ]]


class MetadataValuePage(resource.Resource):
    """A web resource that returns the raw value of a metadata key, with the proper MIME type"""
    def __init__(self, target, key):
        self.target = target
        self.key = key
        resource.Resource.__init__(self)
        self.putChild('.thumbnail', ThumbnailRootPage(target, key))

    def render(self, request):
        # Retrieve the metadata value and modification time first
        defer.gatherResults([
            self.target.metadata.get(self.key),
            self.target.metadata.getMTime(self.key),
            ]).addCallback(
            self._render, request
            ).addErrback(request.processingFailed)
        return server.NOT_DONE_YET

    def renderNoResource(self, request):
        """Render a 404 error for this request"""
        request.write(error.NoResource("No such metadata key %r" % self.key).render(request))
        request.finish()

    def _render(self, resultList, request):
        t, mtime = resultList
        if not t:
            return self.renderNoResource(request)

        value, mimeType = t
        request.setHeader('content-type', mimeType)

        # If this was a conditional If-Modified-Since request,
        # this will send a NOT_MODIFIED response and no data when
        # necessary. If we're sending back data, this automatically
        # sticks on the right Last-Modified header. This will let
        # browsers cache metadata intelligently, yay.
        # Note the short circuit logic necessary to avoid the
        # setLastModified call if we don't know what the modification
        # time is.
        if (mtime is None) or (request.setLastModified(mtime) is not http.CACHED):
            request.write(value)
        request.finish()


class ThumbnailRootPage(resource.Resource):
    """A web resource whose children are thumbnails of image metadata.
       This allows URLs like /stats/some/target/.metadata/photo/.thumbnail/300x400
       """
    def __init__(self, target, key):
        self.target = target
        self.key = key
        resource.Resource.__init__(self)

    def render(self, request):
        return error.NoResource("There's nothing here, it's just an object that generates thumbnails").render(request)

    def getChildWithDefault(self, name, request):
        if not name:
            # Ignore empty path segments
            return self

        # The child name is a thumbnail size
        try:
            width, height = map(int, name.split("x"))
        except ValueError:
            return error.NoResource("Improperly formatted thumbnail size")
        return ThumbnailPage(self.target, self.key, (width, height))


class ThumbnailPage(MetadataValuePage):
    """A web resource that returns a dynamically generated and cached thumbnail
       of a metadata value that happens to be an image.
       """
    def __init__(self, target, key, size):
        self.target = target
        self.key = key
        self.size = size
        resource.Resource.__init__(self)

    def render(self, request):
        # Retrieve the thumbnail and the metadata key's modification time.
        # This uses the same _render callback as MetadataValuePage.
        defer.gatherResults([
            self.target.metadata.getThumbnail(self.key, self.size),
            self.target.metadata.getMTime(self.key),
            ]).addCallback(
            self._render, request
            ).addErrback(request.processingFailed)
        return server.NOT_DONE_YET

    def _render(self, resultList, request):
        t, mtime = resultList
        if not t:
            return self.renderNoResource(request)

        fileObject, mimeType = t
        request.setHeader('content-type', mimeType)

        if (mtime is not None) and (request.setLastModified(mtime) is http.CACHED):
            # setLastModified handles sending our mtime to the client. If the client
            # asked to only retrieve this resource if it's changed, don't send it.
            request.finish()
            return

        # Get twisted.static to send over our file efficiently
        fileObject.seek(0, 2)
        size = fileObject.tell()
        fileObject.seek(0)
        request.setHeader('content-length', str(size))
        static.FileTransfer(fileObject, size, request)


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
        # We process our upload results, if necessary, before
        # doing anything with the main column. This is necessary
        # to avoid race conditions with displaying and setting the
        # same metadata, and we also need this for showing errors.
        result = defer.Deferred()
        self.processUploadResults(context).addBoth(
            self._render_mainColumn, context, result)
        return result

    def _render_mainColumn(self, uploadResult, context, result):
        # uploadResult will be None or a Failure instance
        sections = []

        if uploadResult:
            sections.append(Template.StaticSection("error", [[
                tag('strong')[ str(uploadResult.type) ],
                tag('p')[ str(uploadResult.value) ],
                ]]))

        sections.extend([
            MetadataViewSection(self.statsPage.target),
            MetadataUploadSection(self.statsPage.target),
            ])
        result.callback(sections)

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

    def processUploadResults(self, context):
        """Process the results of submitting our metadata modification form,
           returns a Deferred indicating completion of the operation.
           """
        args = context['request'].args
        keyring = Keyring.getKeyring(context)
        path = self.statsPage.target.path
        iRoot = RpcServer.getRootInterface()

        name = args.get('metadata-key', (None,))[0]
        mimeType = args.get('metadata-type', (None,))[0]
        value = args.get('metadata-value', (None,))[0]
        valueFile = args.get('metadata-value-file', (None,))[0]
        delete = int(args.get('metadata-delete', (0,))[0])

        if not name:
            # Nothing to do, return an already complted Deferred
            result = defer.Deferred()
            result.callback(None)
            return result

        # A file takes precedence over the entry field if we have one
        value = valueFile or value

        # We actually do the metadata setting via the RPC tree,
        # so we can ensure it has the exact same security semantics.
        if delete:
            return iRoot.call('stats.metadata.remove', keyring.key, path, name)
        else:
            return iRoot.call('stats.metadata.set', keyring.key, path, name, value, mimeType)

    leftColumn = [
        Keyring.SecuritySection(
            "To modify metadata, you must have a key granting the "
            "appropriate capabilities. If you have a key, enter it below "
            "to begin editing metadata."
        ),
        Template.StaticSection('information', [
            "This page lists all the metadata associated with a particular stats target.",
            "A stats target is anything that can keep track of a particular category of "
            "messages and/or holds other stats targets. Metadata for these stats targets "
            "control how they are displayed in the stats browser.",
        ]),
    ]

### The End ###
