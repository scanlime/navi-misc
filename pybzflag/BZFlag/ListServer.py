""" BZFlag.ListServer

Abstraction for the BZFlag server list server.
"""
# 
# Python BZFlag Protocol Package
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

listServerURL = None

def getListServerURL():
    """Looks up the current list server URL from bzflag.org"""
    global listServerURL
    if not listServerURL:
        import urllib2
        return urllib2.urlopen("http://bzflag.org/list-server.txt").read().strip()
    return listServerURL


class ServerInfo:
    """Represents information about one BZFlag server"""
    def __init__(self, specifier=None):
        self.name = None
        self.version = None
        self.gameinfo = None
        self.ip = None
        self.title = None
        if specifier:
            from BZFlag.Protocol import Common
            import binascii
            (self.name, self.version, self.gameinfo, self.ip, self.title) = specifier.split(" ",4)
            self.gameinfo = Common.GameInfo(binascii.a2b_hex(self.gameinfo))

    def __str__(self):
        return self.name

    def info(self):
        s  = "        Name: %s\n" % self.name
        s += "       Title: %s\n" % self.title
        s += "     Version: %s\n" % self.version
        s += "          IP: %s\n" % self.ip
        s += "   Game Info: "
        firstKey = 1
        keys = self.gameinfo.__dict__.keys()
        keys.sort()
        for key in keys:
            if firstKey:
                firstKey = 0
            else:
                s += "              "
            s += "%s = %s\n" % (key, self.gameinfo.__dict__[key])
        return s
    

class ListServer:
    """List server abstract base class. This should be subclassed with either
       a local list server implementation or some glue for talking to a remote
       list server.
       """
    def add(self, name, build, version, gameinfo, title):
        """Add a server to the list. Note that the server will be tested for
           connectivity before actially appearing in the listings. If the port
           is None it is omitted from the listing and the default is assumed.
           """
        pass

    def remove(self, name):
        pass

    def setPlayerCounts(self, name, counts):
        """Set player counts. A dictionary of player counts indexed by color
           should be supplied. Counts not in the dictionary will be assumed zero.
           """
        pass

    def list(self):
        """Returns a list of ServerInfo classes"""
        pass

    
class RemoteListServer(ListServer):
    """Implementation of a ListServer that connects to a remote bzfls process"""
    def __init__(self, url):
        # Normally we'd use urlparse for this, but it has wacky defaults
        # for URL schemes it doesn't recognize.
        self.host = url.split("/")[2]
        if self.host.find(":") >= 0:
            (self.host, self.port) = self.host.split(":")
            self.port = int(self.port)
        else:
            self.port = 5156

    def command(self, line):
        """Send a generic bzfls command, returning the results"""
        import socket
        if type(line) == type(()) or type(line) == type([]):
            line = " ".join(map(str, line))
        s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        s.connect((self.host, self.port))
        s.send("%s\n\n" % line)
        response = ''
        while 1:
            buffer = s.recv(32768)
            if not buffer:
                break
            response += buffer
        s.close()
        return response

    def add(self, name, build, version, gameinfo, title):
        self.command(("ADD", name, build, version, gameinfo, title))

    def remove(self, name):
        self.command(("REMOVE", name))

    def setPlayerCounts(self, name, counts):
        self.command(("SETNUM",
                      counts.get("rogue",0), 
                      counts.get("red",0), 
                      counts.get("green",0), 
                      counts.get("blue",0), 
                      counts.get("purple",0)))

    def list(self):
        servers = []
        for line in self.command("LIST").split("\n"):
            line = line.strip()
            if line:
                servers.append(ServerInfo(line))
        return servers


def getDefault():
    """Factory for a default list server object"""
    return RemoteListServer(getListServerURL())

### The End ###
        
    
