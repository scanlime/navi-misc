""" pytherm.rrd

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
from pytherm import units


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
    size = None

    def getUrl(self, context):
        req = context['request']
        return pathToURL(req, self.filename)

    def render(self, context):
        if self.size is None:
            self.determineSize()
        return tag('img', src=self.getUrl(context), width=self.size[0], height=self.size[1],
                   _class=self.cssClass, alt=self.alt)

    def determineSize(self):
        self.size = Image.open(self.filename).size


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

    def __init__(self, params, size=(600,150), yLabel=None, interval="day"):
        prepend = [
            "--width", size[0],
            "--height", size[1],
            "--start", -units.time.lookupName(interval),
            ]
        if yLabel:
            prepend.extend(["--vertical-label", yLabel])
        BuildTarget.__init__(self, None, *(prepend + list(params)))

    def buildRule(self):
        self.size = rrd('graph', self.filename, *map(str, self.dependencies))


class RrdDef(BuildTarget):
    """A DEF parameter for rrdtool, linking an RrdFile with a particular variable."""
    def __init__(self, varName, rrd, cf='AVERAGE', dsName='x'):
        BuildTarget.__init__(self, None, varName, rrd, dsName, cf)

    def __repr__(self):
        return "<RrdDef %s>" % self

    def __str__(self):
        return "DEF:%s=%s:%s:%s" % (self.dependencies[0],
                                    self.dependencies[1].filename,
                                    self.dependencies[2],
                                    self.dependencies[3])


class RrdFile(BuildTarget):
    """An RRD file, usable as a dependency to RrdDef"""
    fileExtension = 'rrd'

    def buildRule(self):
        if not os.path.isfile(self.filename):
            self.rrdInit()

    def rrdInit(self):
        """Create a blank RRD for this therm"""
        stepSize = 60

        # Define the steps and rows for each RRA in each CF (see the rrdcreate manpage)
        nSamples = 60*60*24*2 / stepSize
        rraList = [
            (1,   nSamples),   # Store every sample for the last 2 days
            (7,   nSamples),   # Every 7th sample for the last 2 weeks
            (31,  nSamples),   # Every 31st sample for the last 2 months
            (365, nSamples),   # Every 365th sample for the last 2 years
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

def graphDefineSource(rrd):
    """Define variables for the minimum, maximum, average, and span on an RRD"""
    return [
        RrdDef('data_min', rrd, 'MIN'),
        RrdDef('data_max', rrd, 'MAX'),
        RrdDef('data_average', rrd),
        "CDEF:data_span=data_max,data_min,-",
        ]

def graphUnknownData(title="No data",
                     color=Color(1,1,0).blend(Color(1,1,1), 0.8)):
    return [
        "CDEF:no_data=data_min,data_max,+,UN,INF,UNKN,IF",
        "AREA:no_data%s:%s" % (color, title),
        ]

def graphFreezingPoint(color=Color(0,0,0)):
    return ["HRULE:0%s" % color]

def graphSpan(description, color=Color(0,0,1), alpha=0.7, background=Color(1,1,1)):
    """Graph a line along the average, and a translucent band between its min and max"""
    return [
        # Thick line to separate the minimum from fills
        "LINE3:data_min%s" % background,

        # Min/max range
        "AREA:data_min",
        "STACK:data_span%s" % color.blend(background, alpha),

        # Average
        "LINE1:data_average%s:%s" % (color, description),
        ]

def graphColorSlice(min, max, color, id=[0]):
    """If the average is between 'min' and 'max', fill the area with 'color'."""
    id[0] += 1
    return [
        "CDEF:s%d=data_average,%s,%s,LIMIT" % (id[0], min, max),
        "AREA:s%d%s" % (id[0], color),
        ]

def graphColorRange(min, minColor, max, maxColor, numSlices=64, id=[0]):
    """Graph a linearly interpolated color range, given the minimum and maximum
       data values with their corresponding color. Data outside the minimum and maximum
       gets clamped.
       """
    l = (graphColorSlice("-INF", min, minColor, id=id) +
         graphColorSlice(max, "INF", maxColor, id=id))
    prevAlpha = 0
    while prevAlpha < 1:
        alpha = prevAlpha + 1.0 / numSlices
        l += graphColorSlice(prevAlpha*(max-min)+min,
                             alpha*(max-min)+min,
                             minColor.hsvBlend(maxColor, (alpha + prevAlpha) * 0.5),
                             id=id)
        prevAlpha = alpha
    return l

### The End ###
