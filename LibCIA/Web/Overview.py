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
        T.target_path,
        M_TITLE.value,
        IF(M_ICON.name IS NOT NULL, M_ICON.name, M_PHOTO.name),
        STAT.%(counter_attrib)s
    FROM stats_catalog T
        LEFT OUTER JOIN stats_metadata M_TITLE     ON (T.target_path = M_TITLE.target_path     AND M_TITLE.name     = 'title')
        LEFT OUTER JOIN stats_metadata M_PHOTO     ON (T.target_path = M_PHOTO.target_path     AND M_PHOTO.name     = 'photo')
        LEFT OUTER JOIN stats_metadata M_ICON      ON (T.target_path = M_ICON.target_path      AND M_ICON.name      = 'icon')
        LEFT OUTER JOIN stats_counters STAT        ON (T.target_path = STAT.target_path        AND STAT.name        = %(counter)s)
        WHERE T.parent_path = %(path)s
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
            Columns.IndexedIconColumn(iconIndex=2, pathIndex=0),
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
            Columns.IndexedIconColumn(iconIndex=2, pathIndex=0),
            Columns.TargetTitleColumn(pathIndex=0, titleIndex=1),
            Columns.TargetLastEventColumn(self.columnTitle, 3),
            ]


class OverviewPage(Template.Page):
    """A web page showing an overview of open source activity, meant to
       be used as a front page for the CIA web site. We want to act as
       a jumping-off point for the rest of the site, so this page doesn't
       include its own sidebar- it will copy the sidebar from a given page.
       """
    mainTitle = "CIA Open Source Notification System"
    subTitle = "A real-time window into the open source world"

    heading = Template.pageBody[
        "This is a brief overview of the information collected recently. ",
        tag("a", href="/doc")[ "Learn more about CIA" ],
        ]

    def __init__(self, leftColumnFrom, statsComponent):
        self.leftColumnFrom = leftColumnFrom
        self.statsComponent = statsComponent

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
                        TimestampSection("project", "Least active projects",
                                         counterAttrib = 'last_time',
                                         sort = '',
                                         columnTitle = 'latest event'),
                    ],
                ),
            ]
        ]

### The End ###
