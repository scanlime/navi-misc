""" Nouvelle.Table

A fancy table renderable for Nouvelle. The dataset is represented by a
sequence containing arbitrary objects representing rows. Columns
are classes that know how to retrieve a particular piece of data
from the table and optionally format it.

SortedTable is a Table in which each column heading is a link that
toggles sorting by that column and the direction of the sort.
"""
#
# Nouvelle web framework
# Copyright (C) 2003 Micah Dowty <micahjd@users.sourceforge.net>
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

__all__ = ['Table']


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


class AttributeColumn(Column):
    """A Column that has a fixed heading and returns some attribute from each row,
       with no special formatting.
       """
    def __init__(self, heading, attribute):
        self.heading = heading
        self.attribute = attribute

    def getValue(self, row):
        return getattr(row, self.attribute)


class Table:
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
            body.append(self.rowTag[self.render_heading(context)])
        for row in self.rows:
            body.append(self.rowTag[self.render_row(context, row)])
        return self.tableTag[body]

    def render_heading(self, context):
        """Return a list of headingTag instances labelling each column"""
        return [self.headingTag[c.render_heading(context)] for c in self.columns]

    def render_row(self, context, row):
        """Return a list of dataTag instances for each column in the given row"""
        return [self.dataTag[c.render_data(context, row)] for c in self.columns]

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

### The End ###
