""" pytherm.web

This is a mod_python frontend for accessing collected therm data.
It generates a user-friendly HTML interface, plus it allows automated
remote access over HTTP.

This module is a request handler for mod_python. Add something
like the following to your apache config:

<Location /therm>
    SetHandler python-program
    PythonPath "['/path/to/navi-misc/therm'] + sys.path"
    PythonHandler pytherm.web
</Location>

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

from pytherm import database, units, rrd
import time, shutil, os
import Nouvelle
from Nouvelle import tag, place, xml, ModPython
from mod_python import apache

# FIXME: This is a hack to keep us from processing too many records at once.
globalUpdateLimit = 500

class template:
    """This is a namespace used for common web page components"""

    doctype = xml('<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.1//EN"'
                  ' "http://www.w3.org/TR/xhtml11/DTD/xhtml11.dtd">\n')
    htmlTag = tag('html', xmlns="http://www.w3.org/1999/xhtml")
    refresh = xml('<meta http-equiv="refresh" content="60" />')

    footer = tag('div', _class='footer')[
        "Page generated on ", lambda context: time.ctime()
        ]

    stylesheet = tag('style', type='text/css')[
        open(os.path.join(os.path.split(__file__)[0], 'style.css')).read()]


def renderTemperature(latest, _class='temperatures'):
    degC = latest.get('average')
    if degC is None:
        return "No data"
    degF = units.degCtoF(degC)
    return tag('div', _class=_class)[
        tag('span', _class='mainTemperature')[ "%.01f" % degF, xml("&deg;"), "F" ],
        tag('span', _class='temperatureSeparator')[ " / " ],
        tag('span', _class='altTemperature')[ "%.01f" % degC, xml("&deg;"), "C" ],
        ]

def renderBargraph(alpha, numBars=16):
    alpha = max(0, min(1, alpha))
    filledBars = int(alpha * numBars + 0.5)
    bars = []
    for i in xrange(numBars):
        if i < filledBars:
            bars.append(tag('span', _class="filledBar")[ " " ])
        else:
            bars.append(tag('span', _class="emptyBar")[ " " ])
    return tag('span', _class="bargraph")[ bars ]

def renderBatteryBargraph(latest):
    voltage = latest.get('voltage')
    if voltage is None:
        return []
    minVoltage = 6.5
    maxVoltage = 9.0
    qualitative = (voltage - minVoltage) / (maxVoltage - minVoltage)
    return [
        renderBargraph(qualitative), "%.03f V" % voltage,
        ]

def renderTimestamp(latest):
    if 'time' not in latest:
        return "No packets received"
    delta = time.time() - units.datetimeTicks(latest['time'])
    return [
        units.time.format(delta), " ago",
        ]

def getSourceRrd(source, key, filter=None):
    """Returns an RrdFile for the specified key in packets from the specified source"""
    def dataGenerator(stamp):
        for packet in source.iterPacketsAfter(stamp, limit=globalUpdateLimit):
            value = packet.get(key)
            if value is not None:
                if filter:
                    value = filter(value)
                yield packet['id'], units.datetimeTicks(packet['time']), value

    rrdf = rrd.RrdFile("%s-%s" % (source.name, key))
    rrdf.update(dataGenerator)
    return rrdf

def getPacketLossRrd(source, sequenceMask=31):
    """Returns an RrdFile that measures packet loss for the given source.
       This looks for gaps in the packet sequence numbers.
       """
    def dataGenerator(stamp):
        # Get an initial value for the previous sequence number
        try:
            prev = source.iterPacketsBeforeOrEqual(stamp, 1).next().get('sequence')
        except StopIteration:
            prev = 0

        for packet in source.iterPacketsAfter(stamp, limit=globalUpdateLimit):
            value = packet.get('sequence')
            if value is not None and prev is not None:
                delta = (value - prev) & sequenceMask

                # If there is no packet loss, the delta will be 1. Packet loss
                # gives us a higher delta. If the delta is zero, it means either we
                # received different versions of the same packet or there was enough
                # packet loss that the counter rolled over exactly once. Ignore these
                # situations for now.
                loss = max(0, delta - 1)

                yield packet['id'], units.datetimeTicks(packet['time']), loss
            prev = value

    rrdf = rrd.RrdFile("%s-sequence-deltas" % source.name)
    rrdf.update(dataGenerator)
    return rrdf

def colorGenerator():
    """A generator that indefinitely yields color values to use when
       autocoloring channels.
       """
    import colorsys
    while True:
        for luma in (0.8, 0.5):
            for hue in (0.66, 0, 0.33, 0.75, 0.15):
                yield rrd.Color(hsv=(hue,1,luma))


class IndexPage(ModPython.Page):
    """The root page of the web site- gives a summary of each individual sensor,
       graph thumbnails, and links to the other pages.
       """
    isLeaf = False
    document = [
        template.doctype,
        template.htmlTag[
            tag('head')[
                tag('title')[ "Navi - Therm Server" ],
                template.refresh,
                template.stylesheet,
            ],
            tag('body')[
                place('sources'),
                template.footer,
            ],
        ],
    ]

    def __init__(self):
        self.db = database.open()
        self.children = {
            'source': SourceLookupPage(self.db),
            }

    def render_date(self, context):
        return time.ctime()

    def render_sources(self, context):
        return tag('table')[
            tag('tr')[
                tag('th')[ "Name" ],
                tag('th')[ "Current Temperature" ],
                tag('th')[ "Last 24 Hours" ],
                tag('th')[ "Last Updated" ],
                tag('th')[ "Battery" ],
            ],
            tag('tr')[ tag('td')[" "] ],
            [self.renderSource(source, context) for source in self.db.iterSources()],
        ]

    def renderSource(self, source, context):
        latest = source.getLatestPacket() or {}
        return [
            tag('tr')[
                tag('td')[ tag('a', href="source/%s/" % source.name, _class='name')[ source ]],
                tag('td')[ renderTemperature(latest) ],
                tag('td')[
                    tag('img', _class='thumbnail',
                        width=107, height=32,
                        alt="24-hour thumbnail",
                        src="source/%s/graph/temperature:day/scale/x32" % source.name)
                ],
                tag('td')[ renderTimestamp(latest) ],
                tag('td')[ renderBatteryBargraph(latest) ],
            ],
        ]


class SourceLookupPage(ModPython.Page):
    """A page that directs all children to SourcePages. This page itself returns
       a plaintext list of source names.
       """
    def __init__(self, db):
        self.db = db

    def render(self, context):
        req = context['request']
        req.content_type = 'text/plain'
        for source in self.db.iterSources():
            req.write("%s\n" % source.name)
        return apache.OK

    def getChild(self, name):
        source = self.db.getSource(name)
        if source:
            return SourcePage(source)


class SourcePage(ModPython.Page):
    """A page that shows detailed information on one therm source, and holds
       pages for rendering graphs for that source.
       """
    isLeaf = False
    document = [
        template.doctype,
        template.htmlTag[
            tag('head')[
                tag('title')[ "Therm - ", place('source') ],
                template.refresh,
                template.stylesheet,
            ],
            tag('body')[
                tag('h1')[ place('source') ],
                place('description'),
                place('navigation'),
                place('intervals'),
                template.footer,
            ],
        ],
    ]

    sectionLinks = [[
        tag('a', href='#%s' % name)["[%s]" % name]
        for name in ('temperatures', 'voltages')
        ]]

    # Each interval is a (name, length, anchor) tuple
    intervals = [
        ('Now',         None,     'top'),
        ('Today',       'day',    'day'),
        ('This Week',   'week',   'week'),
        ('This Month',  'month',  'month'),
        ('This Year',   'year',   'year'),
        ]

    def __init__(self, source):
        self.source = source
        self.children = dict(
            graph=SourceGraphs(source),
            latest=SourceLatestPage(source))

    def showDetails(self, context):
        """Should we show extra details on battery level, signal strength, and such?"""
        return bool(int(context['args'].get('details', (0,))[0]))

    def render_navigation(self, context):
        if self.showDetails(context):
            detailSwitch = tag('a', _class='navigation', href="?details=0")["Hide details"]
        else:
            detailSwitch = tag('a', _class='navigation', href="?details=1")["Show details"]

        return tag('div', _class='navigation')[
            tag('a', _class='navigation', href='../../')[ "All Sensors..." ],
            [tag('a', _class='navigation', href="#%s" % anchor or 'top')[ name ]
             for name, length, anchor in self.intervals],
            detailSwitch,
            ]

    def render_intervals(self, context):
        return [[
            tag('a', _name=anchor, id=anchor)[
                tag('h2')[ name ],
            ],
            tag('div', _class='sensorInterval')[ ' ', self.renderInterval(length, context) ],
        ] for name, length, anchor in self.intervals]

    def render_description(self, context):
        details = []

        if self.source.sensor_type:
            details.append("%s sensor" % self.source.sensor_type)
        if self.source.micro_type:
            details.append("%s microcontroller" % self.source.micro_type)

        return tag('div', _class='description')[
            self.source.description,
            tag('p')[ ", ".join(details) ],
            ]

    def renderInterval(self, length, context):
        if not length:
            return self.renderLatest()

        graphs = []
        if self.latest:
            if self.latest.get('average'):
                graphs.append('temperature')

            if self.showDetails(context):
                if self.latest.get('voltage'):
                    graphs.append('voltage')
                if self.latest.get('signal_strength'):
                    graphs.append('signal')
                    graphs.append('packetLoss')
                    graphs.append('packetCopies')

        return  [tag('img', _class='graph',
                     width=691, height=205,
                     alt="%s graph" % name,
                     src="graph/%s:%s" % (name, length))
                 for name in graphs]

    def renderLatest(self):
        self.latest = self.source.getLatestPacket()
        if self.latest:
            info = []

            if self.latest.get('average'):
                info.append(renderTemperature(self.latest))

            if self.latest.get('voltage'):
                info.append(tag('p')[ "Battery: ", renderBatteryBargraph(self.latest) ])

            if self.latest.get('time'):
                info.append(tag('p')[ "Last reading received ",
                                      tag('strong')[ renderTimestamp(self.latest) ]])

        else:
            info = ["No data"]

        return tag('div', _class='currentData')[info]


class SourceGraphs(ModPython.Page):
    """A page that holds the many graphs available for a therm source. Children
       are a graph style and interval separated by a colon.
       """
    def __init__(self, source):
        self.source = source

    def render(self, context):
        return apache.HTTP_NOT_FOUND

    def getChild(self, name):
        style, interval = name.split(":")
        f = getattr(self, "graph_%s" % style, None)
        if not f:
            return apache.HTTP_NOT_FOUND
        return ImageResourcePage(f(interval))

    def graph_temperature(self, interval):
        """Return an RrdGraph showing temperature data"""
        rrdf = getSourceRrd(self.source, 'average')
        graph = rrd.RrdGraph(rrd.graphDefineSource(rrdf) +
                             rrd.graphUnknownData() +
                             rrd.graphColorRange(-20.0, rrd.Color(0.5, 0.5, 1),
                                                 40.0, rrd.Color(1, 0.5, 0),
                                                 id=[0]) +
                             rrd.graphSpan("%s - Temperature" % self.source) +
                             rrd.graphHorizontalRule(0, "Freezing Point"),
                             interval=interval,
                             yLabel="Degrees Celsius")
        graph.updateToLatest()
        return graph

    def graph_voltage(self, interval):
        """Return an RrdGraph showing battery voltage"""
        rrdf = getSourceRrd(self.source, 'voltage')
        graph = rrd.RrdGraph(rrd.graphDefineSource(rrdf) +
                             rrd.graphUnknownData() +
                             rrd.graphColorRange(6, rrd.Color(1, 1, 1),
                                                 9.5, rrd.Color(0, 0, 1),
                                                 id=[0]) +
                             rrd.graphSpan("%s - Battery Voltage" % self.source),
                             interval=interval,
                             yLabel="Volts")
        graph.updateToLatest()
        return graph

    def graph_signal(self, interval):
        """Return an RrdGraph showing signal strength"""
        # Convert to percent
        rrdf = getSourceRrd(self.source, 'signal_strength', lambda x: x*100)
        graph = rrd.RrdGraph(rrd.graphDefineSource(rrdf) +
                             rrd.graphUnknownData() +
                             rrd.graphSpan("%s - Signal Strength" % self.source) +
                             rrd.graphHorizontalRule(100, "Full Strength"),
                             interval=interval,
                             yLabel="Percent Signal")
        graph.updateToLatest()
        return graph

    def graph_packetLoss(self, interval):
        """Return an RrdGraph showing the number of packets that were dropped
           completely. This doesn't count dropped duplicates in which one copy
           could still be received successfully, so this only reports actual
           loss of sensor readings.
           """
        rrdf = getPacketLossRrd(self.source)
        graph = rrd.RrdGraph(rrd.graphDefineSource(rrdf) +
                             rrd.graphUnknownData() +
                             rrd.graphSpan("%s - Number of packets completely lost" % self.source),
                             interval=interval,
                             yLabel="Dropped Packets")
        graph.updateToLatest()
        return graph

    def graph_packetCopies(self, interval):
        """Return an RrdGraph showing the number of packets that were dropped
           completely. This doesn't count dropped duplicates in which one copy
           could still be received successfully, so this only reports actual
           loss of sensor readings.
           """
        rrdf = getSourceRrd(self.source, 'num_copies')
        graph = rrd.RrdGraph(rrd.graphDefineSource(rrdf) +
                             rrd.graphUnknownData() +
                             rrd.graphSpan("%s - Number of duplicate packets" % self.source),
                             interval=interval,
                             yLabel="Duplicate Packets")
        graph.updateToLatest()
        return graph


class SourceLatestPage(ModPython.Page):
    """A page that returns the latest info for a particular source. Children return a single key"""
    contentType = "text/plain"
    def __init__(self, source, key=None):
        self.source = source
        self.key = key

    def getChild(self, name):
        if not self.key:
            return SourceLatestPage(self.source, name)

    def render(self, context):
        req = context['request']
        latest = self.source.getLatestPacket()
        if self.key and self.key not in latest:
            return apache.HTTP_NOT_FOUND
        req.content_type = self.contentType

        if self.key:
            req.write("%s\n" % latest[self.key])
        else:
            for key, value in latest.iteritems():
                req.write("%s: %s\n" % (key, value))
        return apache.OK


class ImageResourcePage(ModPython.Page):
    """Returns an image from the RRD cache. Includes a child resource
       for generating thumbnails.
       """
    contentType = "image/png"
    def __init__(self, resource):
        self.resource = resource
        self.children = dict(scale=ImageScalerPage(self.resource))

    def render(self, context):
        req = context['request']
        stamp = self.resource.getLatestStamp()
        filename = self.resource.getFile(stamp)
        req.content_type = self.contentType
        shutil.copyfileobj(open(filename), req)
        return apache.OK


class ImageScalerPage(ModPython.Page):
    """A page that scales another image. Children are sizes of the form [width]x[height]"""
    def __init__(self, original):
        self.original = original

    def getChild(self, name):
        width, height = name.split('x')
        if width:
            width = int(width)
        else:
            width = None
        if height:
            height = int(height)
        else:
            height = None
        scaled = rrd.ScaledImageResource(self.original, width, height)
        scaled.updateToLatest()
        return ImageResourcePage(scaled)


handler = IndexPage().handler

### The End ###
