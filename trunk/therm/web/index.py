""" index.py

This is a mod_python frontend for accessing collected therm data.
It includes an HTML interface for viewing current data and graphs,
and an XML/RPC interface for remote access via other programs.

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

import sys, os
_thisPath = os.path.realpath(os.path.split(__file__)[0])
_modulesPath = os.path.realpath(os.path.join(_thisPath, "..", "modules"))
sys.path.insert(0, _modulesPath)

import therm_db, units, rrd
import time
import Nouvelle
from Nouvelle import tag, place, xml, ModPython


class ThermRRD(rrd.RrdFile):
    """An RRD updated with values from the therm database"""
    def __init__(self, source, key, filename=None):
        self.source = source
        self.key = key
        rrd.RrdFile.__init__(self, filename, source, key)

    def needsBuilding(self):
        return self.source.getLatestPacket()['id'] > self.getStamp()

    def isEmpty(self):
        # This only checks the most recent packet.. but it's good enough for us
        return self.source.getLatestPacket().get(self.key) is None

    def buildRule(self):
        rrd.RrdFile.buildRule(self)
        self.updatePackets(self.getStamp())

    def updatePackets(self, id, stepSize=20):
        iter = self.source.iterPacketsAfter(id)
        while 1:
            queue = []
            try:
                while len(queue) < stepSize:
                    queue.append(iter.next())
            except StopIteration:
                pass
            if not queue:
                return

            self.update([(packet['time'].ticks(), packet[self.key])
                         for packet in queue if packet.get(self.key)])
            self.setStamp(queue[-1]['id'])

    def rrdInit(self):
        rrd.RrdFile.rrdInit(self)
        self.setStamp(0)

    def getStampFilename(self):
        """Get the filename to use for storing our latest packet ID"""
        return self.filename + ".stamp"

    def getStamp(self):
        try:
            return int(open(self.getStampFilename()).read().strip())
        except IOError:
            return 0

    def setStamp(self, s):
        open(self.getStampFilename(), "w").write("%d\n" % s)


def graphTemperature(source, intervals=("day", "week", "month", "year")):
    name = therm_db.prettifyName(source.name)
    temperatures = ThermRRD(source, 'average')
    images = []
    if not temperatures.isEmpty():
        for interval in intervals:
            images.append(rrd.RrdGraph(
                rrd.graphDefineSource(temperatures) +
                rrd.graphUnknownData() +
                rrd.graphColorRange(-20.0, rrd.Color(0.5, 0.5, 1),
                                    35.0, rrd.Color(1, 1, 0),
                                    id=[0]) +
                rrd.graphSpan("%s - Temperature" % name) +
                rrd.graphFreezingPoint(),
                interval=interval,
                yLabel="Degrees Celsius"))
    return images

def graphVoltage(source, intervals=("week", "month", "year")):
    name = therm_db.prettifyName(source.name)
    images = []
    voltages = ThermRRD(source, 'voltage')
    if not voltages.isEmpty():
        for interval in intervals:
            images.append(rrd.RrdGraph(
                rrd.graphDefineSource(voltages) +
                rrd.graphUnknownData() +
                rrd.graphColorRange(6.0, rrd.Color(1, 1, 1),
                                    9.0, rrd.Color(0, 0, 1),
                                    id=[0]) +
                rrd.graphSpan("%s - Battery Voltage" % name, rrd.Color(1,0,0)),
                interval=interval,
                yLabel="Volts"))
    return images


css = """
.footer {
    text-align: center;
    border-top: 1px solid #777;
    color: #777;
    font: 80% normal;
    padding: 0.5em 0.5em 1em 0.5em;
}

.source {
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
"""


class IndexPage(ModPython.Page):
    document = [
        xml('<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.1//EN"'
            ' "http://www.w3.org/TR/xhtml11/DTD/xhtml11.dtd">\n'),
        tag('html', xmlns="http://www.w3.org/1999/xhtml")[
            tag('head')[
                tag('title')[ "Navi - Therm Server" ],
                xml('<meta http-equiv="refresh" content="30" />'),
                tag('style', type='text/css')[ css ],
            ],
            tag('body')[
                place('sources'),
                tag('div', _class='footer')[
                   "Page generated on ", place('date'),
                ],
            ],
        ],
    ]

    def render_date(self, context):
        return time.ctime()

    def render_sources(self, context):
        results = []
        for source in therm_db.defaultDatabase.iterSources():
            results.append(self.renderSource(source, context))
        return results

    def renderSource(self, source, context):
        latest = source.getLatestPacket()

        info = []

        if latest.get('average'):
            degC = latest['average']
            degF = units.degCtoF(degC)
            info.append(tag('div', _class='temperatures')[
                tag('span', _class='mainTemperature')[ "%.01f" % degF, xml("&deg;"), "F" ],
                tag('span', _class='temperatureSeparator')[ " / " ],
                tag('span', _class='altTemperature')[ "%.01f" % degC, xml("&deg;"), "C" ],
                ])

        info.extend([
            tag('div', _class='name')[ therm_db.prettifyName(source.name) ],
            tag('div', _class='description')[ source.description ],
            self.getThumbnails(context, source),
            ])

        if latest.get('voltage'):
            voltage = latest['voltage']
            minVoltage = 6.5
            maxVoltage = 9.0
            qualitative = (voltage - minVoltage) / (maxVoltage - minVoltage)
            info.append(tag('div', _class='extraInfo')[
                "Battery: ", self.renderBargraph(qualitative), "%.03f V" % voltage,
            ])

        if latest.get('time'):
            delta = time.time() - latest['time'].ticks()
            info.append(tag('div', _class='extraInfo')[
                "Last reading received ",
                tag('strong')[ units.time.format(delta) ], " ago",
            ])

        return tag('div', _class='source')[info]

    def renderBargraph(self, alpha, numBars=30):
        alpha = max(0, min(1, alpha))
        filledBars = int(alpha * numBars + 0.5)
        bars = []
        for i in xrange(numBars):
            if i < filledBars:
                bars.append(tag('span', _class="filledBar")[ " " ])
            else:
                bars.append(tag('span', _class="emptyBar")[ " " ])
        return tag('span', _class="bargraph")[ bars ]

    def getThumbnails(self, context, source):
        results = []

        tempGraphs = graphTemperature(source)
        if tempGraphs:
            results.append(tag('div', _class='thumbnails')[[
                self.makeThumbnailLink(context, i) for i in tempGraphs
            ]])

        voltGraphs = graphVoltage(source)
        if voltGraphs:
            results.append(tag('div', _class='thumbnails')[[
                self.makeThumbnailLink(context, i) for i in voltGraphs
            ]])

        return results

    def makeThumbnailLink(self, context, image):
        """Make a hyperlinked thumbnail to an image"""
        thumb = rrd.ScaledImage(image, height=48)
        thumb.cssClass = 'thumbnail'
        thumb.alt = "Graph Thumbnail"
        thumb.make()
        return tag('a', href=image.getUrl(context))[ thumb ]

index = IndexPage()

### The End ###
