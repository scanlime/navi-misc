#!/usr/bin/env python
#
# Super-experimental caching Gogle Maps proxy
# Copyright (C) 2005 Micah Dowty
#

from twisted.web import server, resource, client
from twisted.internet import reactor
import urlparse, os

def decodeKeyholeTile(tile):
    """Google's keyhole tile names are encoded with what appears to
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

       This converts one such string into a (zoom, x, y) tuple similar to
       the ones used by non-keyhole tiles by using each letter to add one
       bit of data to both the X and Y coordinates.
       """
    x = 0
    y = 0
    for c in tile[1:]:
        x <<= 1
        y <<= 1
        if c in ('t', 's'):
            y |= 1
        if c in ('s', 'r'):
            x |= 1
    return (len(tile), x, y)


class GoogleMapCache(resource.Resource):
    def getChildWithDefault(self, path, request):
        return self

    def render(self, request):
        originalURL, cacheKey = self.translateRequest(request)
        cacheFile = self.getCacheFile(cacheKey)
        print cacheFile

        if os.path.isfile(cacheFile):
            return open(cacheFile).read()
        else:
            client.getPage(originalURL).addCallback(
                self.cbGetPage, cacheFile, request)
            return server.NOT_DONE_YET

    def getCacheFile(self, (path, args)):
        # Normal map tiles
        if path == "/mt":
            return os.path.join("data", "tiles", "%s_%s_%s" %
                                (args['zoom'][0], args['x'][0], args['y'][0]))

        # Keyhole tiles
        if path == "/kh":
            zoom, x, y = decodeKeyholeTile(args['t'][0])
            return os.path.join("data", "keyhole", "%s_%s_%s" % (zoom, x, y))

        # Static data
        if not args:
            return os.path.join("data", "static", path.replace("/","_"))

        # Other cruft
        return os.path.join("data", "other", repr((path, args)).replace("/","_"))

    def cbGetPage(self, pageData, cacheFile, request):
        pageData = self.translatePage(pageData)
        open(cacheFile, "wb").write(pageData)
        request.write(pageData)
        request.finish()

    def translateRequest(self, request):
        (scheme, host, path, parameters,
            query, fragment) = urlparse.urlparse(request.uri)
        cacheKey = (path, request.args)
        originalHost = {
            "/mt": "mt.google.com",
            "/kh": "kh.google.com"
            }.get(path, "maps.google.com")
        originalURL = urlparse.urlunparse((
            "http", originalHost, path, parameters, query, fragment))
        return originalURL, cacheKey

    def translatePage(self, data):
        data = data.replace("maps.google.com", "10.0.0.1:8052")
        data = data.replace("mt.google.com", "10.0.0.1:8052")
        data = data.replace("kh.google.com", "10.0.0.1:8052")
        return data


if __name__ == "__main__":
    site = server.Site(GoogleMapCache())
    reactor.listenTCP(8052, site)
    reactor.run()
