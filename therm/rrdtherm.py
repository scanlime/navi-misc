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

import os, i2c, time, popen2, sys


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


class ThermGrapher:
    """Collects data from a set of thermometers,
       generating graphs and a web page.
       """
    def __init__(self,
                 sampler       = None,
                 therms        = None,
                 webDir        = "/home/httpd/htdocs/therm", # Directory for web page
                 rrdDir        = "rrd",    # Directory for RRD files
                 averagePeriod = 2*60,     # Number of seconds to average for each sample
                 webUpdatePeriod = 10*60,  # Number of seconds between web page and graph updates
                 webRefreshPeriod = 5*60,  # Number of seconds between browser refreshes
                 graphSize     = (600,250) # Size of the graphs' drawing area (not of the final image)
                 ):
        if not sampler:
            sampler = i2c.ThermSampler()
        if not therms:
            therms = i2c.getTherms()
        else:
            therms = list(therms)

        self.sampler = sampler
        self.therms = therms
        self.webDir = webDir
        self.rrdDir = rrdDir
        self.averagePeriod = averagePeriod
        self.webUpdatePeriod = webUpdatePeriod
        self.webRefreshPeriod = webRefreshPeriod
        self.graphSize = graphSize

        self.running = False
        self.averages = {}         # Average temperatures, converted to fahrenheit
        self.lastWebUpdate = 0     # Time of the last web page update
        self.actualGraphSizes = {} # interval name -> (width, height)

        # Sort our list of therms by name
        therms.sort(lambda a,b: cmp(a.description, b.description))

        # Set available colors for the therms to use- these
        # are removed as they're allocated by rrdInit
        self.thermColors = [
            Color(1, 0, 0.5),
            Color(0, 0.8, 0),
            Color(0, 0, 0.8),
            Color(1, 0, 0),
            ]

        # Initialize RRD data for each therm
        for therm in therms:
            self.rrdInit(therm)

        # Different intervals to make graphs at: (name, seconds) tuples
        self.graphIntervals = [
            ("day", 60*60*24),
            ("week", 60*60*24*7),
            ("month", 60*60*24*30),
            ("year", 60*60*24*365),
            ]

        # Image format for graphs
        self.graphFormat = "%s.png"

    def rrdInit(self, therm):
        """Per-therm initialization necessary for rrdtool graphing.
           Assigns an rrd filename, virtual name, and color to each therm.
           Creates the rrd file if necessary.
           """
        therm.vname = "t%d_%d" % (therm.bus, therm.address)
        therm.rrdfile = os.path.join(self.rrdDir, "%d_%d.rrd" % (therm.bus, therm.address))
        therm.color = self.thermColors.pop()

        if not os.path.isfile(therm.rrdfile):
            rrd("create", therm.rrdfile,
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

    def run(self):
        """Enter a loop collecting temperature data and updating the graphs and web page"""
        # Go ahead and update the graphs once before taking readings, so
        # it's easier to debug the graph code.
        self.createGraphs()
        self.createWebPage()

        self.running = True
        while self.running:
            self.collectSamples()
            self.rrdUpdate()
            self.webUpdate()

    def collectSamples(self):
        """Collect samples over the current averaging period, and save
           the resulting averages from each therm.
           """
        periodStart = time.time()
        while time.time() < (periodStart + self.averagePeriod):
            self.sampler.update(self.therms)
        for therm in self.therms:
            self.averages[therm] = i2c.toFahrenheit(therm.getAverage())

    def rrdUpdate(self):
        """Update the RRDs with current averages"""
        for therm in self.therms:
            rrd("update", therm.rrdfile, "N:%s" % self.averages[therm])

    def webUpdate(self):
        """If it's time for an update, generate the graphs and web page"""
        now = time.time()
        if now > (self.lastWebUpdate + self.webUpdatePeriod):
            self.createGraphs()
            self.createWebPage()
            self.lastWebUpdate = now

    def createWebPage(self):
        """Generate a web page with current temperature averages and graphs"""
        index = open(os.path.join(self.webDir, "index.html"), "w")
        index.write("<html>\n")
        index.write('<meta HTTP-EQUIV="Refresh" CONTENT="%d">' % self.webRefreshPeriod)
        index.write('<meta HTTP-EQUIV="Pragma" CONTENT="no-cache">')
        index.write("<head><title>Temperatures</title></head></body>\n\n")

        # Current temperatures table
        index.write('<p>Current Temperatures:')
        index.write('<table border="1" cellspacing="1" cellpadding="4">\n')
        for therm in self.therms:
            if self.averages.has_key(therm):
                index.write("<tr><td>%s</td><td>%.01f &deg;F</td></tr>\n" %
                            (therm.description, self.averages[therm]))
        index.write("</table>\n")
        index.write("updated %s\n" % time.strftime("%c", time.localtime()))
        index.write("</p>\n\n")

        # Graphs
        for name, duration in self.graphIntervals:
            file = self.graphFormat % name
            size = self.actualGraphSizes[name]
            index.write('<p><img src="%s" width="%d" height="%d" alt="last %s"></p>\n' %
                        (file, size[0], size[1], name))

        # A little info about us
        index.write("""
<hr><center>
<a href="http://navi.picogui.org/svn/misc/trunk/therm/">rrdtherm</a>,
built with <a href="http://navi.picogui.org/svn/misc/trunk/rcpod/">rcpod</a>
and <a href="http://ee-staff.ethz.ch/~oetiker/webtools/rrdtool/">rrdtool</a>
</center></body></html>
""")

    def createGraphs(self):
        """Generate graph images for the web page"""
        timestamp = time.strftime("%c", time.localtime())
        
        for name, duration in self.graphIntervals:
            # We write the graph to a temporary file, then move it to its final
            # location. This prevents the web server from reading a partially
            # completed image.
            file = os.path.join(self.webDir, self.graphFormat % name)
            tempFile = os.path.join(self.webDir, self.graphFormat % (name + "-temp"))

            # Start building an rrdtool command line.
            # Add options that affect the whole graph:
            args = ["graph", tempFile,
                    "--start", "-%d" % duration,
                    "--vertical-label", "degrees fahrenheit",
                    "--width", str(self.graphSize[0]),
                    "--height", str(self.graphSize[1]),
                    "--title", "temperatures - last %s (%s)" % (name, timestamp),
                    ]

            # Draw min/max ranges for each therm
            for therm in self.therms:
                args.extend([
                    "DEF:%s_min=%s:temperature:MIN" % (therm.vname, therm.rrdfile),
                    "DEF:%s_max=%s:temperature:MAX" % (therm.vname, therm.rrdfile),
                    "CDEF:%s_span=%s_max,%s_min,-" % (therm.vname, therm.vname, therm.vname),
                    "AREA:%s_min" % therm.vname,
                    "STACK:%s_span%s" % (therm.vname, therm.color.blend(Color(1,1,1), 0.7)),
                    ])

            # Draw averages for each therm
            for therm in self.therms:
                args.extend([
                    "DEF:%s_average=%s:temperature:AVERAGE" % (therm.vname, therm.rrdfile),
                    "LINE1:%s_average%s:%s" % (therm.vname, therm.color, therm.description),
                    ])

            # Make the graph, save its size
            self.actualGraphSizes[name] = rrd(*args)

            # Move it to its final location
            os.rename(tempFile, file)


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
