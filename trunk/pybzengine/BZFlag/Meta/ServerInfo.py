""" BZFlag.Meta.ServerInfo

A class for packaging general server information in the format
used by the metaserver.
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

class ServerInfo:
    """Represents information about one BZFlag server"""
    def __init__(self, specifier=None):
        self.name = None
        self.version = None
        self.gameinfo = None
        self.ip = None
        self.title = None
        self.build = 0
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

### The End ###
