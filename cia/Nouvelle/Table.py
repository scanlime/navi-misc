""" Nouvelle.Table

A fancy table renderable for Nouvelle. The dataset is represented by a
sequence containing arbitrary objects representing rows. Columns
are classes that know how to retrieve a particular piece of data
from the table and optionally format it.

ResortableTable is a Table in which each column heading is a link that
toggles sorting by that column and the direction of the sort.
"""
#
# Nouvelle web framework
# Copyright (C) 2003-2004 Micah Dowty <micahjd@users.sourceforge.net>
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

from __future__ import generators
from Serial import tag
import re

__all__ = ['BaseTable', 'Column', 'AttributeColumn', 'IndexedColumn',
           'ResortableTable']


class Column:
    """A column provides a way to view and sort by some aspect of
       the data stored in a row. This is an abstract base class defining
       a Column's interface.
       """
    def render_heading(self, context):
        """Returns a serializable object representing this column in a table's heading"""
        return self.heading

    def render_data(self, context, row):
        """Returns a serializable object representing this column's data for a particular row.
           Normally this will be a visible representation of getValue's results.
           By default it's just getValue's result without any extra formatting.
           """
        return self.getValue(row)

    def getValue(self, row):
        """Return a single value representing this column's perspective of the row.
           This is used by reduceColumn, and for sorting.
           """
        pass

    def cmp(self, a, b):
        """This column's comparison function, used for sorting a table by this column.
           By default this does an ascending sort using getValue.
           """
        return cmp(self.getValue(a), self.getValue(b))

    def isVisible(self, context):
        """Subclasses can override this to hide the entire column in some circumstances"""
        return True


class AttributeColumn(Column):
    """A Column that has a fixed heading and returns some attribute from each row,
       with no special formatting.
       """
    def __init__(self, heading, attribute):
        self.heading = heading
        self.attribute = attribute

    def getValue(self, row):
        return getattr(row, self.attribute)


class IndexedColumn(Column):
    """A Column that has a fixed index it uses to retrieve an item from the row
       and return it with no special formatting.
       """
    def __init__(self, heading, index):
        self.heading = heading
        self.index = index

    def getValue(self, row):
        return row[self.index]


class BaseTable:
    """A renderable that creates an XHTML table viewing some dataset using
       a list of Column instances. The Columns are used to generate headings
       and data cells for each column of the table.

       Subclasses can override the tag factories here to change the generated
       XHTML, possibly by adding CSS 'class' attributes to the tags.
       """
    tableTag   = tag('table')
    rowTag     = tag('tr')
    headingTag = tag('th')
    dataTag    = tag('td')

    def __init__(self, rows, columns, showHeading=True):
        self.rows = rows
        self.columns = columns
        self.showHeading = showHeading
        self._reduceColumnCache = {}

    def render(self, context={}):
        """Render the entire table, returning an unserialized tag tree"""
        context = dict(context)
        context['table'] = self

        body = []
        if self.showHeading:
            body.append(self.rowTag[self.render_headings(context)])
        for row in self.rows:
            body.append(self.rowTag[self.render_row(context, row)])
        return self.tableTag[body]

    def render_headings(self, context):
        """Return a list of headingTag instances labelling each column"""
        return [self.headingTag[self.render_heading(context, c)] for c in self.getVisibleColumns(context)]

    def render_heading(self, context, column):
        """Return a serializable representation of one column heading"""
        return column.render_heading(context)

    def getVisibleColumns(self, context):
        """Return an iterator over all visible columns"""
        for column in self.columns:
            if column.isVisible(context):
                yield column

    def render_row(self, context, row):
        """Return a list of dataTag instances for each column in the given row"""
        return [self.dataTag[c.render_data(context, row)] for c in self.getVisibleColumns(context)]

    def getColumnValues(self, column):
        """Return an iterator that iterates over all values in the specified column"""
        for row in self.rows:
            yield column.getValue(row)

    def reduceColumn(self, column, operation):
        """Calls the provided 'operation' with a generator that returns the getValue()
           result for the given column on every row. The result is cached.
           This is good for summing columns in the table, for example, by
           passing the Python builtin 'sum' as the operation.
           """
        key = (column, operation)
        if not self._reduceColumnCache.has_key(key):
            self._reduceColumnCache[key] = operation(self.getColumnValues(column))
        return self._reduceColumnCache[key]

    def sortByColumn(self, column, reverse=False):
        if reverse:
            self.rows.sort(lambda a,b: -column.cmp(a,b))
        else:
            self.rows.sort(column.cmp)


class ResortableTable(BaseTable):
    """A Table with hyperlinks on each table column that set that column
       as the sort column, or if it's already the sort column reverse the
       order.

       By default, this generates link URLs and retrieves the user's sort
       setting by assuming context['args'] is a dictionary of arguments
       passed in the query section of our URL, mapping key names to lists
       of values.

       Subclasses can override getCookieFromContext and getCookieHyperlink
       to change this behaviour. The 'cookie' is an opaque piece of information
       used to keep the table's state across page views.
       """
    headingLinkTag = tag('a')

    def __init__(self, rows, columns,
                 defaultSortColumnIndex = 0,
                 defaultSortReversed    = False,
                 ):
        self.defaultSortColumnIndex = defaultSortColumnIndex
        self.defaultSortReversed = defaultSortReversed
        BaseTable.__init__(self, rows, columns)

    def render(self, context={}):
        cookie = self.getCookieFromContext(context)
        if cookie:
            self.setSortFromCookie(cookie)
        else:
            self.sortColumnIndex = self.defaultSortColumnIndex
            self.sortReversed = self.defaultSortReversed
        self.sortByColumn(self.columns[self.sortColumnIndex], self.sortReversed)
        return BaseTable.render(self, context)

    def render_heading(self, context, column):
        """Override render_heading to insert hyperlinks generated with createSortCookie"""
        url = self.getCookieHyperlink(self.getSortCookie(column))
        return self.headingLinkTag(href=url)[column.render_heading(context)]

    def setSortFromCookie(self, cookie):
        """Set our current sort using the given cookie.
           Our cookies are just the column index (in the original
           column list, not just visible columns) optionally
           followed by 'R' for reversed sorts.
           """
        match = re.match("(?P<column>[0-9]+)(?P<reversed>R)?", str(cookie))
        self.sortColumnIndex = int(match.group('column'))
        self.sortReversed = bool(match.group('reversed'))

    def getSortCookie(self, column):
        """Return the cookie that should be used after the user clicks the given column.
           If this is the current sort column already, we reverse the sorting direction.
           If not, we set the column.
           """
        index = self.columns.index(column)
        if index == self.sortColumnIndex and not self.sortReversed:
            return "%dR" % index
        else:
            return str(index)

    def getCookieFromContext(self, context):
        try:
            return context['args']['sort'][0]
        except:
            return None

    def getCookieHyperlink(self, cookie):
        return "?sort=%s" % cookie

### The End ###
