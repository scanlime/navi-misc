""" LibCIA.Web.Stats.Graph

Implements web interfaces based on the stats_relations graph.
This includes the 'related' section on all stats targets, and
visually graphing the relationships between stats targets.
"""
#
# CIA open source notification system
# Copyright (C) 2003-2005 Micah Dowty <micah@navi.cx>
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
import Nouvelle
from twisted.web import resource, server
import LibCIA.Stats.Graph
import Link, Columns


class RelatedTable(Nouvelle.BaseTable):
    tableTag = tag('table', _class='related')


class RelatedFilter(Database.Filter):
    """An XML filter that can control what's displayed in the 'related' box
       on a per-stats-target basis. These filters come from stats metadata,
       so we need to restrict the available SQL variables.
       """
    def varLookup(self, var):
        # Map from some abstract variable names to concrete variable names.
        # The returned variables must match the ones in RelatedSection.query below.
        return {
            'parent_path': 'C.parent_path',
            'target_path': 'C.target_path',
            }[var]


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
        R.freshness,
        PARENT_TITLE.value,
        C.target_path,
        TARGET_TITLE.value,
        IF(M_ICON.name IS NOT NULL, M_ICON.name, M_PHOTO.name)
    FROM stats_relations R
        LEFT OUTER JOIN stats_catalog C
            ON (C.target_path = target_%(otherSide)s_path)
        LEFT OUTER JOIN stats_metadata TARGET_TITLE
            ON (TARGET_TITLE.name = 'title' AND TARGET_TITLE.target_path = C.target_path)
        LEFT OUTER JOIN stats_metadata PARENT_TITLE
            ON (PARENT_TITLE.name = 'title' AND PARENT_TITLE.target_path = C.parent_path)
        LEFT OUTER JOIN stats_metadata M_PHOTO
            ON (C.target_path = M_PHOTO.target_path AND M_PHOTO.name = 'photo')
        LEFT OUTER JOIN stats_metadata M_ICON
            ON (C.target_path = M_ICON.target_path  AND M_ICON.name  = 'icon')
        WHERE R.target_%(thisSide)s_path = %(path)s
            AND C.parent_path != %(path)s
            AND %(filter)s
    ORDER BY NULL
    """

    sectionLimit = 15

    columns = [
        Columns.IndexedIconColumn(iconIndex=4, pathIndex=2),
        Columns.TargetTitleColumn(pathIndex=2, titleIndex=3),
        ]

    def __init__(self, target):
        self.target = target

    def makeLink(self, path, title):
        """Link to a stats target when we already know the title"""
        target = Stats.Target.StatsTarget(path)
        if title is None:
            title = target.name
        return Link.StatsLink(target, text=title)

    def render_rows(self, context):
        # First look for a related-filter metadata key for this target.
        # The default allows all related items to be displayed.
        result = defer.Deferred()
        self.target.metadata.getValue("related-filter", default="<true/>").addCallback(
            self._startQuery, context, result
            ).addErrback(result.errback)
        return result

    def _startQuery(self, filter, context, result):
        # We have our related-filter, start a db interaction to do our actual
        # queries then pass the results on to _render_rows.
        Database.pool.runInteraction(self._runQuery, filter).addCallback(
            self._render_rows, context, result
            ).addErrback(result.errback)

    def _runQuery(self, cursor, filter):
        # Set up and run two SQL queries, one for each side of the graph link that this
        # target may be on. We can't do this in one step and still have the server use
        # its indexes effectively.

        filterSql = RelatedFilter(filter).sql
        sections = {}

        for thisSide, otherSide in ( ('a','b'), ('b', 'a') ):
            cursor.execute(self.query % dict(
                path = Database.quote(self.target.path, 'varchar'),
                filter = filterSql,
                thisSide = thisSide,
                otherSide = otherSide,
                ))

            while 1:
                row = cursor.fetchone()
                if not row:
                    break
                # Drop rows into sections according to their parent path
                try:
                    sections[row[0]].append(row[1:])
                except KeyError:
                    sections[row[0]] = [row[1:]]

        # Sort sections descending by freshness
        for items in sections.itervalues():
            items.sort()
            items.reverse()
        return sections

    def _render_rows(self, queryResults, context, result):
        # Sort sections by decreasing size. We want the most interesting ones at
        # the top, and those are usually the biggest.
        sections = queryResults.keys()
        sections.sort(lambda a,b: cmp(len(queryResults[b]), len(queryResults[a])))
        result.callback([self.render_section(section, queryResults[section]) for section in sections])

    def render_section(self, section, rows):
        """Given a heading renderable and a list of rows for that
           heading, render one section of the 'related' box.
           """
        # Truncate the rows if we need to
        if len(rows) > self.sectionLimit:
            footer = tag('div', _class='relatedFooter')[
                '(%d others)' % (len(rows) - self.sectionLimit)
                ]
            rows = rows[:self.sectionLimit]
        else:
            footer = ()

        parentTitle = rows[0][1]
        return [
            tag('div', _class='relatedHeading')[ self.makeLink(section, parentTitle) ],
            RelatedTable(rows, self.columns, showHeading=False),
            footer,
            ]


class GraphPage(resource.Resource):
    """Implements a web resource that generates rasterized and cached
       graphs from the stats_relations table.
       """
    def __init__(self, *args, **kwargs):
        self.grapher = Stats.Graph.RelationGrapher(*args, **kwargs)

    def render(self, request):
        """Figure out what format the user wants this graph in, and
           call one of our format_* functions.

           Each format function must return an object with a render(f)
           method that writes the finished page to a file-like object
           and returns a Deferred signalling completion.

           The format function probably also should be setting the
           content-type header to an appropriate value.
           """
        format = request.args.get('format', ['png'])[0]
        render = getattr(self, 'format_'+format)(request).render
        render(request).addCallback(
            self._renderDone, request).addErrback(
            request.processingFailed)
        return server.NOT_DONE_YET

    def _renderDone(self, x, request):
        request.finish()

    def format_dot(self, request):
        """Return the graph in its original .dot source format"""
        request.setHeader('content-type', 'text/plain')
        return Stats.Graph.RenderCache(self.grapher)

    def format_svg(self, request):
        """Perform graph layout but not rasterization, and return the SVG"""
        request.setHeader('content-type', 'image/svg+xml')
        graphCache = Stats.Graph.RenderCache(self.grapher)
        layout = Stats.Graph.GraphLayout(graphCache)
        return Stats.Graph.RenderCache(layout)

    def format_png(self, request, defaultWidth=600, maxWidth=2000):
        """Return a fully rasterized graph image at a specified
           size, with caching at every interesting stage.
           """
        # Let our user specify a width, up to a preset maximum.
        # Height will follow automatically to keep the aspect ratio correct.
        width = min(int(request.args.get('width', [defaultWidth])[0]), maxWidth)

        # Let the user specify a background color, or the empty string
        # to leave the background transparent.
        bg = request.args.get('bg', ['white'])[0]
        if not bg:
            bg = None

        request.setHeader('content-type', 'image/png')
        graphCache = Stats.Graph.RenderCache(self.grapher)
        layoutCache = Stats.Graph.RenderCache(Stats.Graph.GraphLayout(graphCache))
        raster = Stats.Graph.SvgRasterizer(layoutCache, width=width, background=bg)
        return Stats.Graph.RenderCache(raster)

### The End ###
