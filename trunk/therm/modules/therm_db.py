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
           "SELECT P.id, P.time, T.average, V.voltage FROM packets P "
           "LEFT OUTER JOIN temperatures T ON (T.packet = P.id) "
           "LEFT OUTER JOIN battery_voltage V ON (V.packet = P.id) "
           "WHERE source = %d ORDER BY P.time DESC LIMIT 1" % self.id):
           return row


class ThermDatabase:
    def __init__(self, **params):
        self.db = MySQLdb.connect(**params)

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

### The End ###
