""" Captcha.File

Utilities for finding and picking random files from our 'data' directory
"""
#
# PyCAPTCHA Package
# Copyright (C) 2004 Micah Dowty <micah@navi.cx>
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

# Determine the data directory. This can be overridden after import-time if needed.
dataDir = os.path.join(os.path.split(os.path.abspath(__file__))[0], "data")


class RandomFileFactory(object):
    """Given a list of files and/or directories, this picks a random file.
       Directories are searched for files matching any of a list of extensions.
       Files are relative to our data directory plus a subclass-specified base path.
       """
    extensions = []
    basePath = "."

    def __init__(self, *fileList):
        self.fileList = fileList
        self._fullPaths = None

    def _checkExtension(self, name):
        """Check the file against our given list of extensions"""
        for ext in self.extensions:
            if name.endswith(ext):
                return True
        return False

    def _findFullPaths(self):
        """From our given file list, find a list of full paths to files"""
        paths = []
        for name in self.fileList:
            path = os.path.join(dataDir, self.basePath, name)
            if os.path.isdir(path):
                for content in os.listdir(path):
                    if self._checkExtension(content):
                        paths.append(os.path.join(path, content))
            else:
                paths.append(path)
        return paths

    def pick(self):
        if self._fullPaths is None:
            self._fullPaths = self._findFullPaths()
        return random.choice(self._fullPaths)

### The End ###
