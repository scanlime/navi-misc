""" BZEngine.UI.Drawable.VRML.Cache

A simple cache for loaded VRML models. This stores cPickle'd
dicts associating mesh name to a Mesh instance, keyed by the MD5 hash
of the raw VRML file.
"""
#
# Python BZEngine Package
# Copyright (C) 2003-2004 Micah Dowty <micahjd@users.sourceforge.net>
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

import BZEngine, cPickle, os, md5
from Parser import Reader

__all__ = ("Cache",)


class Cache:
    """This is both an in-memory and on-disk cache for VRML objects."""
    def __init__(self, path=BZEngine.cachePath):
        self.objects = {}
        self.path = os.path.expanduser(path)
        try:
            os.makedirs(self.path)
        except OSError:
            # Path could already exist or it could be unreachable. We don't care.
            pass

    def getCacheFilename(self, name):
        """To uniquely identify this file, take an MD5 checksum of it"""
        f = open(name)
        hash = md5.new(f.read()).hexdigest()
        f.close
        return os.path.join(self.path, "p%s.meshDict" % hash)

    def load(self, name):
        """This checks the in-memory cache first, then the disk cache.
           Returns a dictionary associating mesh names and drawables.
           """
        try:
            # Use the filename as the memory cache key- we don't want
            # to checksum the file every time we load it from the memory cache.
            return self.objects[name]
        except KeyError:
            # Not in the memory cache, try the disk cache
            cacheFilename = self.getCacheFilename(name)
            if os.path.isfile(cacheFilename):
                f = open(cacheFilename, "rb")
                meshes = cPickle.load(f)
                f.close()
                self.objects[name] = meshes
                return meshes

            # Not in either cache, load it
            else:
                meshes = Reader(name).meshes
                self.objects[name] = meshes
                # Try to store it in the disk cache. If we can't, no big deal.
                try:
                    f = open(cacheFilename, "wb")
                    cPickle.dump(meshes, f, True)
                    f.close()
                except IOError:
                    pass
                return meshes


defaultCache = Cache()
def load(name):
    return defaultCache.load(Util.dataFile(name))

### The End ###
