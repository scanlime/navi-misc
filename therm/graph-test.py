#!/usr/bin/env python

import MySQLdb
import MySQLdb.cursors
import os

class ThermSource:
    """One data source in the therm database"""
    def __init__(self, id=None, medium=None, protocol=None, station_id=None,
                 name=None, description=None):
        self.id = id
        self.medium = medium
        self.protocol = protocol
        self.station_id = station_id
        self.name = name
        self.description = description

    def __repr__(self):
        return "<ThermSource %s at %s:%d:%d>" % (
            self.name, self.medium, self.protocol, self.station_id)


class ThermDatabase:
    def __init__(self, **dbParams):
        params = dict(db="therm", user="therm_reader", passwd=None)
        params.update(dbParams)
        if not params.get("passwd"):
            params["passwd"] = open(os.path.expanduser("~/keys/therm_reader")).read().strip()
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
            yield ThermSource(**row)


if __name__ == "__main__":
    therm = ThermDatabase()
    for source in therm.iterSources():
        temperatures = open("%s.therm.log" % source.name, "w")
        voltages = open("%s.battery.log" % source.name, "w")
        for row in therm.iterDictQuery("SELECT P.time, T.average, V.voltage FROM packets P "
                                       "LEFT OUTER JOIN temperatures T ON (T.packet = P.id) "
                                       "LEFT OUTER JOIN battery_voltage V ON (V.packet = P.id) "
                                       "WHERE source = %d" % source.id):
            time = row['time'].ticks()
            temperature = row['average']
            voltage = row['voltage']

            if temperature is not None:
                temperatures.write("%s %s\n" % (time, temperature))
            if voltage is not None:
                voltages.write("%s %s\n" % (time, voltage))
