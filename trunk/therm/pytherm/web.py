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

from pytherm import database, units, rrd
import time, shutil
import Nouvelle
from Nouvelle import tag, place, xml, ModPython
from mod_python import apache


class template:
    """This is a namespace used for common web page components"""

    doctype = xml('<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.1//EN"'
                  ' "http://www.w3.org/TR/xhtml11/DTD/xhtml11.dtd">\n')
    htmlTag = tag('html', xmlns="http://www.w3.org/1999/xhtml")
    refresh = xml('<meta http-equiv="refresh" content="30" />')

    footer = tag('div', _class='footer')[
        "Page generated on ", lambda context: time.ctime()
        ]

    stylesheet = tag('style', type='text/css')["""
h1 {
    font: 190% sans-serif;
    font-weight: bold;
    color: #333;
    margin: 0em 0em 1px 0em;
    padding: 0.1em 0em 0.2em 0.1em;
}
h2 {
    font: 120% sans-serif;
    font-weight: bold;
    color: #333;
    margin: 0.25em;
    padding: 0em;
}
.footer {
    text-align: center;
    border-top: 1px solid #777;
    color: #777;
    font: 80% normal;
    margin: 1em 0em;
    padding: 1em;
}

div.navigation {
    border-top: 1px solid #777;
    border-bottom: 1px solid #777;
    margin: 1em 0em;
    padding: 1px;
    text-align: center;
}
a.navigation {
    margin: 0.1em 0em;
    padding: 0em 1em;
    text-decoration: none;
    color: blue;
}
a.navigation:hover {
    background: #DDF;
}

a.source {
    text-decoration: none;
    color: black;
}
a.source:hover {
}
div.source {
    background: #DDD;
    border: 1px solid #777;
    margin: 2em 2em;
    padding: 0.5em;
}

.name {
    font: 100% sans-serif;
    font-weight: bold;
    color: #333;
}
.description {
    font: 80% normal;
    color: #666;
    margin: 0.25em 1em;
}

.temperatures {
    float: right;
    margin: 0em 0.5em;
}
.mainTemperature {
    font: 180% sans-serif;
    vertical-align: middle;
}
.temperatureSeparator {
    font: 180% sans-serif;
    font-weight: bold;
    color: #666;
    vertical-align: middle;
}
.altTemperature {
    font: 120% sans-serif;
    color: #666;
    vertical-align: middle;
}

.thumbnails {
    margin: 0.8em 0em 0em 0.25em;
    padding: 0em;
}
img.thumbnail {
    border: 1px solid #777;
    margin: 0em 0.6em 0em 0em;
}
.graphs {
    text-align: center;
}
img.graph {
    margin: 0.4em;
    padding: 1px;
}

.extraInfo {
    margin: 0.75em 0em 0em 0em;
    font: 80% sans-serif;
}
span.bargraph {
    font: 5px sans-serif;
    margin: 0em 2em 0em 1em;
    background: #FFF;
    border: 1px solid #777;
    padding: 3px 1px 3px 0px;
    vertical-align: middle;
}
span.filledBar {
    background: #338;
    margin: 1px 0px 1px 1px;
    vertical-align: middle;
    padding: 2px;
}
span.emptyBar {
    margin: 1px 0px 1px 1px;
    vertical-align: middle;
    padding: 2px;
}
"""]


def renderTemperature(latest, _class='temperatures'):
    degC = latest['average']
    degF = units.degCtoF(degC)
    return tag('div', _class=_class)[
        tag('span', _class='mainTemperature')[ "%.01f" % degF, xml("&deg;"), "F" ],
        tag('span', _class='temperatureSeparator')[ " / " ],
        tag('span', _class='altTemperature')[ "%.01f" % degC, xml("&deg;"), "C" ],
        ]

def renderBargraph(alpha, numBars=30):
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
    voltage = latest['voltage']
    minVoltage = 6.5
    maxVoltage = 9.0
    qualitative = (voltage - minVoltage) / (maxVoltage - minVoltage)
    return [
        "Battery: ", renderBargraph(qualitative), "%.03f V" % voltage,
        ]

def renderTimestamp(latest):
    delta = time.time() - latest['time'].ticks()
    return [
        "Last reading received ",
        tag('strong')[ units.time.format(delta) ], " ago",
        ]


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
        self.children = dict(source=SourceLookupPage(self.db))

    def render_date(self, context):
        return time.ctime()

    def render_sources(self, context):
        results = []
        for source in self.db.iterSources():
            results.append(self.renderSource(source, context))
        return results

    def renderSource(self, source, context):
        latest = source.getLatestPacket()
        info = []

        if latest.get('average'):
            info.append(renderTemperature(latest))

        info.extend([
            tag('div', _class='name')[ source ],
            tag('div', _class='description')[ source.description ],
            tag('div', _class='thumbnails')[[
                tag('img', _class='thumbnail',
                    width=168, height=50,
                    alt="Temperature thumbnail (last %s)" % interval,
                    src="source/%s/graph/temperature:%s/scale/x50" %
                    (source.name, interval))
                for interval in ('day', 'week', 'month', 'year')
            ]],
        ])

        if latest.get('voltage'):
            info.append(tag('div', _class='extraInfo')[
                renderBatteryBargraph(latest),
                ])

        if latest.get('time'):
            info.append(tag('div', _class='extraInfo')[
                renderTimestamp(latest),
                ])

        return tag('a', _class='source', href="source/%s/" % source.name)[
            tag('div', _class='source')[ info ]]


class SourceLookupPage(ModPython.Page):
    """A page that directs all children to SourcePages"""
    def __init__(self, db):
        self.db = db

    def render(self, context):
        return apache.HTTP_NOT_FOUND

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
        self.children = dict(graph=SourceGraphs(source))
        self.latest = source.getLatestPacket()

    def render_navigation(self, context):
        return tag('div', _class='navigation')[
            tag('a', _class='navigation', href='../../')[ "All Sensors..." ],
            [tag('a', _class='navigation', href="#%s" % anchor or 'top')[ name ]
             for name, length, anchor in self.intervals],
            ]

    def render_intervals(self, context):
        return [[
            tag('a', _name=anchor, id=anchor),
            tag('h2')[ name ],
            self.renderInterval(length),
        ] for name, length, anchor in self.intervals]

    def render_description(self, context):
        return tag('div', _class='description')[ self.source.description ]

    def renderInterval(self, length):
        if not length:
            return self.renderLatest()

        graphs = []
        if self.latest.get('average'):
            graphs.append('temperature')
        if self.latest.get('voltage'):
            graphs.append('voltage')
        if self.latest.get('signal_strength'):
            graphs.append('signal')

        return tag('div', _class='graphs')[[
                tag('img', _class='graph',
                    width=691, height=205,
                    alt="%s graph" % name,
                    src="graph/%s:%s" % (name, length))
                for name in graphs
                ]]

    def renderLatest(self):
        info = []

        if self.latest.get('average'):
            info.append(renderTemperature(self.latest))

        if self.latest.get('voltage'):
            info.append(tag('div', _class='extraInfo')[
                renderBatteryBargraph(self.latest),
                ])

        if self.latest.get('time'):
            info.append(tag('div', _class='extraInfo')[
                renderTimestamp(self.latest),
                ])

        return tag('div', _class='source')[info]


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

    def getSourceRrd(self, key, filter=None):
        """Returns an RrdFile for the specified key in packets from the specified source"""
        def dataGenerator(stamp):
            for packet in self.source.iterPacketsAfter(stamp):
                value = packet.get(key)
                if value is not None:
                    if filter:
                        value = filter(value)
                    yield packet['id'], packet['time'].ticks(), value

        rrdf = rrd.RrdFile("%s-%s" % (self.source.name, key))
        rrdf.update(dataGenerator)
        return rrdf

    def graph_temperature(self, interval):
        """Return an RrdGraph showing temperature data"""
        rrdf = self.getSourceRrd('average')
        graph = rrd.RrdGraph(rrd.graphDefineSource(rrdf) +
                             rrd.graphUnknownData() +
                             rrd.graphColorRange(-20.0, rrd.Color(0.5, 0.5, 1),
                                                 40.0, rrd.Color(1, 1, 0),
                                                 id=[0]) +
                             rrd.graphSpan("%s - Temperature" % self.source) +
                             rrd.graphHorizontalRule(0, "Freezing Point"),
                             interval=interval,
                             yLabel="Degrees Celsius")
        graph.updateToLatest()
        return graph

    def graph_voltage(self, interval):
        """Return an RrdGraph showing battery voltage"""
        rrdf = self.getSourceRrd('voltage')
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
        rrdf = self.getSourceRrd('signal_strength', lambda x: x*100)
        graph = rrd.RrdGraph(rrd.graphDefineSource(rrdf) +
                             rrd.graphUnknownData() +
                             rrd.graphSpan("%s - Signal Strength" % self.source) +
                             rrd.graphHorizontalRule(100, "Full Strength"),
                             interval=interval,
                             yLabel="Percent Signal")
        graph.updateToLatest()
        return graph


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
