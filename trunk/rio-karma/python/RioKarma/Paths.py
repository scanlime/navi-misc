""" RioKarma.Paths

This is a simple module that's responsible for the global filesystem
paths used by the RioKarma package. These paths are needed to locate
the various cache files we keep.

"""
#
# Copyright (C) 2005 Micah Dowty <micah@navi.cx>
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

import os

_localDirOverride = None

def setLocalDir(path):
    """Explicitly set the root directory for local storage, overriding our autodetection"""
    global _localDirOverride
    _localDirOverride = path

def getLocalDir():
    """Return the root directory to use for local storage"""
    global _localDirOverride

    if _localDirOverride is not None:
        # We have an explicitly specified path
        path = _localDirOverride
    else:
        # Pick a default according to the OS

        if os.name == 'posix':
            # Use a dotfile in the user's home directory
            path = os.path.expanduser("~/.riokarma-py")

        elif os.name == 'nt':
            # We can still use expanduser, but give it a windows-friendly name
            path = os.path.expanduser(os.path.join("~", "RioKarma Python Package"))

        else:
            # Just stick it in the current directory
            path = os.path.abspath("riokarma-py")

    # Create it if necessary
    if not os.path.isdir(path):
        os.makedirs(path)
    return path

def getCache(name):
    """Return the full path to use for a cache with the provided name"""
    return os.path.join(getLocalDir(), "%s.cache" % name)

### The End ###
