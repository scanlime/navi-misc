""" LibCIA.Web.ServerPages

Web pages used internally to the server, including the "internal server error"
page and a directory listing page.
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

from twisted.protocols import http
from Nouvelle import tag, place, Twisted
from LibCIA import TimeUtil, Units
import Template
import time, Nouvelle


class InternalErrorPage(Twisted.Page):
    """An internal server error page, generated when we encounter an exception
       generating the intended page. This page should be fairly simple, so there's
       little risk in it causing an exception also.
       """
    def __init__(self, failure):
        self.failure = failure

    def preRender(self, context):
        request = context['request']
        request.setHeader('content-type', "text/html")
        request.setResponseCode(http.INTERNAL_SERVER_ERROR)

    def render_time(self, context):
        return TimeUtil.formatDateRFC822(time.time())

    def render_excType(self, context):
        return str(self.failure.value.__class__)

    def render_excValue(self, context):
        return str(self.failure.value)

    def render_traceback(self, context):
        return self.failure.getTraceback()

    def render_uri(self, context):
        return context['request'].uri

    document = tag('html')[
                   tag('head')[
                       tag('title')[ "Internal Server Error" ],
                   ],
                   tag('body')[
                       tag('h2')[ "Internal Server Error" ],

                       # Friendly message
                       tag('p')[
                           "Sorry, it looks like you just found a bug. If you would like to "
                           "help us identify the problem, please email a copy of this page to the "
                           "webmaster of this site along with a description of what happened. Thanks!"
                       ],

                       # Table of useful values
                       tag('table', cellpadding=5) [
                           tag('tr')[
                               tag('td')[ tag('b')[ 'Current time:' ]],
                               tag('td')[ place('time') ],
                           ],
                           tag('tr')[
                               tag('td')[ tag('b')[ 'Requested path:' ]],
                               tag('td')[ place('uri') ],
                           ],
                           tag('tr')[
                               tag('td')[ tag('b')[ 'Exception type:' ]],
                               tag('td')[ place('excType') ],
                           ],
                           tag('tr')[
                               tag('td')[ tag('b')[ 'Exception value:' ]],
                               tag('td')[ place('excValue') ],
                           ],
                       ],

                       # Traceback
                       tag('p')[
                           tag('b')[ 'Traceback:' ],
                       ],
                       tag('p')[
                           tag('pre')[ place('traceback') ],
                       ],
                   ],
               ]


class DirectoryListing(Template.Page):
    """Generates pretty listings for static directories"""
    def __init__(self, dir):
        self.dir = dir
        self.mainTitle = self.dir.basename()
        self.subTitle = 'directory listing'

        self.leftColumn = [
            DirectoryTreeSection(self.dir)
            ]

        self.mainColumn = [
            DirectoryContentsSection(self.dir)
            ]


class DirectoryTreeSection(Template.Section):
    """A section showing this directory's location in the tree"""
    title = 'tree'

    def __init__(self, dir):
        self.dir = dir

    def render_rows(self, context):
        """Assemble our file tree in the form of nested dictionaries,
           then let Template.FileTree render it.
           """
        request = context['request']
        tree = {}
        node = tree
        urlSegments = ['']

        # Walk down the tree putting in our ancestor directories
        # (not counting the root directory, since navigating to
        # it is better done using our tabs or site name link)
        for path in request.prepath:
            if path:
                urlSegments.append(path)
                link = tag('a', href="/".join(urlSegments))[ path ]
                node = node.setdefault(link, {})

        return [Template.FileTree(tree)]


class FileNameColumn(Nouvelle.Column):
    heading = 'name'

    def getValue(self, file):
        return file.basename()

    def render_data(self, context, file):
        """We hyperlink the file name, and give it an icon to indicate directory vs file"""
        if file.isdir():
            icon = Template.dirIcon
        else:
            icon = Template.fileIcon
        name = file.basename()
        return [
            icon,
            tag('a', href=name)[ name ],
            ]


class FileSizeColumn(Nouvelle.Column):
    heading = 'size'

    def __init__(self):
        self.formatter = Units.StorageUnits().format

    def getValue(self, file):
        return file.getFileSize()

    def render_data(self, context, file):
        if file.isfile():
            return self.formatter(file.getFileSize())
        else:
            return ''


class DirectoryContentsSection(Template.Section):
    """A section showing this directory's contents"""
    title = 'directory contents'

    columns = [
        FileNameColumn(),
        FileSizeColumn(),
        ]

    def __init__(self, dir):
        self.dir = dir

    def render_rows(self, context):
        files = self.dir.listEntities()
        if files:
            return [Template.Table(files, self.columns, id="contents")]
        else:
            return ["No visible files"]

### The End ###
