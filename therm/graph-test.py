#!/usr/bin/env python

import MySQLdb
import MySQLdb.cursors
import Gnuplot
import os, time

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

    def getLatestPacket(self):
       for row in therm.iterDictQuery("SELECT P.id, P.time, T.average, V.voltage FROM packets P "
                                       "LEFT OUTER JOIN temperatures T ON (T.packet = P.id) "
                                       "LEFT OUTER JOIN battery_voltage V ON (V.packet = P.id) "
                                       "WHERE source = %d ORDER BY P.time DESC LIMIT 1" % source.id):
           return row


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


def showLatest(therm, source):
    latest = source.getLatestPacket()
    tokens = ["%-45r" % source,
              "[%s]" % latest['time'],
              "id=%d" % latest['id']]
    if latest['average'] is not None:
        tokens.append(("%.02fC" % latest['average']))
    if latest['voltage'] is not None:
        tokens.append(("%.04fV" % latest['voltage']))
    print " ".join(tokens)


def createGraphs(therm, source):
    g = Gnuplot.Gnuplot()
    thermData = []
    batteryData = []

    for row in therm.iterDictQuery("SELECT P.time, T.average, V.voltage FROM packets P "
                                   "LEFT OUTER JOIN temperatures T ON (T.packet = P.id) "
                                   "LEFT OUTER JOIN battery_voltage V ON (V.packet = P.id) "
                                   "WHERE source = %d" % source.id):
        t = row['time'].ticks()
        temperature = row['average']
        voltage = row['voltage']

        if temperature is not None:
            thermData.append((t, temperature))
        if voltage is not None:
            batteryData.append((t, voltage))

    if len(thermData) > 1:
        g('set terminal png size 3000,240')
        g('set output "%s.therm.png"' % source.name)
        g('set pointsize 1')
        g.xlabel('Time')
        g.ylabel('Degrees C')
        g.plot(thermData)
        time.sleep(0.2)

    if len(batteryData) > 1:
        g('set terminal png size 800,240')
        g('set output "%s.battery.png"' % source.name)
        g('set pointsize 1')
        g.xlabel('Time')
        g.ylabel('Volts')
        g.plot(batteryData)
        time.sleep(0.2)

if __name__ == "__main__":
    therm = ThermDatabase()
    for source in therm.iterSources():
        showLatest(therm, source)
        createGraphs(therm, source)
