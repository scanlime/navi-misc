""" LibCIA.Web.DataDriven

A simple system built on top of Nouvelle's Table for displaying
information gathered from an SQL database query.
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
from LibCIA import Database
import Template
from Nouvelle import tag, place, xml, subcontext


class DataTable(Template.Table):
    """A table that sources its data from an SQL query's results, using the SQL
       server to sort the query rather than sorting the rows manually.
       'ORDER BY' and 'LIMIT' clauses will automatically be appended to the query.
       """
    def __init__(self, query, columns, limit=None, offset=None, id=None):
        self.query = query
        self.limit = limit
        self.offset = offset
        Template.Table.__init__(self, [], columns, id=id)

    def render(self, context={}):
        """Override render() to execute an SQL query with the proper sorting information
           rather than trying to use sortByColumn. We have to do this rather than just
           providing a new sortByColumn because ResortableTable doesn't know about Deferreds.
           """
        cookie = self.getCookieFromContext(context)
        if cookie:
            self.setSortFromCookie(cookie)
        else:
            self.sortColumnIndex = self.defaultSortColumnIndex
            self.sortReversed = self.defaultSortReversed

        query = "%s ORDER BY %s" % (self.query, self.sortColumnIndex + 1)
        if self.sortReversed:
            query += ' DESC'
        if self.limit:
            query += ' LIMIT ' + str(self.limit)
        if self.offset:
            query += ' OFFSET ' + str(self.offset)

        result = defer.Deferred()
        Database.pool.runQuery(query).addCallback(
            self._render, context, result).addErrback(result.errback)
        return result

    def _render(self, rows, context, result):
        """Actually render the table, after we get the database query back"""
        self.rows = list(rows)
        # If we had no results, render a None so we can detect this in the Section
        if self.rows:
            result.callback(Template.Table.render(self, context))
        else:
            result.callback(None)
        self.rows = []


class DataTableSection(Template.Section):
    """Base classes for sections displaying data from an SQL query.
       This provides a convenient way to construct the proper DataTable,
       and provides widgets for paging through and searching long results.
       """
    # Placeholders for things subclasses might want to override...
    tableFactory = DataTable
    title = None
    query = None
    tableId = None
    columns = []

    def getQueryArgs(self):
        """Return a dictionary of named format specifiers to replace
           in the query, %(foo)s and such.
           """
        return {}

    def render_rows(self, context):
        result = defer.Deferred()
        self.tableFactory(self.query % self.getQueryArgs(),
                          self.columns,
                          id = self.tableId,
                          limit = 20,
                          ).render(context).addCallback(
            self._render_rows, context, result
            ).addErrback(result.errback)
        return result

    def _render_rows(self, table, context, result):
        """This is called after the Deferred we get back from the
           DataTable completes. We either have a rendered table,
           or None if the query had no results.
           """
        if table:
            result.callback([table])
        else:
            result.callback(self.renderEmpty())

    def renderEmpty(self):
        """This returns the rows list used to render empty result sets.
           By default it returns [], hiding the section entirely.
           This could be replaced with a message indicating there are no results.
           """
        return []

### The End ###
