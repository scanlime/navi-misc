#
# pyhole, the Python Keyhole-like app for your Symbian Series 60 phone.
# Copyright (C) 2005 Micah Dowty <micah@navi.cx>
#
#   This program is free software; you can redistribute it and/or modify
#   it under the terms of the GNU General Public License as published by
#   the Free Software Foundation; either version 2 of the License, or
#   (at your option) any later version.
#
#   This program is distributed in the hope that it will be useful,
#   but WITHOUT ANY WARRANTY; without even the implied warranty of
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#   GNU General Public License for more details.
#
#   You should have received a copy of the GNU General Public License
#   along with this program; if not, write to the Free Software
#   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
#

import os, math, socket, time

# These are Symbian-specific modules. Go ahead and let this
# module import without them, in case another app wants to run
# this module headless.
try:
    import appuifw, key_codes, e32, graphics
except ImportError:
    pass


class HttpPipe:
    """Pyhole was originally written to urllib2, but I've had to
       rewrite its networking code at lower and lower levels in
       order to squeeze the performance I want out of a high-latency
       connection.

       This is a fairly specialized connection that performs HTTP 1.1
       GET requests using 'identity' content-encodings. It pipelines
       requests, so that we can keep at least one GET in-flight
       at all times. On a cell phone link, the high latency will kill
       our throughput without pipelining.

       We implement proxy support, but only via CONNECT requests.
       This is done to improve latency, but also to improve compatibility
       with some proxies that perform inefficiently in our particular
       use case.

       Both the server address and the proxy must be (host, port) tuples.
       """
    def __init__(self, addr, proxy=None):
        self.addr = addr
        self.sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.pending = []
        self.buffer = ''

        if proxy:
            self.sock.connect(proxy)
            self.sock.sendall("CONNECT %s:%d\r\n\r\n" % addr)
            self.pending.append(HttpPipeRequest())
        else:
            self.sock.connect(addr)

    def submit(self, path, handler):
        """Send out a request for the provided path. When it becomes
           this request's turn to read a response, the handler is called
           with a dictionary of response headers.
           """
        self.sock.sendall("GET %s HTTP/1.1\r\n"
                          "Host: %s:%d\r\n"
                          "Accept-Encoding: identity\r\n"
                          "\r\n" % ((path,) + self.addr))
        self.pending.append(handler)

    def flush(self, blocksize=4096):
        """Handle all received data"""
        while self.pending:
            block = self.sock.recv(blocksize)
            if not block:
                break
            self.buffer += block

            while self.pending:
                self.buffer = self.pending[0].received(self.buffer)
                if self.pending[0].completed:
                    del self.pending[0]
                else:
                    break


class HttpPipeRequest:
    """Handles one request for an HttpPipe. This class by itself can
       receive headers, but it performs a no-op with any content received.
       Subclasses can define a way of storing content.
       """
    completed = False
    hasHeader = False
    length = None
    bytesReceived = 0
    bytesRemaining = None
    statusCode = None
    statusMessage = None
    headers = None
    
    def received(self, buffer):
        """After every received packet, this is called with the new
           buffer contents. Returns any unprocessed data, to be
           retained in the buffer.
           """
        if not self.hasHeader:
            try:
                header, buffer = buffer.split("\r\n\r\n", 1)
            except ValueError:
                # Haven't received the whole header
                return buffer

            # Decode the header a bit
            lines = header.split("\r\n")
            protocol, code, self.statusMessage = lines[0].split(" ", 2)
            self.statusCode = int(code)
            self.headers = {}
            while len(lines) > 1:
                k, v = lines.pop().split(": ")
                self.headers[k.lower()] = v
            self.hasHeader = True

            # If it was a success, start receiving data. If not, we're done
            if self.statusCode != 200:
                self.completed = True
                self.receivedError()
                return buffer

            self.length = int(self.headers.get("content-length", 0))
            self.bytesRemaining = self.length
            self.receivedHeaders()

        if self.bytesRemaining > 0:
            data = buffer[:self.bytesRemaining]
            buffer = buffer[self.bytesRemaining:]
            self.bytesReceived += len(data)
            self.bytesRemaining -= len(data)
            self.receivedData(data)

        if self.bytesRemaining < 1:
            self.receiveCompleted()
            self.completed = True
        return buffer

    def receivedError(self):
        """An HTTP error was received, this request is now completed"""
        pass

    def receivedHeaders(self):
        """The headers have been completely received"""
        pass

    def receivedData(self, block):
        """One block of data has just been successfully received"""
        pass

    def receiveCompleted(self):
        """The request has been completed successfully"""
        pass


class HttpError(Exception):
    pass


class RequiredHttpRequest(HttpPipeRequest):
    """A request that raises an exception on error, killing the pipe."""
    def receivedError(self):
        raise HttpError("%d %s" % (self.statusCode, self.statusMessage))


class DownloadHttpRequest(RequiredHttpRequest):
    """Stores all received data in a local file,
       updates the UI's progress indicator.
       """
    def __init__(self, path, progressCb, completedCb):
        self.file = open(path, 'wb')
        self.progressCb = progressCb
        self.completedCb = completedCb

    def receivedHeaders(self):
        self.progressCb(0)

    def receivedData(self, block):
        self.file.write(block)
        self.progressCb(self.bytesReceived / float(self.length))

    def receiveCompleted(self):
        self.file.close()
        del self.file
        self.completedCb()
        del self.progressCb
        del self.completedCb


class TileSource:
    """Abstract base class representing a backend that
       represents a way of converting a tile location (TL)
       into a URL. A 'tile location' is an (x, y, zoom) tuple
       identifying a particular location on the map in a
       fairly uniform way.

       A 'tile id' (TID) is a way of identifying a particular
       tile image. It consists of a (source, tl) tuple,

       We treat the result of getURL like a real URL, but
       in fact to eliminate redundant URL encoding/decoding
       we represent it as a ((host, port), path) tuple
       and assume an HTTP server. If there's a reason to
       in the future, it would be trivial to use real URLs
       here instead.
       """
    name = None
    fileExt = '.png'

    def getURL(self, tl):
        raise NotImplementedError


class GoogleMapSource(TileSource):
    """Retrieves normal map tiles from Google"""
    name = 'gmap'

    def getURL(self, tl):
        return (('mt.google.com', 80), '/mt?v=w2.5&x=%d&y=%d&zoom=%d' % (
            tl[0], tl[1], 17 - tl[2]))


class GoogleSatelliteSource(TileSource):
    """Retrieves satellite imagery tiles from Google.
       The satellite server, instead of tile numbers, uses a quadtree
       encoding. An empty string returns the entire world as one tile.
       Each character appended to that string zooms in to one of four
       quadrants on that tile. The length of this string, then, is
       equal to our canonical zoom level and the characters are formed
       according to the bits in our tile numbers.
       """
    name = 'gsat'
    fileExt = '.jpg'

    def getURL(self, tl):
        x, y, zoom = tl
        tree = []
        if zoom > 0:
            i = zoom
            mask = 1 << (zoom-1)
            while i:
                if x & mask:
                    if y & mask:
                        tree.append('s')
                    else:
                        tree.append('r')
                else:
                    if y & mask:
                        tree.append('t')
                    else:
                        tree.append('q')
                i -= 1
                mask = mask >> 1
        return (('kh.google.com', 80), '/kh?v=3&t=t' + ''.join(tree))


class Viewport:
    """Our canonical representation of the current viewport is a
       set of (x,y) coordinates for the center of the screen,
       and a zoom level. This set of coordinates encodes the tile
       number plus the pixel within that tile.

       In Google maps, we have 256x256 tiles and 17 zoom levels,
       so this all fits in 25 bits. The zoom level is also
       normalized to indicate the number of subdivisions applied
       to a single tile containing the entire world.

       Tiles themselves are represented everywhere using a
       (tx, ty, zoom) tuple.
       """
    maxZoom = 17
    logTileSize = 8
    tileSize = 1 << logTileSize
    tileMask = tileSize - 1

    def __init__(self, size):
        self.size = size
        self.width = size[0]
        self.height = size[1]
        self.halfw = int(self.width / 2)
        self.halfh = int(self.height / 2)
        self.reset()

    def reset(self):
        """Set up a default view of the whole world"""
        self.center(0, 0, 0)

    def move(self, x=0, y=0, zoom=0):
        """Move the viewport by the provided delta in pixels.
           The zoom level is positive to zoom in, negative to zoom
           out, or zero to leave the zoom level alone.
           """
        # Compute the mask used to wrap pixel coords
        pixelMask = (1 << (self.logTileSize + self.zoom)) - 1
        self.x = (self.x + x) & pixelMask
        self.y = (self.y + y) & pixelMask

        # Update zoom, and scale pixel coordinates by
        # the appropriate power of two
        newZoom = max(0, min(self.maxZoom, self.zoom + zoom))
        delta = newZoom - self.zoom
        self.zoom = newZoom
        if delta > 0:
            s = 1 << delta
            self.x *= s
            self.y *= s
        elif delta < 0:
            s = 1 << (-delta)
            self.x = int(self.x / s)
            self.y = int(self.y / s)

    def center(self, lat, long, zoom=None):
        if zoom is not None:
            self.zoom = zoom
        self.x, self.y = self._project(lat, long)

    def _project(self, lat, long):
        """Project the given lat/long into tile coordinates,
           in pixels. All our tile sources are assumed to use
           a Mercator projection.
           """
        x = (long + 180.0) / 360.0

        s = math.sin(lat * (math.pi / 180))
        s = max(-0.9999, min(0.9999, s))
        y = 0.5 - math.log((1+s) / (1-s)) / (4 * math.pi)

        # Scale from floats in the range [0, 1] into pixels
        res = 1 << (self.logTileSize + self.zoom)
        return int(x * res), int(y * res)

    def foreachTile(self, cb):
        """Iterates over all tile location that should be
           visible through this viewport. For each,
           it invokes cb(xy, tl) where 'xy' is the screen
           coordinate of the tile's top-left corner, and tl
           is a 'tile location' tuple as defined by TileSource.
           """
        # Calculate, in tile pixel coordinates, the
        # top left corner of the screen.
        tlx = self.x - self.halfw
        tly = self.y - self.halfh

        # Compute the mask used to wrap tile coordinates (tile number mask)
        tnm = (1 << self.zoom) - 1

        # Split up the tile pixel coordinates into
        # a tile coordinate, and a pixel offset telling
        # us how far past the edge of the screen to start.
        sy = -(tly & self.tileMask)
        ty = tly >> self.logTileSize
        while sy < self.height:
            sx = -(tlx & self.tileMask)
            tx = tlx >> self.logTileSize
            while sx < self.width:
                cb((sx, sy), (tx & tnm, ty & tnm, self.zoom))
                sx += self.tileSize
                tx += 1
            sy += self.tileSize
            ty += 1


class MapRenderer:
    """This object owns a Viewport and an offscreen Image, rendering
       tiles from a MemoryCache into that Image.
       """
    def __init__(self, size, cache, source):
        self.view = Viewport(size)
        self.image = graphics.Image.new(size)
        self.cache = cache
        self.source = source

    def draw(self):
        self.view.foreachTile(self.drawTile)

    def drawTile(self, loc, tl):
        """Draw a single tile identified by the tile location 'tl'
           at the (x, y) coordinates 'loc'. This looks up a tile from
           the cache. If it isn't immediately available, the cache
           will work in the background on processing it but for now
           we just draw a placeholder rectangle.
           """
        # Common case- the tile is in cache, blit it and get out
        img = self.cache.get((self.source, tl))
        if img:
            self.image.blit(img, target=loc)
            return

        # All the other rendering methods we have need an explicit
        # target rectangle, not just the top-left coordinates.
        ts = self.view.tileSize
        logTs = self.view.logTileSize
        target = loc + (loc[0] + ts, loc[1] + ts)

        # When we're zooming into a new area, make the map more
        # responsive by immediately showing a scaled version of
        # the previous zoom level. This keeps hopping upward
        # in our tile quadtree until it reaches something in our
        # cache, then scales that tile.
        x, y, z = tl

        # Delta zoom level, increases as we try higher magnifications
        dz = 0

        # Keep magnifying until we either run out of zoom levels,
        # or until a tile would be an area of less than a pixel
        # on the magnified image
        while z > 0 and dz < logTs:
            z -= 1
            dz += 1
            img = self.cache.get((self.source, (x >> dz, y >> dz, z)))
            if img:

                # Zoomed tile size, the size of the area on the source tile
                # that we're magnifying from.
                zts = ts >> dz

                # Our X and Y coordinates on the zoomed tile are made
                # from the portion of X and Y that we chopped off when
                # doing the tile lookup, but magnified by the zoomed
                # tile size.
                zmask = (1 << dz) - 1
                logZts = lts - dz
                zx = (x & zmask) << logZts
                zy = (y & zmask) << logZts
                self.image.blit(img, target=loc, scale=1, source=(
                    zx, zy, zx + zts, zy + zts))
                return

        # If all else fails, an ugly grey placeholder rectangle
        self.image.rectangle(target, fill=0xAAAAAA, outline=0xAA8888)


class MemoryCache:
    """A fixed size asynchronous RAM cache for tiles. This stores
       a fixed number of Image objects, each keyed to a TID tuple.

       On a cache hit, we just have to look up the tile and update
       its stamp in our MRU hash. On a cache miss, we immediately
       recycle the oldest entry in the cache but the new item is
       loaded by a background thread.
       """
    def __init__(self, size=16):
        # Our MRU list is maintained as a dictionary,
        # with an increasing 'stamp' value that indicates
        # freshness. We need updating freshness to be as
        # fast as possible, whereas recycling old tiles
        # happens less often.
        self.stamp = 1
        self.mru = {}

        # Two hashes mapping TID to images, one for complete
        # images and one that we're waiting on the backend for.
        # Note that TIDs from both these hashes are sorted
        # using the MRU hash.
        self.complete = {}
        self.pending = {}

        # Prime the cache by inserting 'size' objects
        # with unique keys that aren't valid TIDs. These
        # fake items have stamps of zero, so they will hang out
        # at the very bottom of the MRU list until gone.
        tileSize = (Viewport.tileSize, Viewport.tileSize) 
        for i in xrange(size):
            self.mru[i] = 0
            self.complete[i] = graphics.Image.new(tileSize)

    def get(self, tid):
        # Touch this TID, whether it's already in 'complete',
        # already in 'pending', or about to be added to 'pending'.
        self.stamp += 1
        self.mru[tid] = self.stamp

        # Fast path: returning an already-complete TID
        try:
            return self.complete[tid]
        except KeyError:
            pass

        # Almost-as-fast path: bailing out when we're already
        # working on loading this TID
        if tid in self.pending:
            return

        # Search for an old image to recycle.
        oldStamp = self.stamp
        for t, s in self.mru.items():
            if s < oldStamp:
                oldStamp = s
                oldTid = t
        del self.mru[oldTid]

        # Hopefully this recycled image is complete already,
        # but we might have actually got another pending image.
        if oldTid in self.complete:
            img = self.complete[oldTid]
            del self.complete[oldTid]
        else:
            img = self.pending[oldTid]
            del self.pending[oldTid]
        self.pending[tid] = img


class KeyboardState(dict):
    """Handles key up/down events, tracking the state of each scancode"""
    def __init__(self, timer):
        self.timer = timer

    def __getitem__(self, scan):
        return self.get(scan, False)

    def track(self, event):
        if event['type'] == appuifw.EEventKeyDown:
            self[event['scancode']] = True
            self.timer.signal()
        elif event['type'] == appuifw.EEventKeyUp:
            self[event['scancode']] = False
            self.timer.signal()

    def getDirection(self):
        """Decode the current state of the direction pad into an x,y tuple"""
        x = y = 0
        if self[key_codes.EScancodeLeftArrow]:   x -= 1
        if self[key_codes.EScancodeRightArrow]:  x += 1
        if self[key_codes.EScancodeUpArrow]:     y -= 1
        if self[key_codes.EScancodeDownArrow]:   y += 1
        return x, y


class SmoothScroller:
    """Animates the position of a Viewport smoothly according to keyboard input"""
    def __init__(self, keys, view):
        self.keys = keys
        self.view = view
        self.prevTime = None

        self.maxVelocity = 1500.0
        self.keyAccel = 200.0
        self.damping = 0.9

        # Current velocity
        self.vx = 0
        self.vy = 0

        # Subpixel remainders
        self.rx = 0
        self.ry = 0

    def update(self, timer):
        # Update speeds according to keyboard state
        kx, ky = self.keys.getDirection()
        self.vx = self.vx * self.damping + kx * self.keyAccel * timer.dt
        self.vy = self.vy * self.damping + ky * self.keyAccel * timer.dt

        if kx or ky or abs(self.vx) > 0.1 or abs(self.vy) > 0.1:
            timer.signal()
            
        # Move the viewport according to our speed,
        # saving any subpixel remainders.
        x = self.vx * timer.dt + self.rx
        y = self.vy * timer.dt + self.ry
        ix = int(x + 0.5)
        iy = int(y + 0.5)
        self.view.move(ix, iy)
        self.rx = x - ix
        self.ry = y - iy


class Timekeeper:
    """Track the current time, and handles putting the UI
       thread to sleep when it's no longer needed.
       """
    _prevTime = None
    dt = 0
    ticks = 0

    def __init__(self):
        self._lock = e32.Ao_lock()
        self.signal()

    def signal(self):
        """Indicate that we need to render another frame"""
        self._signaled = True
        self._lock.signal()

    def tick(self):
        """Begin a new frame. May sleep until signaled, if
           we haven't already been signaled.
           """
        e32.ao_yield()
        while not self._signaled:
            self._prevTime = None
            self._lock.wait()

        self._signaled = False
        now = time.clock()
        if self._prevTime is None:
            self._prevTime = now
            self.dt = 0.0
        else:
            self.dt = now - self._prevTime
            self._prevTime = now
        self.ticks += 1


class InterfaceLayout:
    """Calculates and saves rectangles that depend on the UI's size"""
    def __init__(self, w, h):
        self.all = (0, 0, w, h)

        textHeight = 10
        margin = 2
        progressWidth = 32
        statusHeight = textHeight + margin * 2

        # The two main sections of the app
        self.status = (0, 0, w, statusHeight)
        self.map = (0, statusHeight, w, h - statusHeight)

        # Items in the status bar
        self.statusText = (margin, margin + textHeight)
        self.progress = lambda p: (w - progressWidth - margin*2, margin,
                                   w - progressWidth - margin*2 +
                                   int(progressWidth * p), statusHeight - margin)
        self.progressOutline = self.progress(1)


class Interface:
    """User interface frontend: glues together appuifw, the Canvas, MapRenderer,
       SmoothScroller, and such.
       """
    def __init__(self, sources, cache):
        self.sources = sources
        self.sourceIndex = 0
        self.cache = cache

        self.timer = Timekeeper()
        self.keys = KeyboardState(self.timer)
        self.canvas = appuifw.Canvas(self.expose, self.keys.track)
        self.show()
        self.renderer = MapRenderer(self.canvas.size, self.cache,
                                    self.sources[self.sourceIndex])
        self.layout = InterfaceLayout(*self.canvas.size)
        self.bindKeys()
        self.running = True

    def bindKeys(self):
        self.scroll = SmoothScroller(self.keys, self.renderer.view)
        self.canvas.bind(key_codes.EKeySelect, lambda: self.renderer.view.move(zoom=1))
        self.canvas.bind(key_codes.EKey2, lambda: self.renderer.view.move(zoom=-1))
        self.canvas.bind(key_codes.EKey5, self.toggleSource)

    def show(self):
        """Attach this UI to the Application"""
        appuifw.app.screen = 'full'
        appuifw.app.exit_key_handler = self.quit
        appuifw.app.body = self.canvas

    def quit(self):
        self.running = False
        self.timer.signal()

    def toggleSource(self):
        """A user command to toggle the data source for this view"""
        self.sourceIndex = (self.sourceIndex + 1) % len(self.sources)
        self.renderer.source = self.sources[self.sourceIndex]

    def expose(self, rect):
        """The Canvas asked us to redraw"""
        # FIXME: poke the render loop
        pass

    def run(self):
        while self.running:
            self.timer.tick()
            self.scroll.update(self.timer)
            self.renderer.draw()

            self.renderer.image.text(self.layout.statusText, u'tick: %d pending: %d' %
                                     (self.timer.ticks, len(self.cache.pending)))

            self.canvas.blit(self.renderer.image)


if __name__ == "__main__":
    sources = (GoogleMapSource(), GoogleSatelliteSource())
    cache = MemoryCache()

    ui = Interface(sources, cache)
    ui.run()


# class DiskCache:
#     """Manages our on-disk tile cache. This is responsible for
#        our file naming policy, and for loading existing images
#        off disk. When we encounter an image that hasn't been
#        downloaded yet, this asks the TileFetcher to start
#        downloading it and returns a placeholder.

#        FIXME: The disk cache does not yet have an eviction policy.
#        """
#     def __init__(self, fetcher, path="E:\\system\\data\\maps"):
#         self.fetcher = fetcher
#         self.root = path

#     def get(self, (source, tile), memCache=None):
#         """Request a tile from the cache. Returns a TileImage
#            if it's available now, otherwise returns a TilePlaceholder
#            and requests the tile from our fetcher.
#            """
#         path = self.getPath(source, tile)
#         if os.path.isfile(path):
#             try:
#                 return TileImage(path)
#             except SymbianError:
#                 # An invalid image. Retry the download
#                 pass

#         self.fetcher.request(source.getURL(tile), path)

#         # If we can, cheat a bit and use a scaled version of the
#         # parent tile as a placeholder.
#         if memCache:
#             try:
#                 x = tile[0] & 1
#                 y = tile[1] & 1
#                 parent = memCache.mru[(source, (tile[0] >> 1, tile[1] >> 1, tile[2] - 1))]
#             except KeyError:
#                 pass
#             else:
#                 return ZoomedTilePlaceholder(parent, x, y)
#         return EmptyTilePlaceholder()

#     def getPath(self, source, tile):
#         """Get an appropriate file name for a particular tile
#            and source. This has the side effect of creating
#            directories as necessary.
#            """
#         x, y, zoom = tile

#         # Always start out with the source name then the zoom level
#         levels = [source.name, "z%d" % zoom]
        
#         # To keep directories at a reasonable size and improve
#         # spatial locality, we include only 4 bits of each axis
#         # in a particular directory tree level. This gives us
#         # a maximum of 256 files per directory, and 5 nested
#         # directories. It also means we can encode each directory
#         # name in hex with only 2 digits.
#         shift = (zoom + 3) & ~3
#         while shift > 0:
#             shift -= 4
#             levels.append("%X%X" % (
#                 (x >> shift) & 0xF,
#                 (y >> shift) & 0xF))

#         # Assemble the path, creating directories as necessary
#         filename = levels.pop() + source.fileExt
#         path = self.root
#         if not os.path.isdir(path):
#             os.mkdir(path)
#         for l in levels:
#             path = os.path.join(path, l)
#             if not os.path.isdir(path):
#                 os.mkdir(path)
#         return os.path.join(path, filename)


# class TileFetcher:
#     """The tile fetcher runs in the main thread while no
#        UI callbacks are active, and downloads individual
#        tiles over GPRS.
#        """
#     def __init__(self):
#         # FIXME: Load proxy settings dynamically
#         self.proxy = ('flapjack.navi.cx', 143)
        
#         self.pipes = {}
#         self.mru = MRUlist()
#         self.pendingUrls = {}

#     def request(self, url, path):
#         """Request to download the provided URL.
#            If it's already been requested, this bumps
#            the priority of that URL. Note that currently
#            we assume HTTP, and the URL is actually encoded
#            as a ((host, port), path) tuple to avoid redundant
#            URL encoding and decoding.
#            """
#         if url not in self.pendingUrls:
#             self.mru.push(url, path)

#     def poll(self):
#         for pipe in self.pipes.itervalues():
#             pipe.flush()
#         self.trySubmit()

#     def trySubmit(self):
#         """Examine the next request to execute,
#            in newest-first order. This ensures we
#            focus our attention on what the user is
#            actively browsing, but if we're otherwise idle
#            we can pick up tiles we skipped earlier.
#            """
#         while len(self.mru):
#             url, localPath = self.mru.newest()
#             addr, remotePath = url

#             # Create a pipe if necessary
#             try:
#                 pipe = self.pipes[addr]
#             except KeyError:
#                 pipe = HttpPipe(addr, self.proxy)
#                 self.pipes[addr] = pipe

#             # Only submit if the pipe is about to stall
#             if len(pipe.pending) > 2:
#                 break

#             # Transfer from the MRU list to the pending URL hash
#             self.pendingUrls[url] = True
#             self.mru.remove(url)

#             pipe.submit(remotePath, DownloadHttpRequest(
#                 localPath, self.progress, lambda url=url: self.completed(url)))

#     def completed(self, url):
#         """A URL has completed. Notify the UI, and
#            remove it from our pending list.
#            """
#         del self.pendingUrls[url]
#         self.ui.downloadProgress = None
#         self.ui.needsRedraw()

#         # See if we can go ahead and repopulate the HTTP pipe
#         # without waiting for the next poll cycle to come around.
#         self.trySubmit()

#     def progress(self, p):
#         """Update the UI's progress bar"""
#         self.ui.downloadProgress = p
#         self.ui.needsStatusUpdate()

#     def run(self, ui):
#         self.ui = ui
#         while ui.running:
#             self.poll()
#             e32.ao_sleep(0.1)


# if __name__ == "__main__":
#     fetcher = TileFetcher()
#     ui = SymbianUI((GoogleMapSource(), GoogleSatelliteSource()),
#                    MemoryCache(DiskCache(fetcher)))
#     fetcher.run(ui)
