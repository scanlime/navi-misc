#!/usr/bin/env python
#
# Super-experimental library for manipulating google maps
# data and generating static high-res map output.
# Copyright (C) 2005 Micah Dowty
#

import urllib, Image
from reportlab.pdfgen import canvas
from reportlab.lib import pagesizes, units
from cStringIO import StringIO


class KeyholeTile:
    """Represents one tile of satelite imagery, downloadable from Google's Keyhole server.

       Google's keyhole tile names are encoded with what appears to
       be a quadtree. Tiles always start with 't', then are followed
       by a variable-length sequence of letters (depending on the zoom)
       that refine the position by selecting one of four quadrants:

             +Y
          +---+---+
          | t | s |
       -X +---+---+ +X
          | q | r |
          +---+---+
             -Y

       Here we represent tile positions by converting these quadtree
       traversal instructions into one bit each of X and Y data, and
       representing the tiles with zoom, x, and y numbers.

       The default tile is at (0,0), represented by only the string 't'.
       This includes methods to get and set the tile position in different
       formats, to get neighbouring tiles, and to zoom in on a tile.
       We can also generate URLs and download a tile's contents as a PIL image.

       A note about coordinates and zooms: The default tile (zoom level 1)
       represents the entire earth in a single tile. We follow the lat/long
       sign convention where north and east are positive. The default tile
       has +180 latitude at the top, -180 latitude at the bottom, -180
       longitude on the left, and +180 longitude on the right.
       """
    def __init__(self, zoom=1, x=0, y=0, size=256):
        self.zoom = zoom
        self.x = x
        self.y = y
        self.size = size

    def getScale(self):
        """Return this tile's scale in degrees per pixel"""
        # Note that at the lowest zoom level (1) we cover a full 360
        # degrees in each axis. Every zoom level is a power of two.
        return (360.0 / self.size) / (1 << (self.zoom - 1))

    def getLatLong(self, point=(0,0)):
        """Return a (lat, long) tuple for this tile. By default
           this returns the top-left corner, but a reference point
           in pixels may be supplied.
           """
        scale = self.getScale()
        return (180 - scale * (self.size * self.y + point[0]),
                -180 + scale * (self.size * self.x + point[1]))

    def setLatLong(self, lat, long, zoom=None):
        """Set the tile's latitude/longitude, and optionally the zoom level.
           This returns a tuple indicating the pixel coordinates within this
           tile that refer exactly to the given lat/long.
           """
        if zoom is not None:
            self.zoom = zoom

        # First calculate the absolute coordinates from (180,-180) in pixels
        scale = self.getScale()
        x = (long + 180) / scale
        y = (180 - lat) / scale

        # Divide that into tiles, return the remainder
        self.x = int(x // self.size)
        self.y = int(y // self.size)
        return (x % self.size, y % self.size)

    def setEncodedName(self, tile):
        """Set this tile's information from an encoded name string"""
        x = 0
        y = 0
        for c in tile[1:]:
            x <<= 1
            y <<= 1
            if c in ('t', 's'):
                y |= 1
            if c in ('s', 'r'):
                x |= 1
        self.zoom = len(tile)
        self.x = x
        self.y = y

    def getEncodedName(self):
        """Return an encoded string representing this tile"""
        result = ""
        x = self.x
        y = self.y
        assert self.zoom >= 1
        for i in xrange(self.zoom-1):
            char = {
                (0,0): 'q',
                (1,0): 'r',
                (0,1): 't',
                (1,1): 's',
                }[ x&1, y&1 ]
            result = char + result
            x >>= 1
            y >>= 1
        return 't' + result

    def getURI(self):
        """Get the URI at which this tile can be downloaded from google"""
        return 'http://kh.google.com/kh?v=1&t=' + self.getEncodedName()

    def getImage(self):
        """Download this tile and return a PIL image"""
        data = urllib.urlopen(self.getURI()).read()
        return Image.open(StringIO(data))

    def translate(self, x=0, y=0):
        """Return a new tile relative to this tile's position"""
        return self.__class__(self.zoom, self.x+x, self.y+y)

    def zoom(self, z):
        """Zoom in (z>0) or out (z<0). When zooming in, the returned
           tiles are aligned on their top-left corner.
           """
        if z < 0:
            x = self.x / (1 << -z)
            y = self.y / (1 << -z)
        else:
            x = self.x * (1 << z)
            y = self.y * (1 << z)
        return self.__class__(self.zoom + z, self.x+x, self.y+y)


def drawMap(canvas, center, pageOrigin=(0,0), pageSize=pagesizes.letter, zoom=18, dpi=300):
    outputTileSize = units.inch * center.size / dpi

    # Calculate the size of the page in tiles, rounding up
    tileGrid = (int(pageSize[0] / outputTileSize + 1),
                int(pageSize[1] / outputTileSize + 1))

    for y in xrange(-tileGrid[1]/2, tileGrid[1]/2+2):
        for x in xrange(-tileGrid[0]/2, tileGrid[0]/2+2):
            tile = center.translate(x, y)
            print "Tile %d,%d" % (x,y)
            canvas.drawInlineImage(tile.getImage(),
                                   pageOrigin[0] + pageSize[0]/2 + x * outputTileSize,
                                   pageOrigin[1] + pageSize[1]/2 - y * outputTileSize,
                                   outputTileSize, outputTileSize)


if __name__ == "__main__":
    sbo = (40.00372, -105.2635)
    c = canvas.Canvas("output.pdf", pagesize=pagesizes.letter)

    origin = KeyholeTile()
    origin.zoom = 18
    origin.setLatLong(*sbo)

    drawMap(c, origin)

    c.showPage()
    c.save()

### The End ###
