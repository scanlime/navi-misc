""" LibCIA.Database

Utilities for accessing CIA's persistent data stored in an SQL database
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

from twisted.enterprise.adbapi import ConnectionPool
import MySQLdb.cursors
import _mysql
import os
import XML


# Import quoting functions for use elsewhere.
# We use twisted's quote utility most places,
# but as it doesn't understand MySQL-style binary
# objects, we use _mysql to define quoteBlob.
from twisted.enterprise.util import quote
from _mysql import escape_string as quoteBlob

# Disable the silly BLOB-to-array() conversion
# in the latest versions of python-mysqldb
def removeBlobConversions():
    from MySQLdb.converters import conversions
    from MySQLdb.constants import FIELD_TYPE
    del conversions[FIELD_TYPE.BLOB]
try:
    removeBlobConversions()
except:
    pass


def readDictFile(path):
    """Read a file formatted as a list of keys and values
       separated by '=' signs. Any amount of whitespace
       is allowed around the '=' and at the beginning and
       end of lines.
       """
    d = {}
    for line in open(path).readlines():
        line = line.strip()
        try:
            key, value = line.split('=', 1)
            d[key.strip()] = value.strip()
        except ValueError:
            pass
    return d


def createPool(overrides={}, filename="~/.cia_db", serverCursor=False):
    """
    This creates the global ConnectionPool object that we use to access our database.
    Note that a ConnectionPool doesn't actually connect to the database, it
    just imports the database module, validates it, and provides a way to
    run queries. This is initialized at the module level, so we can use rebuild
    to modify the database information if necessary.

    The database password is retrieved from ~/.mysql_passwd so it isn't stored
    in this file. If the file can't be read, an exception is raised.
    """

    # Defaults
    info = {
        'host': 'localhost',
        'db':   'cia',
        'user': 'root',

        # This is so we don't splurt our password out to twistd.log...
        'cp_noisy':  False,
        }

    # With server-side cursors we can iterate over the result set without
    # copying it all from mysqld to twistd. Unfortunately this can't
    # be the default yet- server side cursors require the execute/fetch
    # cycle to be obeyed strictly, and not all of CIA does this yet.
    if serverCursor:
        info['cursorclass'] = MySQLdb.cursors.SSCursor

    # Load user settings from disk
    if filename:
        filename = os.path.expanduser(filename)
        try:
            info.update(readDictFile(filename))
            os.chmod(filename, 0600)
        except IOError:
            raise Exception("Please create a file %r containing a list of database parameters.\n"
                            "For example:\n"
                            "  host = mysql.example.com\n"
                            "  user = bob\n"
                            "  passwd = widgets\n"
                            % filename)

    # Allow the caller to override settings, and remove Nones
    info.update(overrides)
    for key in info.keys():
        if info[key] is None:
            del info[key]

    return ConnectionPool('MySQLdb', **info)


pool = None

def init(*args, **kwargs):
    global pool
    pool = createPool(*args, **kwargs)


class Filter(XML.XMLObjectParser):
    """A Database.Filter is syntactically very similar to a Message.Filter,
       but describes an SQL expression. This class as-is is a generic SQL expression
       builder, which is probably too lenient for most apps. Subclasses can
       define their own variable lookup methods, to restrict the SQL expressions
       this can generate.

       After parsing, the completed SQL expression is available in the 'sql' attribute.

       >>> Filter('<and> \
                       <or> \
                           <match var="parent_path">project</match> \
                           <match var="parent_path">author</match> \
                       </or> \
                       <not> \
                           <match var="target_path">project/muffin-deluxe</match> \
                       </not> \
                   </and>').sql
       u"(((parent_path = 'project') OR (parent_path = 'author')) AND (!((target_path = 'project/muffin-deluxe'))))"

       """
    resultAttribute = 'sql'

    def varLookup(self, var):
        """Given a variable attribute, return an SQL expression representing it.
           The default assumes it's already valid SQL, but subclasses may implement
           this differently.
           """
        return var

    def element_match(self, element):
        """Compare a given variable exactly to the element's content, not including
           leading and trailing whitespace.
           """
        return "(%s = %s)" % (self.varLookup(element.getAttributeNS(None, 'var')),
                              quote(XML.shallowText(element).strip(), 'varchar'))

    def element_like(self, element):
        """Compare a given variable to the element's content using SQL's 'LIKE' operator,
           not including leading and trailing whitespace. This is case-insensitive, and includes
           the '%' wildcard which may be placed at the beginning or end of the string.
           """
        return "(%s LIKE %s)" % (self.varLookup(element.getAttributeNS(None, 'var')),
                                 quote(XML.shallowText(element).strip(), 'varchar'))

    def element_and(self, element):
        """Evaluates to True if and only if all child expressions evaluate to True"""
        return "(%s)" % (" AND ".join([self.parse(node) for node in XML.getChildElements(element)]))

    def element_or(self, element):
        """Evaluates to True if and only if any child function evaluates to True"""
        return "(%s)" % (" OR ".join([self.parse(node) for node in XML.getChildElements(element)]))

    def element_not(self, element):
        """The NOR function, returns false if and only if any child expression evaluates to True.
           For the reasoning behind calling this 'not', see the doc string for this class.
           """
        return "(!%s)" % self.element_or(element)

    def element_true(self, element):
        """Always evaluates to True"""
        return "(1)"

    def element_false(self, element):
        """Always evaluates to False"""
        return "(0)"

    def __and__(self, other):
        """Perform a logical 'and' on two Filters without evaluating them"""
        newFilter = Filter()
        newFilter.sql = "(%s AND %s)" % (self.sql, other.sql)
        return newFilter

    def __or__(self, other):
        """Perform a logical 'or' on two Filters without evaluating them"""
        newFilter = Filter()
        newFilter.sql = "(%s OR %s)" % (self.sql, other.sql)
        return newFilter

    def __invert__(self):
        """Perform a logical 'not' on this Filter without evaluating it"""
        newFilter = Filter()
        newFilter.sql = "(!%s)" % self.sql
        return newFilter

### The End ###
