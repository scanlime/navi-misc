""" BZFlag.Meta.Client

Client interface for the metaserver. The metaserver is a centeral resource
for finding information about BZFlag servers. This client can retrieve
server lists and update the central server list.
"""
#
# Python BZFlag Package
# Copyright (C) 2003 Micah Dowty <micahjd@users.sourceforge.net>
#
#  This library is free software; you can redistribute it and/or
#  modify it under the terms of the GNU Lesser General Public
#  License as published by the Free Software Foundation; either
#  version 2.1 of the License, or (at your option) any later version.
#
#  This library is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
#  Lesser General Public License for more details.
#
#  You should have received a copy of the GNU Lesser General Public
#  License along with this library; if not, write to the Free Software
#  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
#

import BZFlag
from BZFlag import Network, Errors
import urlparse
from BZFlag.Meta.ServerInfo import ServerInfo


# The file at this URL contains the URL for the BZFlag metaserver
metaMetaURL = "http://bzflag.org/list-server.txt"


metaURL = None
def getMetaserverURL(eventLoop, callback):
    """Contact the meta-meta-server to find the URL for the metaserver.
       call the provided callback with the URL once we find it.
       """
    global metaMetaURL, metaURL
    if metaURL:
        callback(metaURL)
    else:
        # Stick together an HTTP request, with responseHandler called when it comes back.
        def responseHandler(response):
            lines = [line.strip() for line in response.strip().split("\n")]
            if lines[0] != "HTTP/1.1 200 OK":
                raise Errors.NetworkException("Meta-meta-server returned status '%s'" % lines[0])
            metaURL = lines[-1]
            callback(metaURL)
        parsed = urlparse.urlparse(metaMetaURL)
        Network.asyncRequest(eventLoop, parsed[1], 80,
                             "GET %s HTTP/1.1\nHost: %s\nUser-Agent: %s/%s\n\n" %
                             (parsed[2], parsed[1], BZFlag.name, BZFlag.version),
                             responseHandler)


class MetaClient:
    """Client for the BZFlag metaserver"""
    def __init__(self, eventLoop, url=None):
        self.eventLoop = eventLoop
        self.url = url

    def command(self, line, callback=None):
        """Send a generic metaserver command, giving the results
           to the supplied callback when they come in.
           """
        # This turns into two distinct asynchronous steps- looking
        # up the metaserver URL (which is cached), and actually
        # performing the request to the metaserver.

        if type(line) == type(()) or type(line) == type([]):
            line = " ".join(map(str, line))
        line += "\n\n"

        def performRequest(url):
            # Normally we'd use urlparse for this, but it has wacky defaults
            # for URL schemes it doesn't recognize.
            host = url.split("/")[2]
            Network.asyncRequest(self.eventLoop, host, 5156, line, callback)

        if self.url:
            # We're overriding the meta-meta-server's URL
            performRequest(url)
        else:
            getMetaserverURL(self.eventLoop, performRequest)

    def add(self, serverInfo, callback=None):
        import binascii
        self.command(("ADD",
                      serverInfo.name,
                      serverInfo.build,
                      serverInfo.version,
                      binascii.b2a_hex(str(serverInfo.gameinfo)),
                      serverInfo.title),
                     callback)

    def remove(self, name, callback=None):
        self.command(("REMOVE", name), callback)

    def setPlayerCounts(self, name, counts, callback=None):
        self.command(("SETNUM", name,
                      counts.get("rogue",0),
                      counts.get("red",0),
                      counts.get("green",0),
                      counts.get("blue",0),
                      counts.get("purple",0)),
                     callback)

    def list(self, callback, filterVersion=BZFlag.protocolVersion):
        """Downloads a list of all active BZFlag servers, filtered
           to the given protocol version. None will retrieve all
           servers, even if they aren't compatible with this package.
           """
        def responseParser(response):
            servers = []
            for line in response.split("\n"):
                line = line.strip()
                if line:
                    serverInfo = ServerInfo(line)
                    if serverInfo.version == filterVersion or not filterVersion:
                        servers.append(serverInfo)
            callback(servers)
        self.command("LIST", responseParser)

### The End ###
