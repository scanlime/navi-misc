#!/usr/bin/env python
#
# A daemon that takes therm readings, updating a web page
# with graphs and a table of current temperatures.
# Uses rrdtool to generate graphs.
#
# Run with the '-f' option to fork into the background.
#
# -- Micah Dowty <micah@picogui.org>
#

import os, time, popen2, sys, xmlrpclib, re


def quote(s):
    """Quote a string to protect shell metacharacters"""
    s.replace('"', '\\"')
    return '"%s"' % s


def rrd(*args):
    """Simple wrapper around rrdtool. If the first argument is
       'graph', this returns a (width, height) tuple with the size
       reported by rrdtool.
       """
    # Quote each argument to protect its valuable spaces from the evil shell
    quotedArgs = [quote(arg) for arg in args]
    command = "rrdtool " + " ".join(quotedArgs)
    child = popen2.Popen3(command, False)

    # If we're graphing, wait for a size line from rrdtool
    if args[0] == 'graph':
        size = map(int, child.fromchild.readline().strip().split("x"))
    else:
        size = None

    # Wait for rrdtool to finish
    if child.wait():
        raise Exception("Error in rrdtool")
    return size


def alphaNum(s):
    """Replace all non-alphanumeric characters in the given string with underscores"""
    return re.sub("[^a-zA-Z0-9\-]", "_", s)


class Color:
    """Represents an RGB color. When converted to a string, a hex color in #RRGGBB form
       results. Component values are floating point, between 0 and 1.
       """
    def __init__(self, red, green, blue):
        self.rgb = (red, green, blue)

    def __str__(self):
        bytes = [int(component * 255 + 0.49) for component in self.rgb]
        return "#%02X%02X%02X" % tuple(bytes)

    def blend(self, color, alpha):
        """Blend this color with the given color according to the given alpha value.
           When alpha==0, returns this color. When alpha==1, returns the given color.
           """
        newRgb = []
        for i in xrange(3):
            newRgb.append(self.rgb[i] * (1-alpha) + color.rgb[i] * alpha)
        return Color(*newRgb)


class Graph:
    """A graph created by rrdtool. Consists of a filename and size,
       can be used to generate HTML image tags. Parameters are all
       rrdtool arguments after 'graph' and the filename.
       """
    def __init__(self, config, name, *params):
        # Create file names and such
        self.name = name
        self.alphanumName = alphaNum(name)
        self.fileName = config['graphFormat'] % self.alphanumName
        tempFile = os.path.join(config['webDir'], config['graphFormat'] % (self.alphanumName + "-temp"))
        webFile = os.path.join(config['webDir'], self.fileName)

        # Write the graph to a temporary file rather than writing it
        # to its final destinatio first, so the web server won't see
        # a partially completed graph.
        self.width, self.height = rrd('graph', tempFile, *params)
        os.rename(tempFile, webFile)

    def tag(self):
        return '<img src="%s" width="%s" height"%s">' % (self.fileName, self.width, self.height)


class Therm:
    """Information about a therm, and methods to store its value and graph RRDs"""
    def __init__(self, config, id, description):
        self.config = config
        self.id = id
        self.description = description
        self.rrdFile = os.path.join(config['rrdDir'], "%s.rrd" % id)
        self.value = None

        # Create the RRD file if it doens't exist
        if not os.path.isfile(self.rrdFile):
            self.rrdInit()

        self.color = Color(0,0,1)

    def rrdInit(self):
        """Create a blank RRD for this therm"""
        rrd("create", self.rrdFile,
            "DS:temperature:GAUGE:600:U:U",
            "RRA:AVERAGE:0.5:1:600",
            "RRA:AVERAGE:0.5:6:700",
            "RRA:AVERAGE:0.5:24:775",
            "RRA:AVERAGE:0.5:288:797",
            "RRA:MAX:0.5:1:600",
            "RRA:MAX:0.5:6:700",
            "RRA:MAX:0.5:24:775",
            "RRA:MAX:0.5:288:797",
            "RRA:MIN:0.5:1:600",
            "RRA:MIN:0.5:6:700",
            "RRA:MIN:0.5:24:775",
            "RRA:MIN:0.5:288:797")

    def update(self, time, value):
        """Update with new data. 'time' should be the UNIX time of the end of the sampling
           period where 'value' was collected.
           """
        self.value = value
        rrd("update", self.rrdFile, "%d:%.04f" % (int(time), value))

    def graph(self, interval):
        """Create a graph of this therm over the given interval.
           Intervals are specified as (name, seconds) tuples, as in the config.
           """
        return Graph(self.config, "%s-%s" % (self.id, interval[0]),
                     # Graph options
                     "--start", "-%d" % interval[1],
                     "--vertical-label", "Degrees Fahrenheit",
                     "--width", str(self.config['graphSize'][0]),
                     "--height", str(self.config['graphSize'][1]),

                     # Min/max ranges for each therm
                     "DEF:temp_min=%s:temperature:MIN" % self.rrdFile,
                     "DEF:temp_max=%s:temperature:MAX" % self.rrdFile,
                     "CDEF:temp_span=temp_max,temp_min,-",
                     "AREA:temp_min",
                     "STACK:temp_span%s" % self.color.blend(Color(1,1,1), 0.7),

                     # Average line
                     "DEF:temp_average=%s:temperature:AVERAGE" % self.rrdFile,
                     "LINE1:temp_average%s:%s" % (self.color, self.description),
                     )


class ThermGrapher:
    """Collects data from a set of thermometers,
       generating graphs and a web page.
       """
    def __init__(self, config={}):
        # Store the default configuration, then update with anything overridden by the caller
        self.config = {
            "server":  "http://navi.picogui.org:4510",    # Server URL
            "webDir":  "/home/httpd/htdocs/therm",        # Directory for web page
            "rrdDir":  "rrd",                             # Directory for RRD files
            "webUpdatePeriod": 10*60,                     # Number of seconds between web page and graph updates
            "webRefreshPeriod": 5*60,                     # Number of seconds between browser refreshes
            "graphSize": (600,200),                       # Size of the graphs' drawing area (not of the final image)
            'graphIntervals': [                           # Intervals to make graphs at: (name,seconds) tuples
               ('6 hours', 60*60*6),
               ("day", 60*60*24),
               ("week", 60*60*24*7),
               ("month", 60*60*24*30),
               ("year", 60*60*24*365),
               ],
            'defaultIntervalName': 'day',                 # Name of the graph interval to show by default
            'graphFormat': "%s.png",                      # Image format string for graphs
            }
        self.config.update(config)
        
        self.server = xmlrpclib.ServerProxy(self.config['server'])
        self.running = False
        self.lastWebUpdate = 0

        self.loadTherms()

    def loadTherms(self):
        """From the server, get a mapping from therm IDs to descriptions, use it to create Therm instances
           which will store data needed to graph each sensor's readings. We store a map
           from therm IDs to instances to easily update them with new data, and a list
           of therms sorted by description, for easy display.
           """
        self.thermMap = {}
        self.sortedTherms = []
        for id, description in self.server.getDescriptions().items():
            t = Therm(self.config, id, description)
            self.thermMap[id] = t
            self.sortedTherms.append(t)
        self.sortedTherms.sort(lambda a,b: cmp(a.description, b.description))

    def run(self):
        """Enter a loop collecting temperature data and updating the graphs and web page"""
        self.running = True
        while self.running:
            self.updateTherms()
            self.webUpdate()
            self.waitForAveragePeriod()

    def webUpdate(self):
        """If it's time for an update, generate the graphs and web page"""
        now = time.time()
        if now > (self.lastWebUpdate + self.config['webUpdatePeriod']):
            self.createWebPages()
            self.lastWebUpdate = now

    def updateTherms(self):
        """Get a new list of temperature averages from the server and update our Therm objects"""
        self.serverTimes = self.server.getTimes()
        avg = self.server.getAverages()
        for id, value in avg.iteritems():
            self.thermMap[id].update(self.serverTimes['periodStart'], value)

    def createWebPages(self):
        """Generate web pages for each interval, with graphs and current temperatures"""
        # Create a map from web page name to interval.
        # Our default interval (the first one in our config)
        # will be renamed to index.html
        pageMap = {}
        for interval in self.config['graphIntervals']:
            if interval[0] == self.config['defaultIntervalName']:
                pageMap[interval] = "index.html"
            else:
                pageMap[interval] = "%s.html" % alphaNum(interval[0])

        # Write each HTML file...
        # Write to a temporary file then move it to its final location,
        # so the web server never sees any half-updated pages.
        for interval, fileName in pageMap.iteritems():
            tempFilePath = os.path.join(self.config['webDir'], "temp-" + fileName)
            filePath = os.path.join(self.config['webDir'], fileName)
            
            f = open(os.path.join(self.config['webDir'], tempFilePath), "w")
            f.write("<html>\n")
            f.write('<meta HTTP-EQUIV="Refresh" CONTENT="%d">' % self.config['webRefreshPeriod'])
            f.write('<meta HTTP-EQUIV="Pragma" CONTENT="no-cache">')
            f.write("<head><title>Temperatures - last %s</title></head></body>\n\n" % interval[0])

            # Links to the other interval pages
            for linkInterval in self.config['graphIntervals']:
                if linkInterval == interval:
                    f.write('[%s] ' % linkInterval[0])
                else:
                    f.write('[<a href="%s">%s</a>] ' % (pageMap[linkInterval], linkInterval[0]))
            f.write("\n<hr>\n")
            f.write("Last updated %s\n\n" % time.strftime("%c", time.localtime()))

            # A table, with a row for each therm. Shows a graph on the left, current temperature on the right
            f.write("<table>\n")
            for therm in self.sortedTherms:
                f.write("<tr>\n")
                f.write("\t<td>%s</td>\n" % therm.graph(interval).tag())
                f.write('\t<td><table padding="10"><tr><td><font size="+1">\n')
                f.write("\t\t<p>%s</p><p>%.01f &deg;F</p>\n" % (therm.description, therm.value))
                f.write("\t</font></td></tr></table></td>\n")
                f.write("</tr>\n")
            f.write("</table>\n")

            # Footer with some info on us
            f.write("\n<hr><center>\n")
            f.write('<a href="http://navi.picogui.org/svn/misc/trunk/therm/">rrdtherm</a>,\n')
            f.write('built with <a href="http://navi.picogui.org/svn/misc/trunk/rcpod/">rcpod</a>\n')
            f.write('and <a href="http://ee-staff.ethz.ch/~oetiker/webtools/rrdtool/">rrdtool</a>\n')
            f.write('</center></body></html>\n')

            # Close the file, move the temp file to the final location
            f.close()
            os.rename(tempFilePath, filePath)


def daemonize(pidfile=None):
    """Daemonize this process with the UNIX double-fork trick.
       Writes the new PID to the provided file name if not None.
       """
    pid = os.fork()
    if pid > 0:
        sys.exit(0)
    os.setsid()
    os.umask(0)
    pid = os.fork()
    if pid > 0:
        if pidfile:
            open(pidfile, "w").write(str(pid))
        sys.exit(0)
 

if __name__ == '__main__':
    # The -f option forks into the background
    if len(sys.argv) > 1 and sys.argv[1] == '-f':
        daemonize("rrdtherm.pid")
    ThermGrapher().run()

### The End ###
