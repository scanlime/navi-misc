""" LibCIA.Web.Stats.Graph

Implements web interfaces based on the stats_relations graph.
This includes the 'related' section on all stats targets, and
visually graphing the relationships between stats targets.
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
from LibCIA.Web import Template
from LibCIA import Stats, Database
from Nouvelle import tag
from twisted.web import resource, server
import LibCIA.Stats.Graph
import Link


class RelatedSection(Template.Section):
    """A section showing links to related stats targets. This works by looking for
       nodes connected to this one in the stats_relations graph. The paths and
       titles of related nodes are fetched using one SQL query, for efficiency.

       The query sorts first by parent path, so we can extract each section in one
       piece, and second in descending order by freshness. This way, instead of
       constantly having the strongest associations at the top, every time an
       association is reinforced it pops up to the top, showing our visitors what's
       cool and hip.
       """
    title = 'related'

    query = """
    SELECT
        C.parent_path,
        PARENT_TITLE.value,
        C.target_path,
        TARGET_TITLE.value
    FROM stats_relations R
        LEFT OUTER JOIN stats_catalog C
            ON (C.target_path = IF(R.target_a_path = %(path)s, R.target_b_path, R.target_a_path))
        LEFT OUTER JOIN stats_metadata TARGET_TITLE
            ON (TARGET_TITLE.name = 'title' AND TARGET_TITLE.target_path = C.target_path)
        LEFT OUTER JOIN stats_metadata PARENT_TITLE
            ON (PARENT_TITLE.name = 'title' AND PARENT_TITLE.target_path = C.parent_path)
        WHERE (R.target_a_path = %(path)s or R.target_b_path = %(path)s)
            AND C.parent_path != %(path)s
    ORDER BY C.parent_path, R.freshness DESC
    """

    sectionLimit = 15

    def __init__(self, target):
        self.target = target

    def makeLink(self, path, title):
        """Link to a stats target when we already know the title"""
        target = Stats.Target.StatsTarget(path)
        if title is None:
            title = target.name
        return Link.StatsLink(target, text=title)

    def render_rows(self, context):
        # Run our big SQL query to get all data for this section first
        result = defer.Deferred()
        Database.pool.runQuery(self.query % {
            'path': Database.quote(self.target.path, 'varchar'),
            }).addCallback(
            self._render_rows, context, result
            ).addErrback(result.errback)
        return result

    def _render_rows(self, queryResults, context, result):
        # From the rows returned from our SQL query, construct a
        # dictionary that maps from a parent hyperlink to a list
        # of child hyperlinks sorted by decreasing freshness.
        currentParentLink = None
        currentParentPath = None
        d = {}
        for parentPath, parentTitle, targetPath, targetTitle in queryResults:
            if parentPath != currentParentPath:
                currentParentPath = parentPath
                currentParentLink = self.makeLink(parentPath, parentTitle)
            d.setdefault(currentParentLink, []).append(self.makeLink(targetPath, targetTitle))

        # Sort these parent sections by decreasing size. We want
        # the most interesting ones at the top, and those are usually the biggest.
        sections = d.keys()
        sections.sort(lambda a,b: cmp(len(d[b]), len(d[a])))
        result.callback([self.render_section(section, d[section]) for section in sections])

    def render_section(self, section, contents):
        """Given a heading renderable and a list of contents for that
           heading, render one section of the 'related' box.
           """
        # Truncate the contents if we need to
        if len(contents) > self.sectionLimit:
            contents = contents[:self.sectionLimit] + ['(%d others)' % (len(contents) - self.sectionLimit)]

        return [
            tag('div', _class='relatedHeading')[ section ],
            tag('ul', _class='related')[[
                tag('li', _class='related')[ item ]
                for item in contents
            ]],
        ]


class GraphPage(resource.Resource):
    """Implements a web resource that generates rasterized and cached
       graphs from the stats_relations table.
       """
    def __init__(self, *args, **kwargs):
        self.grapher = Stats.Graph.RelationGrapher(*args, **kwargs)
        self.layout = Stats.Graph.GraphLayout(self.grapher)

    def render(self, request):
        # Get the resolution from our page arguments, but put an upper
        # limit on it so we don't use all our memory trying to rasterize this.
        dpi = min(60, float(request.args.get('dpi',[3])[0]))

        # Cache the rasterized graph
        r = Stats.Graph.SvgRasterizer(self.layout, dpi=dpi, background='white')
        Stats.Graph.RenderCache().get(r).addCallback(
            self._render, request).addErrback(request.processingFailed)
        return server.NOT_DONE_YET

    def _render(self, image, request):
        request.setHeader('content-type', 'image/png')
        request.write(image)
        request.finish()

### The End ###
