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
from LibCIA.Web import Template
from LibCIA import Units, Database
from Nouvelle import tag


class Info(Template.Section):
    """A section that displays a StatsTarget's miscellaneous metadata"""
    title = "information"

    def __init__(self, target):
        self.target = target
        self.metadata = target.metadata

    def render_rows(self, context):
        photo_query = """
        SELECT IM.path, IM.width, IM.height
        FROM stats_statstarget ST
        LEFT OUTER JOIN images_imageinstance IM
        ON (IM.source_id = ST.photo_id AND IM.thumbnail_size = 256)
        WHERE ST.path = %s
        """ % Database.quote(self.target.path, 'varchar')

        # Grab the metadata keys we'll need and wait for them to become available
        result = defer.Deferred()
        defer.gatherResults([
            self.metadata.getValue('url'),
            self.metadata.getValue('description'),
            Database.pool.runQuery(photo_query),
            ]).addCallback(self._render_rows, context, result).addErrback(result.errback)
        return result

    def _render_rows(self, metadata, context, result):
        url, description, photo_results = metadata
        rows = []
        if url:
            rows.append(tag('a', href=url)[url])
        if photo_results and photo_results[0][0]:
            path, width, height = photo_results[0]
            rows.append(Template.Photo('/images/db/' + path, width=width, height=height))
        if description:
            rows.append(description)
        result.callback(rows)


### The End ###
