""" Wasabi.Settings

Implements Wasabi's settings menu and its submenus.
"""
#
# Wasabi Project
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

from Wasabi import Menu, Icon
from BZEngine.UI import Texture
from BZEngine import Util
import os, md5, cPickle, Image, glob


class SettingsMenu(Menu.ArcMenu):
    def __init__(self, book):
        items = [
            Menu.PageItem(Icon.load('background'), [
            BackgroundLoader,
            Menu.defaultFades(BackgroundsMenu),
            ])
            ]
        Menu.ArcMenu.__init__(self, book, items, "Settings")


class BackgroundLoader(Menu.LoaderPage):
    """Loads backgrounds into memory before the backgrounds menu itself runs"""
    def run(self):
        bgc = BackgroundCache()
        for file in getBackgroundList():
            print file
            print bgc.load(file)


class BackgroundsMenu(Menu.ArcMenu):
    def __init__(self, book):
        items = []

        Menu.ArcMenu.__init__(self, book, items, "Backgrounds")


def getBackgroundList():
    """Return a list of available background filenames by executing
       the backgrounds.py data file. Functions and modules helpful for
       locating backgrounds are placed in its an
       """
    l = []
    ns = {
        'glob': glob.glob,
        'dataFile': Util.dataFile,
        'os': os,
        'list': l
        }
    exec open(Util.dataFile('backgrounds.py')) in ns
    return l


class BackgroundCache:
    """Given a path to an image PIL can open, rescales the image,
       converts to a texture, and caches the result.
       """
    def __init__(self, cacheDir="~/.wasabi/bgcache", textureSize=(512,512)):
        self.cacheDir = os.path.expanduser(cacheDir)
        self.textureSize = textureSize

        # Make sure our cache directory exists
        try:
            os.makedirs(self.cacheDir)
        except OSError:
            pass

    def getCacheFilename(self, file):
        """For a given path, return the filename to be used in the cache"""
        file = os.path.abspath(file)
        return os.path.join(self.cacheDir, md5.new(file).hexdigest() + ".p")

    def load(self, file):
        """Load the given file into a texture, using the cache if possible"""
        cacheFilename = self.getCacheFilename(file)
        if os.path.isfile(cacheFilename):
            # Load the cached texture
            return cPickle.load(open(cacheFilename, "rb"))
        else:
            # Load and rescale the image
            img = Image.open(file).resize(self.textureSize, Image.BICUBIC)

            # Load it into a texture, and cache it
            tex = Texture.Texture()
            tex.loadImage(img)
            cPickle.dump(tex, open(cacheFilename, "wb"), -1)
            return tex

### The End ###
