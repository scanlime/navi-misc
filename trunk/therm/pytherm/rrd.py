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
import os, time, popen2, sys, re, md5
import threading, colorsys, random, shutil
import Image
from pytherm import units


class RRDException(Exception):
    pass


_rrdtool = {}

def rrd(*args):
    """Simple wrapper around rrdtool. It takes parameters to pass to rrdtool, and returns
       a list of lines returned from it. This keeps a persistent rrdtool process running,
       and streams commands to it over a pipe.
       """
    # We have one rrdtool instance per thread
    global _rrdtool
    thisThread = threading.currentThread()
    if not thisThread in _rrdtool:
        _rrdtool[thisThread] = popen2.Popen3("rrdtool -")
    rrdtool = _rrdtool[thisThread]

    command = " ".join(['"%s"' % str(arg).replace('"','') for arg in args])
    rrdtool.tochild.write(command + "\n")
    rrdtool.tochild.flush()

    # Collect results until the "OK" line
    results = []
    while 1:
        line = rrdtool.fromchild.readline()
        if not line:
            _rrdtool[thisThread] = None
            raise RRDException("Unexpected EOF from rrdtool")
        line = line.strip()
        results.append(line)
        if line.startswith("OK "):
            break

    # Look for errors only after receiving everything, so our rrdtool
    # instance stays in sync.
    for line in results:
        if line.startswith("ERROR:"):
            raise RRDException("Error %r in command %r" % (line, command))

    return results


def getTempDir():
    """Return a temporary directory to use. This is in /tmp, and is
       based on a hash of our path- so individual installations of
       the therm software get distinct temp directories.
       """
    return "/tmp/therm-%08X" % abs(hash(__file__.replace("pyc", "py")))


def escapeForPath(s):
    """Escape a string such that any characters unsuitable for use
       in a file or directory name are gone"""
    return s.replace(os.sep, "")


class CachedResource:
    """A resource, represented as a file on disk, that is generated
       and cached atomically. Cached items are identified by a name
       and a stamp- the name identifies the cached resource at any
       point in time, the stamp represents the object's freshness.

       On disk, every resource name is assigned to a directory. Files
       named according to the stamp are stored within. While a resource
       is being created, a temporary file is created. When it's finished,
       it is atomically installed by renaming.
       """
    stampType = int

    def __init__(self, name):
        self.name = name
        self.path = os.path.join(getTempDir(), "r.%s" % escapeForPath(name))

    def getFile(self, stamp):
        """Get the final file name for a particular stamp"""
        name = escapeForPath(str(stamp))
        return os.path.join(self.path, name)

    def stampExists(self, stamp):
        return os.path.isfile(self.getFile(stamp))

    def iterStamps(self):
        """Iterate over all stamps for which we have finished resources"""
        try:
            names = os.listdir(self.path)
        except OSError:
            return
        for name in names:
            if name and name[0] != '.':
                try:
                    yield self.stampType(name)
                except:
                    pass

    def getLatestStamp(self):
        # We can't just run max() on the sequence, since it might be empty
        latest = 0
        for stamp in self.iterStamps():
            latest = max(stamp, latest)
        return latest

    def cleanStamps(self, criteria):
        """Remove all stamps for which criteria(stamp)==True"""
        dirEmpty = True
        for s in self.iterStamps():
            if criteria(s):
                os.unlink(self.getFile(s))
            else:
                dirEmpty = False
        try:
            os.rmdir(self.path)
        except OSError:
            pass

    def newVersion(self, timeout=60*10):
        """To start writing out a new version of this resource, this
           returns a suitable temporary file name to use. It has the side
           effect of clearing out any expired temporary files. The new file
           will expire after the given number of seconds.
           """
        try:
            os.makedirs(self.path)
        except OSError:
            pass

        tempDir = os.path.join(getTempDir(), "unfinished")
        try:
            os.makedirs(tempDir)
        except OSError:
            pass

        # File names are an expiration time and a random identifier,
        # separated by an underscore. Look for expired files.
        now = time.time()
        for filename in os.listdir(tempDir):
            filepath = os.path.join(tempDir, filename)
            expiration, randomness = filename.split("_")
            if float(expiration) < now:
                # Expired, delete it
                os.unlink(filepath)

        # Generate a new name
        randomness = abs(hash(random.random()) ^ hash(self.name) ^ os.getpid())
        return os.path.join(tempDir, "%s_%s" % (now + timeout, randomness))

    def commitVersion(self, tempFile, stamp):
        """Commit a finished temporary file by renaming it atomically"""
        os.rename(tempFile, self.getFile(stamp))

    def rollbackVersion(self, tempFile):
        """Cancel a version without committing it to a stamp"""
        try:
            os.unlink(tempFile)
        except OSError:
            pass


class CachedResourceProxy(CachedResource):
    """A class that isn't a resource by itself but should refer to an external one"""
    def __init__(self, original):
        self.original = original
        for name in "name getFile stampExists iterStamps cleanStamps".split():
            setattr(self, name, getattr(original, name))


class DependentResource(CachedResource):
    """A CachedResource that is automatically updated to track the latest
       version of one or more other resources. A provided specification list
       can contain CachedResource instances that this depends on, plus strings
       that identify this dependent resource uniquely. This requires that all
       dependencies use the same format for stamps and that a new stamp for
       any dependency will be greater than existing stamps for all other
       dependencies.
       """
    def __init__(self, *spec):
        self.spec = spec

        elements = []
        for item in spec:
            if isinstance(item, CachedResource):
                elements.append(item.name)
            else:
                elements.append(str(item))
        CachedResource.__init__(self, ".".join(elements))

    def updateToLatest(self):
        """Commit a new stamp to this resource generated from the latest versions of
           all other resources, then delete any old stamps for this resource.
           """
        # Determine the newest stamp in each dependency
        latest = {}
        for item in self.spec:
            if isinstance(item, CachedResource):
                latest[item] = item.getLatestStamp()
        if not latest:
            return

        # Our new stamp is the greatest out of all deps' stamps
        stamp = max(latest.itervalues())
        self.updateStamp(latest, stamp)

        # Clean old versions if that was successful
        self.cleanStamps(lambda s: s < stamp)

    def updateStamp(self, depVersions, stamp):
        """Regenerate a single stamp from the original, using the provided dependency versions"""
        temp = self.newVersion()
        try:
            self.build(depVersions, temp)
        except:
            self.rollbackVersion(temp)
            raise
        self.commitVersion(temp, stamp)

    def build(self, depVersions, output):
        """A subclass-defined function to rebuild this resource given
           dependency versions and an output file path.
           """
        raise NotImplementedError()


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


class ScaledImageResource(DependentResource):
    """Rescale the input image. If only width or height are provided,
       the resulting image has the same aspect ratio as the input image.
       """
    def __init__(self, original, width=None, height=None):
        assert not (width is None and height is None)
        self.scaleTo = (width, height)
        DependentResource.__init__(self, original, "scale-%sx%s" % self.scaleTo)

    def build(self, depVersions, output):
        original = self.spec[0]
        i = Image.open(original.getFile(depVersions[original]))

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
        bg.save(output, 'PNG')


class RrdGraph(DependentResource):
    """A graph created by rrdtool from one or more RRDs. or other dependencies.
       Parameters should be plain strings for rrdtool, or resources that can be
       converted to strings using their rrdparam(stamp) method, such as RrdDef objects.
       """
    def __init__(self, params, size=(600,150), yLabel=None, interval="day"):
        self.params = [
            "--width", size[0],
            "--height", size[1],
            "--start", -units.time.lookupName(interval),
            ]
        if yLabel:
            self.params.extend(["--vertical-label", yLabel])
        self.params.extend(params)

        # Putting all rrdtool params in the spec would be prohibitively long
        # for a filename. We have to include the resoruces themselves. For readability
        # we also include the size and interval, and for uniqueness we include an md5
        # digest of all string parameters.
        resources = []
        digest = md5.new()
        for param in params:
            if isinstance(param, CachedResource):
                resources.append(param)
            else:
                digest.update(str(param))
        DependentResource.__init__(self, "%dx%d" % size, interval,
                                   digest.hexdigest(), *resources)

    def build(self, depVersions, output):
        params = []
        for param in self.params:
            if isinstance(param, CachedResource):
                params.append(param.rrdparam(depVersions[param]))
            else:
                params.append(param)
        self.size = rrd('graph', output, "-a", "PNG", *params)


class RrdDef(CachedResourceProxy):
    """A DEF parameter for rrdtool, linking an RrdFile with a particular variable."""
    def __init__(self, varName, rrd, cf='AVERAGE', dsName='x'):
        self.varName = varName
        self.cf = cf
        self.dsName = dsName
        CachedResourceProxy.__init__(self, rrd)

    def rrdparam(self, stamp):
        return "DEF:%s=%s:%s:%s" % (self.varName,
                                    self.original.getFile(stamp),
                                    self.dsName,
                                    self.cf)


class RrdFile(CachedResource):
    """An RRD file, usable as a dependency to RrdDef"""
    def update(self, source):
        """Look for updates and commit a new stamp if necessary. 'source'
           is a function that returns a (stamp, time, value) tuple for every
           stamp after the one provided. Returns the latest stamp value.
           """
        temp = self.newVersion()
        try:

            # Find the most recent stamp, for a starting point
            latestStamp = self.getLatestStamp()
            if latestStamp > 0:
                # We have an existing stamp to copy
                shutil.copyfile(self.getFile(latestStamp), temp)
            else:
                # Start a blank RRD
                self.rrdInit(temp)
            assert os.path.isfile(temp)

            # Look for new samples to insert after our current latest.
            for stamp, time, value in source(latestStamp):
                if stamp <= latestStamp:
                    continue
                try:
                    rrd("update", temp, "%d:%s" % (int(time), value))
                except RRDException:
                    # Ignore errors caused by two updates within a second
                    if str(sys.exc_info()[1]).find("illegal attempt to update") < 0:
                        raise

                latestStamp = stamp

        except:
            self.rollbackVersion(temp)
            raise

        # Commit the updated RRD
        self.commitVersion(temp, latestStamp)

        # Clean old versions if that was successful
        self.cleanStamps(lambda s: s < latestStamp)
        return latestStamp

    def rrdInit(self, filename, stepSize=60):
        """Create a blank RRD for this therm"""
        # Define the steps and rows for each RRA in each CF (see the rrdcreate manpage)
        nSamples = 60*60*24*2 / stepSize
        rraList = [
            (1,   nSamples),   # Store every sample for the last 2 days
            (7,   nSamples),   # Every 7th sample for the last 2 weeks
            (31,  nSamples),   # Every 31st sample for the last 2 months
            (365, nSamples),   # Every 365th sample for the last 2 years
            ]

        # Start out with the parameters to define our RRD file and data source
        params = ["create", filename,
                  "--step", stepSize,
                  "--start", int(time.time() - 60*60*24*365*2),
                  "DS:x:GAUGE:%s:U:U" % int(stepSize * 10)]

        # Define RRAs with all the combining functions we're interested in
        for cf in ("AVERAGE", "MIN", "MAX"):
            for steps, rows in rraList:
                params.append("RRA:%s:0.5:%s:%s" % (cf, steps, rows))

        rrd(*params)


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

def graphColorRange(min, minColor, max, maxColor, numSlices=96, id=[0]):
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
