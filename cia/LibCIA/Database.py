""" LibCIA.Database

Utilities for accessing CIA's persistent data stored in an SQL database
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

from twisted.enterprise.adbapi import ConnectionPool
import _mysql
import os


# Import quoting functions for use elsewhere.
# We use twisted's quote utility most places,
# but as it doesn't understand MySQL-style binary
# objects, we use _mysql to define quoteBlob.
from twisted.enterprise.util import quote
from _mysql import escape_string as quoteBlob


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


def createPool():
    #
    # This creates the global ConnectionPool object that we use to access our database.
    # Note that a ConnectionPool doesn't actually connect to the database, it
    # just imports the database module, validates it, and provides a way to
    # run queries. This is initialized at the module level, so we can use rebuild
    # to modify the database information if necessary.
    #
    # The database password is retrieved from ~/.mysql_passwd so it isn't stored
    # in this file. If the file can't be read, an exception is raised.

    # Defaults
    info = {
        'host': 'localhost',
        'db':   'cia',
        'user': 'root',

        # This is so we don't splurt our password out to twistd.log...
        'cp_noisy':  False,
        }

    filename = os.path.expanduser("~/.cia_db")
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

    return ConnectionPool('MySQLdb', **info)

pool = createPool()

### The End ###
