""" therm_db

This module acts as a simple abstraction for reading from the therm
database.

"""
#
# Wireless therm system
# Copyright (C) 2004 Micah Dowty
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License
# as published by the Free Software Foundation; either version 2
# of the License, or (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
#

import MySQLdb
import MySQLdb.cursors
import os


def prettifyName(str):
    """Replace underscores with spaces, and give it title capitalization"""
    words = str.replace("_", " ").split()
    return " ".join([ word.capitalize() for word in words ])


class ThermSource:
    """One data source in the therm database"""
    def __init__(self, db, id, medium=None, protocol=None, station_id=None,
                 name=None, description=None):
        self.db = db
        self.id = id
        self.medium = medium
        self.protocol = protocol
        self.station_id = station_id
        self.name = name
        self.description = description

    def __repr__(self):
        return "<ThermSource %s at %s:%d:%d>" % (
            self.name, self.medium, self.protocol, self.station_id)

    def getLatestPacket(self):
       for row in self.db.iterDictQuery(
           "SELECT * FROM packets P "
           "LEFT OUTER JOIN temperatures T ON (T.packet = P.id) "
           "LEFT OUTER JOIN battery_voltage V ON (V.packet = P.id) "
           "WHERE source = %d ORDER BY P.time DESC LIMIT 1" % self.id):
           return row

    def iterPacketsAfter(self, id):
        """Iterate over packets after the given ID"""
        return self.db.iterDictQuery(
            "SELECT * FROM packets P "
            "LEFT OUTER JOIN temperatures T ON (T.packet = P.id) "
            "LEFT OUTER JOIN battery_voltage V ON (V.packet = P.id) "
            "WHERE source = %d AND P.id > %d" % (self.id, id))


class ThermDatabase:
    def __init__(self, **params):
        self.db = MySQLdb.connect(**params)

        # Turn on autocommit. For DB API compliance this is off
        # initially, but it will cause our connection to miss new
        # packets arriving.
        self.db.cursor().execute("SET AUTOCOMMIT=1")

    def iterDictQuery(self, query):
        """Perform an SQL query and iterate over the result rows as dictionaries"""
        cursor = MySQLdb.cursors.DictCursor(self.db)
        cursor.execute(query)
        for row in cursor:
            yield row

    def iterSources(self):
        """Iterate over all source objects in the database"""
        for row in self.iterDictQuery("SELECT * FROM sources"):
            yield ThermSource(self, **row)

    def getSource(self, name):
        """Look up a source by name"""
        for row in self.iterDictQuery("SELECT * FROM sources WHERE name = '%s'" % (
            self.db.escape_string(name))):
            return ThermSource(self, **row)
        raise KeyError("No such therm source %r" % name)


def getDatabaseHost():
    """Normally the database is located on navi, but if this
       script is running on navi we shoud specify localhost
       for the security db to find us.
       """
    if os.uname()[1] == "navi":
        return "localhost"
    else:
        return "navi"

# This is a very low-privilege account that can only read
# from the database, probably no reason not to have
# all the login info here.
defaultDatabase = ThermDatabase(
    db="therm", host=getDatabaseHost(),
    user="therm_reader", passwd="e5ce14d3")

### The End ###
