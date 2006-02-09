""" LibCIA.Web.Info

Just a cute little page with informational doodads on it.
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

from __future__ import division
import Template, Server
from LibCIA import TimeUtil, XML, Database, Units, Cache
from Nouvelle import place, tag
import LibCIA, Nouvelle
from twisted.internet import defer
import time, sys, os


class Component(Server.Component):
    """A server component showing the info page"""
    name = 'Server Info'

    def __init__(self):
        self.resource = Page()

    def __contains__(self, page):
        return isinstance(page, Page)


class Clock(Template.Section):
    title = 'UTC clock'

    def render_rows(self, context):
        return [TimeUtil.formatDate(time.time())]


class Version(Template.Section):
    title = 'version'

    def render_rows(self, context):
        rows = [Template.value[LibCIA.__version__]]
        svnRev = self.getSvnRevision()
        if svnRev:
            rows.append(["SVN revision ", Template.value[svnRev]])
        rows.append(["Database version ", self.getDbVersion()])
        return rows

    def getSvnRevision(self):
        """Return the current Subversion repository revision, or None
           if we're not in an svn working copy or it can't be parsed.
           """
        try:
            entries = XML.parseString(open(".svn/entries").read()).documentElement
            highestRev = 0
            for tag in XML.getChildElements(entries):
                if tag.nodeName == 'entry':
                    rev = tag.getAttributeNS(None, 'committed-rev')
                    if rev and rev > highestRev:
                        highestRev = rev
            return highestRev
        except:
            return None

    def getDbVersion(self):
        """Return our database's schema version via a Deferred"""
        return Database.pool.runInteraction(self._getDbVersion)

    def _getDbVersion(self, cursor):
        cursor.execute("SELECT value FROM meta WHERE name = 'version'")
        row = cursor.fetchone()
        if row:
            return Template.value[ row[0] ]
        else:
            return Template.error["unknown"]


class WebServer(Template.Section):
    title = 'web server'

    def render_requestCount(self, context):
        return context['request'].site.requestCount

    def render_uptime(self, context):
        return TimeUtil.formatDuration(time.time() - context['request'].site.serverStartTime)

    def render_mtbr(self, context):
        site = context['request'].site
        if site.requestCount > 0:
            return TimeUtil.formatDuration((time.time() - site.serverStartTime) / site.requestCount)
        else:
            return Template.error["unknown"]

    rows = [
               [
                   Template.value[ place('requestCount') ],
                   ' requests have been processed since the server was loaded',
               ],
               [
                   'The server has been up for ',
                   Template.value[ place('uptime') ],
               ],
               [
                   'On average, there is a request every ',
                   Template.value[ place('mtbr') ],
               ],
           ]


class IndexedStorageColumn(Nouvelle.IndexedColumn):
    """An IndexedColumn that renders its content as a
       size, in bytes- rescaled into an appropriate unit
       """
    def render_data(self, context, row):
        return Units.StorageUnits().format(self.getValue(row))


class HitRatioColumn(Nouvelle.Column):
    """Show the ratio of hits to total cache accesses, as a percentage"""
    heading = 'hit ratio'

    def getValue(self, perf):
        total = perf.hits + perf.misses
        if total > 0:
            return perf.hits / total * 100

    def render_data(self, context, perf):
        value = self.getValue(perf)
        if value is None:
            return Template.error["unknown"]
        else:
            return "%.02f %%" % value


class CachePerformance(Template.Section):
    title = 'cache performance'

    columns = [
        Nouvelle.AttributeColumn('name', 'name'),
        Nouvelle.AttributeColumn('hits', 'hits'),
        Nouvelle.AttributeColumn('misses', 'misses'),
        HitRatioColumn(),
        ]

    def render_rows(self, context):
        perfList = list(Cache.getCachePerformanceList())
        if perfList:
            return [Template.Table(perfList, self.columns, id='cachePerf')]
        else:
            return []


class DbTables(Template.Section):
    title = 'database tables'

    columns = [
        Nouvelle.IndexedColumn('name', 0),
        IndexedStorageColumn('data size', 5),
        IndexedStorageColumn('index size', 7),
        Nouvelle.IndexedColumn('type', 1),
        Nouvelle.IndexedColumn('approx. rows', 3),
        ]

    def render_rows(self, context):
        # Fetch the results of a 'show table status' before we can do anything
        result = defer.Deferred()
        Database.pool.runQuery('SHOW TABLE STATUS').addCallback(
            self._render_rows, result).addErrback(result.errback)
        return result

    def _render_rows(self, tableInfo, result):
        result.callback([
            Template.Table(list(tableInfo), self.columns, id='db'),
            ])


class System(Template.Section):
    title = 'system'

    def render_pyInfo(self, context):
        result = []
        for line in ('Python %s on %s' % (sys.version, sys.platform)).split("\n"):
            if result:
                result.append(tag('br'))
            result.append(line)
        return result

    def render_sysUptime(self, context):
        # This only works on linux systems for now
        try:
            seconds = float(open("/proc/uptime").read().split()[0])
        except:
            return "System uptime unknown"
        else:
            return ["This system has been up for ",
                    Template.value[ TimeUtil.formatDuration(seconds) ]]

    def render_load(self, context):
        # Also only works on linux now
        try:
            load = os.getloadavg()
        except OSError:
            return "Load average unknown"
        else:
            return ["Load average: ",
                    Template.value[ load[0] ],
                    " ", load[1], " ", load[2]]

    rows = [
               place('pyInfo'),
               place('sysUptime'),
               place('load'),
           ]


class Page(Template.Page):
    """A web page showing information about the server"""
    mainTitle = 'Server Info'
    subTitle = 'so that everyone needs to hang on tighter just to keep from being thrown to the wolves'

    leftColumn = [
        Version(),
        Clock(),
        ]

    mainColumn = [
        WebServer(),
        System(),
        CachePerformance(),
        DbTables(),
        ]

### The End ###
