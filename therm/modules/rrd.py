""" rrd

A simple Python interface to rrdtool

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

from Nouvelle import tag
import os, time, popen2, sys, re, colorsys, sha
import Image


def shellEscape(s):
    return re.sub(r"([^0-9A-Za-z_\./:-])", r"\\\1", s)


def rrd(*args):
    """Simple wrapper around rrdtool. If the first argument is
       'graph', this returns a (width, height) tuple with the size
       reported by rrdtool.
       """
    # Quote each argument to protect its valuable spaces from the evil shell
    command = " ".join(["rrdtool"] + [shellEscape(str(arg)) for arg in args])
    child = popen2.Popen4(command)

    # If we're graphing, wait for a size line from rrdtool
    line = child.fromchild.readline().strip()
    size = None

    if args[0] == 'graph':
        tokens = line.split("x")
        if len(tokens) != 2:
            raise Exception("Unexpected response from rrdtool: %r" % line)
        size = map(int, tokens)
    elif args[0] == 'create':
        if line:
            raise Exception("Unexpected response from rrdtool: %r" % line)

    # Wait for rrdtool to finish
    if child.wait():
        raise Exception("Error in rrdtool")
    return size


class Timestamped:
    """Abstract base class for an object that has a timestamp"""
    def getTimestamp(self):
        return None


class TimestampedFile(Timestamped):
    """Abstract base class for a file acting as a timestamp"""
    def __init__(self, filename):
        self.filename = filename

    def __repr__(self):
        return "<%s %r>" % (self.__class__.__name__, self.filename)

    def getTimestamp(self):
        try:
            return os.stat(self.filename).st_mtime
        except OSError:
            return None


class BuildTarget(TimestampedFile):
    """Abstract object representing a timestamped file that is generated
       from some other timestamped object.
       """
    fileExtension = None

    def __init__(self, filename=None, *dependencies):
        if filename is None:
            filename = self.newTempFilename(dependencies, self.fileExtension)
        TimestampedFile.__init__(self, filename)
        self.dependencies = dependencies

    def newTempFilename(self, dependencies, fileExtension=None, tempDir=None):
        """Automatically generate a temporary file name from the dependencies.
           The file name is a hash of our dependencies' repr()s.
           """
        if tempDir is None:
            thisDir = os.path.split(os.path.realpath(__file__))[0]
            tempDir = os.path.realpath(os.path.join(thisDir, "..", "web", "tmp"))
        digest = sha.new()
        for dep in dependencies:
            digest.update(repr(dep))
        name = digest.hexdigest()
        if fileExtension:
            name = "%s.%s" % (name, fileExtension)
        return os.path.join(tempDir, name)

    def buildRule(self):
        """This is defined by subclasses, to regenerate the target"""
        pass

    def make(self):
        """Make the target if necessary"""
        for dep in self.dependencies:
            if isinstance(dep, BuildTarget):
                dep.make()
        if self.needsBuilding():
            self.buildRule()

    def needsBuilding(self):
        """Using time stamps, determine whether this needs building"""
        thisTimestamp = self.getTimestamp()
        if thisTimestamp is None:
            return True
        for dependency in self.dependencies:
            if not isinstance(dependency, Timestamped):
                continue
            depStamp = dependency.getTimestamp()
            if depStamp is not None and depStamp > thisTimestamp:
                return True
        return False


def lerp(a, b, alpha):
    """Linear interpolation, returns a when alpha=0, b when alpha=1"""
    if type(a) == type([]) or type(a) == type(()):
        # If we have lists, recursively lerp all items in them
        return [lerp(a[i], b[i], alpha) for i in xrange(len(a))]
    else:
        return a*(1-alpha) + b*alpha


class Color:
    """Represents an RGB color, which may be accessed in RGB or HSV colorspaces.
       When converted to a string, a hex color in #RRGGBB form
       results. Component values are floating point, between 0 and 1.
       Colors can commonly be constructed in the following forms:

          RGB:  Color(1, 0.5, 0)
          RGB:  Color(rgb=(1, 0.5, 0))
          HSV:  Color(hsv=(0.2, 1, 1))
       """
    def __init__(self, red=0, green=0, blue=0, rgb=None, hsv=None):
        if hsv:
            self.rgb = colorsys.hsv_to_rgb(*hsv)
        elif rgb:
            self.rgb = rgb
        else:
            self.rgb = (red, green, blue)

    def __str__(self):
        bytes = [int(component * 255 + 0.49) for component in self.rgb]
        return "#%02X%02X%02X" % tuple(bytes)

    def blend(self, color, alpha):
        """Blend this color with the given color according to the given alpha value.
           When alpha==0, returns this color. When alpha==1, returns the given color.
           Blends in RGB colorspace.
           """
        return Color(rgb=lerp(self.rgb, color.rgb, alpha))

    def hsvBlend(self, color, alpha):
        """Like blend, but in HSV colorspace"""
        return Color(hsv=lerp(
            colorsys.rgb_to_hsv(*self.rgb),
            colorsys.rgb_to_hsv(*color.rgb),
            alpha))


def makeRelPath(fromPath, toPath):
    """Create a relative path that when evaluated relative to 'fromPath'
       will refer to the path in 'toPath'.
       """
    fromPath = os.path.realpath(fromPath)
    toPath = os.path.realpath(toPath)
    segments = []

    # Back up as much as we need to
    while 1:
        base = os.path.realpath(os.path.join(fromPath, *segments))
        if toPath.startswith(base):
            break
        else:
            segments.append("..")

    # Add the rest
    for segment in toPath[len(base):].split(os.sep):
        if segment:
            segments.append(segment)
    return os.sep.join(segments)


def pathToURL(request, path):
    """Try to guess a URL for the given path using our current request"""
    pathDiff = makeRelPath(request.filename, path)
    return os.path.abspath(os.path.join(request.uri, pathDiff))


class ImageBuildTarget(BuildTarget):
    """A build target that's also an image. The buildRule should fill in
       width and height. These images are renderable by Nouvelle.
       """
    cssClass = None
    alt = None
    size = (None, None)

    def getUrl(self, context):
        req = context['request']
        return pathToURL(req, self.filename)

    def render(self, context):
        return tag('img', src=self.getUrl(context), width=self.size[0], height=self.size[1],
                   _class=self.cssClass, alt=self.alt)


class ScaledImage(ImageBuildTarget):
    """Rescale the input image. If only width or height are provided,
       the resulting image has the same aspect ratio as the input image.
       """
    fileExtension = 'png'

    def __init__(self, original, width=None, height=None, filename=None):
        assert not (width is None and height is None)
        self.scaleTo = (width, height)
        ImageBuildTarget.__init__(self, filename, original)

    def buildRule(self):
        i = Image.open(self.dependencies[0].filename)

        width, height = self.scaleTo
        if width is None:
            width = i.size[0] * height / i.size[1]
        if height is None:
            height = i.size[1] * width / i.size[0]
        self.size = (width, height)

        # Our thumbnails look much better if we paste the image into
        # a larger transparent one first with a margin about equal to one
        # pixel in our final thumbnail size. This smoothly blends
        # the edge of the image to transparent rather than chopping
        # off a fraction of a pixel. It looks, from experimentation,
        # like this margin is only necessary on the bottom and right
        # sides of the image.
        margins = (i.size[0] // self.size[0] + 1,
                   i.size[1] // self.size[1] + 1)
        bg = Image.new("RGBA",
                       (i.size[0] + margins[0],
                        i.size[1] + margins[1]),
                       (255, 255, 255, 0))
        bg.paste(i, (0,0))

        bg.thumbnail(self.size, Image.ANTIALIAS)
        bg.save(self.filename, 'PNG')


class RrdGraph(ImageBuildTarget):
    """A graph created by rrdtool from one or more RRDs. Dependencies are all
       rrdtool parameters- either strings interpreted as command line parameters,
       or RrdDef objects.
       """
    fileExtension = 'png'

    def __init__(self, *params):
        BuildTarget.__init__(self, None, *params)

    def buildRule(self):
        self.size = rrd('graph', self.filename, *map(str, self.dependencies))


class RrdDef(BuildTarget):
    """A DEF parameter for rrdtool, linking an RrdFile with a particular variable."""
    def __init__(self, varName, rrd, dsName, cf):
        BuildTarget.__init__(self, None, varName, rrd, dsName, cf)

    def __str__(self):
        return "DEF:%s=%s:%s:%s" % (self.dependencies[0],
                                    self.dependencies[1].filename,
                                    self.dependencies[2],
                                    self.dependencies[3])


class RrdFile(BuildTarget):
    """An RRD file, built from a DataSource object"""
    fileExtension = 'rrd'

    def buildRule(self):
        if not os.path.isfile(self.filename):
            self.rrdInit()

    def rrdInit(self):
        """Create a blank RRD for this therm"""
        stepSize = 60

        # Define the steps and rows for each RRA in each CF (see the rrdcreate manpage)
        rraList = [
            (1, 60*60*24*2 / stepSize),             # Store every sample for the last 2 days
            (7, 60*60*24*7*2 / stepSize / 7),       # Every 7th sample for the last 2 weeks
            (31, 60*60*24*31*2 / stepSize / 31),    # Every 31st sample for the last 2 months
            (365, 60*60*24*365*2 / stepSize / 365), # Every 365th sample for the last 2 years
            ]

        # Start out with the parameters to define our RRD file and data source
        params = ["create", self.filename,
                  "--step", stepSize,
                  "--start", int(time.time() - 60*60*24*365*2),
                  "DS:x:GAUGE:%s:U:U" % int(stepSize * 10)]

        # Define RRAs with all the combining functions we're interested in
        for cf in ("AVERAGE", "MIN", "MAX"):
            for steps, rows in rraList:
                params.append("RRA:%s:0.5:%s:%s" % (cf, steps, rows))

        rrd(*params)

    def update(self, pairs):
        """Update with new data, provided as a list of time/value pairs"""
        rrd("update", self.filename, *["%d:%.04f" % (int(time), value) for time, value in pairs])


#     def graph(self, interval):
#         """Create a graph of this therm over the given interval.
#            Intervals are specified as (name, seconds) tuples, as in the config.
#            """
#         lineColor = Color(0,0,1)
#         noDataColor = Color(1,1,0).blend(Color(1,1,1), 0.8)
#         freezingColor = Color(0,0,0)

#         params = [
#             # Graph options
#             "--start", "-%d" % interval[1],
#             "--vertical-label", "Degrees Fahrenheit",
#             "--width", str(self.config['graphSize'][0]),
#             "--height", str(self.config['graphSize'][1]),

#             # Data sources
#             "DEF:temp_min=%s:temp:MIN" % self.rrdFile,
#             "DEF:temp_max=%s:temp:MAX" % self.rrdFile,
#             "CDEF:temp_span=temp_max,temp_min,-",
#             "DEF:temp_average=%s:temp:AVERAGE" % self.rrdFile,
#             ]

#         # Create a color gradient indicating the hot/coldness of the current temperature.
#         warmthMin = 0
#         warmthMax = 0.02
#         sliceNum = 0
#         while warmthMin < 1:
#             # Convert the current 'warmth' values to a color and a temperature range
#             if warmthMin > 0:
#                 tempMin = warmthMin * 100
#             else:
#                 tempMin = "-INF"
#             if warmthMax < 1:
#                 tempMax = warmthMax * 100
#             else:
#                 tempMax = "INF"

#             warmthAvg = (warmthMin + warmthMax)/2.0
#             color = Color(0, 0, 1).hsvBlend(Color(1, 0.5, 0), warmthAvg)

#             params.extend([
#                 "CDEF:s%d=temp_average,%s,%s,LIMIT" % (sliceNum, tempMin, tempMax),
#                 "AREA:s%d%s" % (sliceNum, color),
#                 ])

#             # Next slice...
#             warmthMin, warmthMax = warmthMax, warmthMax + (warmthMax - warmthMin)
#             sliceNum += 1

#         params.extend([
#             # Thick white line at the minimum to separate it from the gradient
#             "LINE3:temp_min%s" % Color(1,1,1),

#             # Min/max ranges for each therm
#             "AREA:temp_min",
#             "STACK:temp_span%s" % lineColor.blend(Color(1,1,1), 0.7),

#             # Average line
#             "LINE1:temp_average%s:%s" % (lineColor, self.description),

#             # Unknown data
#             "CDEF:no_data=temp_max,temp_min,+,UN,INF,UNKN,IF",
#             "AREA:no_data%s:No data" % noDataColor,

#             # Freezing point
#             "HRULE:32%s" % freezingColor,
#             ])

#         return Graph(self.config, "%s-%s" % (self.id, interval[0]), *params)


# class ThermGrapher:
#     """Collects data from a set of thermometers,
#        generating graphs and a web page.
#        """
#     def __init__(self, config={}):
#         # Store the default configuration, then update with anything overridden by the caller
#         self.config = {
#             "server":  "http://navi.picogui.org:4510",    # Server URL
#             "webDir":  "/home/httpd/htdocs/therm",        # Directory for web page
#             "rrdDir":  "rrd",                             # Directory for RRD files
#             "webUpdatePeriod": 10*60,                     # Number of seconds between web page and graph updates
#             "webRefreshPeriod": 5*60,                     # Number of seconds between browser refreshes
#             'averagePeriod': 60,                          # Number of seconds the thermserver should average samples for
#             "graphSize": (600,150),                       # Size of the graphs' drawing area (not of the final image)
#             'graphIntervals': [                           # Intervals to make graphs at: (name,seconds) tuples
#                ('6 hours', 60*60*6),
#                ("day", 60*60*24),
#                ("week", 60*60*24*7),
#                ("month", 60*60*24*30),
#                ("year", 60*60*24*365),
#                ],
#             'defaultIntervalName': 'day',                 # Name of the graph interval to show by default
#             'graphFormat': "%s.png",                      # Image format string for graphs
#             }
#         self.config.update(config)

#         # Create a proxy object we use to make XML-RPC calls to the server,
#         # and make sure its averaging period is set correctly
#         self.server = xmlrpclib.ServerProxy(self.config['server'])
#         self.server.setAveragePeriod(self.config['averagePeriod'])

#         self.running = False
#         self.lastWebUpdate = 0

#         self.loadTherms()

#     def loadTherms(self):
#         """From the server, get a mapping from therm IDs to descriptions, use it to create Therm instances
#            which will store data needed to graph each sensor's readings. We store a map
#            from therm IDs to instances to easily update them with new data, and a list
#            of therms sorted by description, for easy display.
#            """
#         self.thermMap = {}
#         self.sortedTherms = []
#         for id, description in self.server.getDescriptions().items():
#             t = Therm(self.config, id, description)
#             self.thermMap[id] = t
#             self.sortedTherms.append(t)
#         self.sortedTherms.sort(lambda a,b: cmp(a.description, b.description))

#     def run(self):
#         """Enter a loop collecting temperature data and updating the graphs and web page"""
#         self.running = True
#         while self.running:
#             self.updateTherms()
#             self.webUpdate()
#             self.waitForAveragePeriod()

#     def waitForAveragePeriod(self):
#         """Wait until the next averaging period on the server has started"""
#         remaining = (self.serverTimes['averagePeriod'] +
#                      self.serverTimes['periodStart'] -
#                      self.serverTimes['time'])
#         time.sleep(remaining + 5)

#     def webUpdate(self):
#         """If it's time for an update, generate the graphs and web page"""
#         now = time.time()
#         if now > (self.lastWebUpdate + self.config['webUpdatePeriod']):
#             self.createWebPages()
#             self.lastWebUpdate = now

#     def updateTherms(self):
#         """Get a new list of temperature averages from the server and update our Therm objects"""
#         self.serverTimes = self.server.getTimes()
#         avg = self.server.getAverages()
#         for id, value in avg.iteritems():
#             if value is not None:
#                 self.thermMap[id].update(self.serverTimes['periodStart'], value)

#     def createWebPages(self):
#         """Generate web pages for each interval, with graphs and current temperatures"""
#         # Create a map from web page name to interval.
#         # Our default interval (the first one in our config)
#         # will be renamed to index.html
#         pageMap = {}
#         for interval in self.config['graphIntervals']:
#             if interval[0] == self.config['defaultIntervalName']:
#                 pageMap[interval] = "index.html"
#             else:
#                 pageMap[interval] = "%s.html" % alphaNum(interval[0])

#         # Write each HTML file...
#         # Write to a temporary file then move it to its final location,
#         # so the web server never sees any half-updated pages.
#         for interval, fileName in pageMap.iteritems():
#             tempFilePath = os.path.join(self.config['webDir'], "temp-" + fileName)
#             filePath = os.path.join(self.config['webDir'], fileName)

#             f = open(os.path.join(self.config['webDir'], tempFilePath), "w")
#             f.write("<html>\n")
#             f.write('<meta HTTP-EQUIV="Refresh" CONTENT="%d">' % self.config['webRefreshPeriod'])
#             f.write('<meta HTTP-EQUIV="Pragma" CONTENT="no-cache">')
#             f.write("<head><title>Temperatures - last %s</title></head></body>\n\n" % interval[0])

#             # Links to the other interval pages
#             for linkInterval in self.config['graphIntervals']:
#                 if linkInterval == interval:
#                     f.write('[%s] ' % linkInterval[0])
#                 else:
#                     f.write('[<a href="%s">%s</a>] ' % (pageMap[linkInterval], linkInterval[0]))
#             f.write("\n<hr>\n")
#             f.write("Last updated %s\n\n" % time.strftime("%c", time.localtime()))

#             # A table, with a row for each therm. Shows a graph on the left, current temperature on the right
#             f.write("<table>\n")
#             for therm in self.sortedTherms:
#                 f.write("<tr>\n")
#                 f.write("\t<td>%s</td>\n" % therm.graph(interval).tag())
#                 f.write('\t<td><table cellspacing="10"><tr><td><font size="+1">\n')
#                 f.write("\t\t<p>%s</p><p>%.01f &deg;F</p>\n" % (therm.description, therm.value))
#                 f.write("\t</font></td></tr></table></td>\n")
#                 f.write("</tr>\n")
#             f.write("</table>\n")

#             # Footer with some info on us
#             f.write("\n<hr><center>\n")
#             f.write('<a href="http://navi.picogui.org/svn/misc/trunk/therm/">rrdtherm</a>,\n')
#             f.write('built with <a href="http://navi.picogui.org/svn/misc/trunk/rcpod/">rcpod</a>\n')
#             f.write('and <a href="http://ee-staff.ethz.ch/~oetiker/webtools/rrdtool/">rrdtool</a>\n')
#             f.write('</center></body></html>\n')

#             # Close the file, move the temp file to the final location
#             f.close()
#             os.rename(tempFilePath, filePath)

### The End ###
