#
# pyhole, the Python Keyhole-like app for your Symbian Series 60 phone.
# Copyright (C) 2005 Micah Dowty
#

import os, math, socket

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

    def __init__(self, width, height):
        self.width = width
        self.height = height
        self.halfw = int(width / 2)
        self.halfh = int(height / 2)
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
        """Iterates over all tiles that should be
           visible through this viewport. For each,
           it invokes cb(x, y, tile).
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
                cb(sx, sy, (tx & tnm, ty & tnm, self.zoom))
                sx += self.tileSize
                tx += 1
            sy += self.tileSize
            ty += 1


class TileSource:
    """Abstract base class representing a backend that
       assigns a URL to each tile. TileSource subclasses
       are responsible for converting a standard 3-tuple
       into the format that backend uses to identify
       a tile. TileSources also have names used to identify
       them to the cache.

       We treat the result of getURL like a real URL, but
       in fact to eliminate redundant URL encoding/decoding
       we represent it as a ((host, port), path) tuple
       and assume an HTTP server. If there's a reason to
       in the future, it would be trivial to use real URLs
       here instead.
       """
    name = None
    fileExt = '.png'

    def getURL(self, tile):
        raise NotImplementedError


class GoogleMapSource(TileSource):
    """Retrieves normal map tiles from Google"""
    name = 'gmap'

    def getURL(self, tile):
        return (('mt.google.com', 80), '/mt?v=w2.5&x=%d&y=%d&zoom=%d' % (
            tile[0], tile[1], 17 - tile[2]))


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

    def getURL(self, tile):
        x, y, zoom = tile
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


class TileImage:
    """A normal tile object, abstracted a bit so we can
       manipulate real tiles and tile placeholders identically.
       """
    cacheable = True

    def __init__(self, diskpath):
        self.image = graphics.Image.open(diskpath)

    def draw(self, canvas, x, y):
        canvas.blit(self.image, target=(x,y))


class TilePlaceholder:
    """A placeholder that renders in place of tiles we
       haven't downloaded yet. The interface is
       identical to a real TileImage.
       """
    cacheable = False

    def draw(self, canvas, x, y):
        canvas.rectangle((x, y,
                          x + Viewport.tileSize,
                          y + Viewport.tileSize),
                         fill=0xAAAAAA, outline=0xAA8888)


class MRUlist:
    """A most/least recently used list"""
    def __init__(self):
        self.list = []
        self.hash = {}
        self.__getitem__ = self.hash.__getitem__
        self.__len__ = self.hash.__len__

    def bump(self, key):
        self.list.remove(key)
        self.list.append(key)

    def popNewest(self):
        del self.hash[self.list.pop()]

    def popOldest(self):
        key = self.list[0]
        del self.list[0]
        del self.hash[key]

    def newest(self):
        key = self.list[-1]
        return key, self.hash[key]

    def oldest(self):
        key = self.list[0]
        return key, self.hash[key]

    def remove(self, key):
        self.list.remove(key)
        del self.hash[key]

    def push(self, key, value, bump=True):
        """Push a new item onto the MRU list. 'bump'
           determines our behaviour when the item already
           exists- if bump is True, this puts the item
           back at the fresh end of the MRU list. If False,
           it stays where it is.
           """        
        if key in self.hash:
            if bump:
                self.list.remove(key)
            else:
                return
        self.hash[key] = value
        self.list.append(key)


class MemoryCache:
    """In-memory tile cache, limited to a specific
       number of tiles. This uses an MRU list to evict
       old tiles. 'backing' is where we look on a cache
       miss. It is any object with the same get() signature
       as this one.
       """
    def __init__(self, backing, size=16):
        self.mru = MRUlist()
        self.size = size
        self.backing = backing

    def get(self, *key):
        try:
            obj = self.mru[key]
        except KeyError:
            # Cache miss. If our MRU list is full, evict something.
            if len(self.mru) == self.size:
                self.mru.popOldest()

            obj = self.backing.get(*key)
            if obj.cacheable:
                self.mru.push(key, obj)
            return obj
        else:
            # Cache hit, keep the entry fresh
            self.mru.bump(key)
            return obj


class DiskCache:
    """Manages our on-disk tile cache. This is responsible for
       our file naming policy, and for loading existing images
       off disk. When we encounter an image that hasn't been
       downloaded yet, this asks the TileFetcher to start
       downloading it and returns a placeholder.

       FIXME: The disk cache does not yet have an eviction policy.
       """
    def __init__(self, fetcher, path="E:\\system\\data\\maps"):
        self.fetcher = fetcher
        self.root = path

    def get(self, source, tile):
        """Request a tile from the cache. Returns a TileImage
           if it's available now, otherwise returns a TilePlaceholder
           and requests the tile from our fetcher.
           """
        path = self.getPath(source, tile)
        if os.path.isfile(path):
            try:
                return TileImage(path)
            except SymbianError:
                # An invalid image. Retry the download
                pass

        self.fetcher.request(source.getURL(tile), path)
        return TilePlaceholder()

    def getPath(self, source, tile):
        """Get an appropriate file name for a particular tile
           and source. This has the side effect of creating
           directories as necessary.
           """
        x, y, zoom = tile

        # Always start out with the source name then the zoom level
        levels = [source.name, "z%d" % zoom]
        
        # To keep directories at a reasonable size and improve
        # spatial locality, we include only 4 bits of each axis
        # in a particular directory tree level. This gives us
        # a maximum of 256 files per directory, and 5 nested
        # directories. It also means we can encode each directory
        # name in hex with only 2 digits.
        shift = (zoom + 3) & ~3
        while shift > 0:
            shift -= 4
            levels.append("%X%X" % (
                (x >> shift) & 0xF,
                (y >> shift) & 0xF))

        # Assemble the path, creating directories as necessary
        filename = levels.pop() + source.fileExt
        path = self.root
        if not os.path.isdir(path):
            os.mkdir(path)
        for l in levels:
            path = os.path.join(path, l)
            if not os.path.isdir(path):
                os.mkdir(path)
        return os.path.join(path, filename)


class TileFetcher:
    """The tile fetcher runs in the main thread while no
       UI callbacks are active, and downloads individual
       tiles over GPRS.
       """
    def __init__(self):
        # FIXME: Load proxy settings dynamically
        self.proxy = ('flapjack.navi.cx', 143)
        
        self.pipes = {}
        self.mru = MRUlist()
        self.pendingUrls = {}

    def request(self, url, path):
        """Request to download the provided URL.
           If it's already been requested, this bumps
           the priority of that URL. Note that currently
           we assume HTTP, and the URL is actually encoded
           as a ((host, port), path) tuple to avoid redundant
           URL encoding and decoding.
           """
        if url not in self.pendingUrls:
            self.mru.push(url, path)

    def poll(self):
        for pipe in self.pipes.itervalues():
            pipe.flush()
        self.trySubmit()

    def trySubmit(self):
        """Examine the next request to execute,
           in newest-first order. This ensures we
           focus our attention on what the user is
           actively browsing, but if we're otherwise idle
           we can pick up tiles we skipped earlier.
           """
        while len(self.mru):
            url, localPath = self.mru.newest()
            addr, remotePath = url

            # Create a pipe if necessary
            try:
                pipe = self.pipes[addr]
            except KeyError:
                pipe = HttpPipe(addr, self.proxy)
                self.pipes[addr] = pipe

            # Only submit if the pipe is about to stall
            if len(pipe.pending) > 2:
                break

            # Transfer from the MRU list to the pending URL hash
            self.pendingUrls[url] = True
            self.mru.remove(url)

            pipe.submit(remotePath, DownloadHttpRequest(
                localPath, self.progress, lambda url=url: self.completed(url)))

    def completed(self, url):
        """A URL has completed. Notify the UI, and
           remove it from our pending list.
           """
        del self.pendingUrls[url]
        self.ui.downloadProgress = None
        self.ui.drawStatus()
        self.ui.needsRedraw()

        # See if we can go ahead and repopulate the HTTP pipe
        # without waiting for the next poll cycle to come around.
        self.trySubmit()

    def progress(self, p):
        """Update the UI's progress bar"""
        self.ui.downloadProgress = p
        self.ui.drawStatus()

    def run(self, ui):
        self.ui = ui
        while ui.running:
            self.poll()
            e32.ao_sleep(0.1)


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
                                   w - progressWidth - margin*2 + int(progressWidth * p),
                                   statusHeight - margin)

        # "busy" indicator overlaid on the bottom-right corner
        busySize = 6
        self.busy = (w - busySize - margin,
                     h - busySize - margin,
                     w - margin, h - margin)


class SymbianUI:
    """Frontend for browsing Google Maps using the Series 60 GUI."""
    def __init__(self, sources, cache):
        self.view = None
        self.sources = sources
        self.sourceIndex = 0
        self.downloadProgress = None
        self.cache = cache
        self.backbuffer = None

        self.canvas = appuifw.Canvas(self.flip)
        self.bindKeys()
        self.show()

    def bindKeys(self):
        self.canvas.bind(key_codes.EKeyRightArrow, lambda: self.scroll(1, 0))
        self.canvas.bind(key_codes.EKeyLeftArrow, lambda: self.scroll(-1, 0))
        self.canvas.bind(key_codes.EKeyUpArrow, lambda: self.scroll(0, -1))
        self.canvas.bind(key_codes.EKeyDownArrow, lambda: self.scroll(0, 1))
        self.canvas.bind(key_codes.EKeySelect, lambda: self.zoom(1))
        self.canvas.bind(key_codes.EKeyStar, lambda: self.zoom(-1))
        self.canvas.bind(key_codes.EKeyHash, self.toggleSource)

    def show(self):
        """Attach this UI to the Application"""
        appuifw.app.screen = 'full'
        appuifw.app.exit_key_handler = self.quit
        appuifw.app.body = self.canvas
        self.running = True

        # The following things depend on the canvas' size
        self.view = Viewport(*self.canvas.size)
        self.backbuffer = graphics.Image.new(self.canvas.size)
        self.layout = InterfaceLayout(*self.canvas.size)

        self.needsRedraw()

    def quit(self):
        self.running = False

    def flip(self, *rect):
        """Flip a rectangle from the backbuffer to the canvas. This
           is called by our canvas redraw handler, or when we are done
           painting an area of the backbuffer.
           """
        if self.backbuffer:
            self.canvas.blit(self.backbuffer, source=rect, target=rect)

    def setBusy(self, color):
        """Draw the 'busy' indicator, a small rectangle in the corner
           of the screen. For debugging mostly, the color indicates
           what we're waiting on.
           """
        self.canvas.rectangle(self.layout.busy, fill=color, outline=0)

    def drawAll(self):
        """Redraw and flip the entire screen. This needs to draw
           the status area in addition to the map itself, since
           the tiles will end up scribbling on top of the status
           area. In practise this isn't bad, since we are typically
           updating the status area much more frequently on purpose.
           """
        self.setBusy(0xFF0000)
        self.view.foreachTile(self.drawTile)
        self.drawStatus(flip=False)
        self.flip(self.layout.all)

    def drawTile(self, x, y, tile):
        """Retrieve and draw one tile at the given position"""
        obj = self.cache.get(self.sources[self.sourceIndex], tile)
        obj.draw(self.backbuffer, x, y)

    def drawStatus(self, flip=True):
        """Draw the status area, and by default flip it to the frontbuffer"""
        # Background and status text
        self.backbuffer.rectangle(self.layout.status, fill=0x888888)
        self.backbuffer.text(self.layout.statusText, self.getStatusText())

        # Draw the progress bar if we have one
        if self.downloadProgress is not None:
            progressOutline = self.layout.progress(1)
            self.backbuffer.rectangle(progressOutline, fill=0xFFFFFF)
            self.backbuffer.rectangle(self.layout.progress(self.downloadProgress), fill=0x000088)
            self.backbuffer.rectangle(progressOutline, outline=0)

        if flip:
            self.flip(self.layout.status)

    def getStatusText(self):
        pipes = self.cache.backing.fetcher.pipes.values()
        if pipes:
            p = len(pipes[0].pending)
        else:
            p = None

        return u'P.URLs: %s  Pipe: %s  MRU: %s' % (
            len(self.cache.backing.fetcher.pendingUrls),
            p,
            len(self.cache.backing.fetcher.mru))

    def needsRedraw(self):
        """Calling this function indicates that we need to redraw the
           map. This is called by UI callbacks, so it can happen any
           time an active object may be invoked by Symbian- even during
           a redraw. If we get called during a redraw, this schedules
           another redraw to happen immediately after this one is finished.
           """
        self.drawAll()

    def scroll(self, x, y):
        """A scroll event from the user. Moves the viewport in fixed
           increments, then redraws.
           """
        self.view.move(x*30, y*30)
        self.needsRedraw()

    def zoom(self, z):
        """A zoom in/out event from the user"""
        self.view.move(zoom=z)
        self.needsRedraw()

    def toggleSource(self):
        """A user command to toggle the data source for this view"""
        self.sourceIndex = (self.sourceIndex + 1) % len(self.sources)
        self.needsRedraw()


if __name__ == "__main__":
    fetcher = TileFetcher()
    ui = SymbianUI((GoogleMapSource(), GoogleSatelliteSource()),
                   MemoryCache(DiskCache(fetcher)))
    fetcher.run(ui)
