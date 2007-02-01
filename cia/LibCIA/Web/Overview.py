""" LibCIA.Web.Overview

A front page for CIA showing a concise overview of the day's open
source development activity.

Note that this is currently the *only* module in CIA that makes
assumptions about the way the stats hierarchy is organized. If you
have CIA set up differently, you will need to modify this page
or choose not to use it in your .tac file.
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

import random
import Template
from LibCIA import Database
from LibCIA.Stats import Target
from LibCIA.Web.Stats import Columns
from Nouvelle import tag, place
import Nouvelle
from twisted.internet import defer


class ActivitySection(Template.Section):
    """A Section displaying links to the most or least active items within
       a given stats target.
       """
    query = """
    SELECT
        STAT.target_path,
        ST.title,
        ICO.path,
        STAT.%(counter_attrib)s,
        ICO.width,
        ICO.height
    FROM stats_counters STAT FORCE INDEX (%(counter_attrib)s)
        LEFT OUTER JOIN stats_catalog  T           ON (STAT.target_path = T.target_path)
        LEFT OUTER JOIN stats_statstarget ST       ON (T.target_path = ST.path)
        LEFT OUTER JOIN images_imageinstance ICO   ON (ICO.source_id = IF(ST.icon_id IS NOT NULL, ST.icon_id, ST.photo_id) AND ICO.thumbnail_size = 32)
        WHERE STAT.name = %(counter)s AND T.parent_path = %(path)s
    ORDER BY STAT.%(counter_attrib)s %(sort)s
    LIMIT %(limit)d
    """

    def __init__(self, targetPath, title,
                 numItems      = 15,
                 counter       = 'today',
                 counterAttrib = 'event_count',
                 sort          = 'DESC',
                 columnTitle   = 'events today',
                 ):
        self.targetPath = targetPath
        self.title = title
        self.numItems = numItems
        self.counter = counter
        self.counterAttrib = counterAttrib
        self.sort = sort
        self.columnTitle = columnTitle
        self.title = title
        self.initQuery()
        self.initColumns()

    def initQuery(self):
        self.query = self.query % dict(
            path = Database.quote(self.targetPath, 'varchar'),
            limit = self.numItems,
            counter = Database.quote(self.counter, 'varchar'),
            counter_attrib = self.counterAttrib,
            sort = self.sort,
            )

    def initColumns(self):
        self.columns = [
            Columns.IndexedIconColumn(iconIndex=2, widthIndex=4, heightIndex=5),
            Columns.TargetTitleColumn(pathIndex=0, titleIndex=1),
            Columns.IndexedBargraphColumn(self.columnTitle, 3),
            ]

    def render_rows(self, context):
        # First we run a big SQL query to gather all the data for this catalog.
        # Control is passed to _render_rows once we have the query results.
        result = defer.Deferred()
        Database.pool.runQuery(self.query).addCallback(
            self._render_rows, context, result
            ).addErrback(result.errback)
        return result

    def _render_rows(self, queryResults, context, result):
        # Use a non-resortable table for this, since we've already dictated
        # how our data is sorted in the SQL query and the user can't do much about it yet.
        result.callback([Nouvelle.Table.BaseTable(list(queryResults), self.columns)])


class TimestampSection(ActivitySection):
    """A section showing the newest or oldest items within a stats target"""

    def __init__(self, targetPath, title,
                 numItems      = 15,
                 counter       = 'forever',
                 counterAttrib = 'first_time',
                 sort          = 'DESC',
                 columnTitle   = 'first event',
                 ):
        ActivitySection.__init__(self, targetPath, title, numItems,
                                 counter, counterAttrib, sort, columnTitle)

    def initColumns(self):
        self.columns = [
            Columns.IndexedIconColumn(iconIndex=2, widthIndex=4, heightIndex=5),
            Columns.TargetTitleColumn(pathIndex=0, titleIndex=1),
            Columns.TargetLastEventColumn(self.columnTitle, 3),
            ]


class OverviewPage(Template.Page):
    """A web page showing an overview of open source activity, meant to
       be used as a front page for the CIA web site. We want to act as
       a jumping-off point for the rest of the site, so this page doesn't
       include its own sidebar- it will copy the sidebar from a given page.
       """
    mainTitle = "CIA - The open source informant"

    heading = Template.pageBody[
        "This is a brief overview of the information collected recently. ",
        tag("a", href="/doc")[ "Learn more about CIA" ],
        ]

    taglines = [
        "Where were YOU at 04:00?",
        "The last best hope",
        "Resistance is useless",
        "More fun than a bucket of anchovies",
        "We know where you're going today",
        ]

    def __init__(self, leftColumnFrom, statsComponent):
        self.leftColumnFrom = leftColumnFrom
        self.statsComponent = statsComponent

    def render_subTitle(self, context):
        return random.choice(self.taglines)

    def render_leftColumn(self, context):
        return self.leftColumnFrom.leftColumn

    def render_mainColumn(self, context):
        return [
            self.heading,
            Nouvelle.subcontext(component=self.statsComponent)[
                Template.SectionGrid(
                    [
                        ActivitySection("project", "Most active projects today"),
                        ActivitySection("author", "Most active authors today"),
                    ],
                    [
                        TimestampSection("project", "Newest projects"),
                        TimestampSection("author", "Newest authors"),
                    ],
                ),
            ]
        ]

### The End ###
