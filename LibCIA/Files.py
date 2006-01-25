""" LibCIA.Files

This module is just a centralized place to manage CIA's filesystem
usage. A top-level data directory can be set here during initialization,
and this module is later queried for individual directories to use.

The top-level directory ('data' by default) is split into three branches:

  db     Persistent data that should be backed up.

  temp   Transient files. These should be automatically deleted when they
         are no longer needed. It's safe to clear this directory when the
         server isn't running.

  cache  Data that is cached to improve performance, but can be rebuilt
         from 'db' at any time. Files in the cache can be safely removed
         at any time.

  log    Log files written by the server. These are only for the
         administrator's benefit, and can be removed at any time.

"""
#
# CIA open source notification system
# Copyright (C) 2003-2005 Micah Dowty <micah@navi.cx>
#
#  This program is free software; you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation; either version 2 of the License, or
#  (at your option) any later version.
#
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with this program; if not, write to the Free Software
#  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
#

import os, random

def setDataRoot(path):
    """Change the root 'data' directory. This can be called in
       your .tac file to override the default location for all
       data files. The default is set below to 'data', in the
       same directory as LibCIA.
       """
    global dataRoot
    dataRoot = os.path.realpath(path)

setDataRoot(os.path.join(os.path.split(__file__)[0], '..', 'data'))

# Top-level directories
dbDir = 'db'
tempDir = 'temp'
cacheDir = 'cache'
logDir = 'log'

def getDir(*seg):
    """Get a data directory made from the provided path segments.
       This returns a full path, creating it if necessary.
       """
    path = os.path.join(dataRoot, *seg)
    if not os.path.isdir(path):
        os.makedirs(path)
    return path

def tryGetDir(*seg):
    """Like getDir, but this will not check the directory
       for existence nor create any new directories.
       """
    return os.path.join(dataRoot, *seg)

def getCacheFile(ns, digest):
    """Get a cache file in the provided namespace (directory)
       for an item with the provided digest string.
       """
    return os.path.join(getDir(cacheDir, ns), digest)

def getTempFile():
    """Return a new temporary filename, guaranteed
       not to exist at the moment. This is not designed
       to be used when our data directory may be shared
       by other users, so it doesn't deal with the race
       condition that would result in that situation.
       """
    root = getDir(tempDir)
    for i in xrange(100):
        path = os.path.join(root, '%d-%d' % (
            os.getpid(), random.randint(100000, 999999)))
        if not os.path.isfile(path):
            return path
    raise NotImplementedError("getTempFile() appears to be failing")

### The End ###
