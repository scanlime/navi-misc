""" pytherm.database

This module acts as a simple abstraction for reading from the therm
database.

"""
#
# Wireless therm system
# Copyright (C) 2004-2005 Micah Dowty
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
import os, time


class ThermSource:
    """One data source in the therm database"""
    packetQuery = """
        SELECT P.id, P.time, P.num_copies, P.sequence, P.signal_strength, T.average, V.voltage
            FROM packets P FORCE INDEX (id)
            LEFT OUTER JOIN temperatures T ON (T.packet = P.id)
            LEFT OUTER JOIN battery_voltage V ON (V.packet = P.id)
        """

    def __init__(self, db, id, **properties):
        self.db = db
        self.id = id
        self.name = None
        self.description = None
        self.__dict__.update(properties)

    def __repr__(self):
        return "<ThermSource %s at %s:%d:%d>" % (
            self.name, self.medium, self.protocol, self.station_id)

    def __str__(self):
        """Replace underscores with spaces, and give it title capitalization"""
        words = self.name.replace("_", " ").split()
        return " ".join([ word.capitalize() for word in words ])

    def getLatestPacket(self):
        for row in self.db.iterDictQuery(
            "%s WHERE P.source = %d ORDER BY P.id DESC LIMIT 1" % (
            self.packetQuery, self.id)):
            return row

    def iterPacketsAfter(self, id, limit=None):
        """Iterate over packets after the given ID"""
        if limit:
            limit = "LIMIT %d" % limit
        else:
            limit = ''
        return self.db.iterDictQuery(
            "%s WHERE P.source = %d AND P.id > %d ORDER BY P.id %s" % (
            self.packetQuery, self.id, id, limit))

    def iterPacketsBeforeOrEqual(self, id, limit=None):
        """The complement to iterPacketsAfter- iterates backwards over
           all packets with an ID less than or equal to the given one.
           """
        if limit:
            limit = "LIMIT %d" % limit
        else:
            limit = ''
        return self.db.iterDictQuery(
            "%s WHERE P.source = %d AND P.id <= %d ORDER BY P.id DESC %s" % (
            self.packetQuery, self.id, id, limit))

    def pollNewPackets(self, afterId=None, pollInterval=0.5):
        """Iterate over new packets as they arrive. If an ID is given,
           start looking for packets after that ID- if no ID is given,
           we use the ID of the latest packet currently in the database.
           """
        if afterId is None:
            afterId = self.getLatestPacket()['id']
        while 1:
            for packet in self.iterPacketsAfter(afterId):
                afterId = packet['id']
                yield packet
            time.sleep(pollInterval)


class ThermDatabase:
    sourceQuery = """
        SELECT id, medium, protocol, station_id, name, description,
            micro_type, sensor_type FROM sources
        """

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
        for row in self.iterDictQuery("%s ORDER BY name" % self.sourceQuery):
            yield ThermSource(self, **row)

    def getSource(self, name):
        """Look up a source by name"""
        for row in self.iterDictQuery("%s WHERE name = '%s'" % (
            self.sourceQuery, self.db.escape_string(name))):
            return ThermSource(self, **row)
        raise KeyError("No such therm source %r" % name)


def getDatabaseHost():
    """Normally the database is located on zero, but if this
       script is running on zero we shoud specify localhost
       for the security db to find us.
       """
    if os.uname()[1] == "zero":
        return "localhost"
    else:
        return "zero.navi.cx"

def open(cache=[None]):
    """Open a default database connection, with a very low-privilege
       account that can only read from the database.
       """
    if not cache[0]:
        cache[0] = ThermDatabase(
            db="therm", host=getDatabaseHost(),
            user="therm_reader", passwd="e5ce14d3")
    return cache[0]

### The End ###
