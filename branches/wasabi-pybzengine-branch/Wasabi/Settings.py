""" Wasabi.Settings

Implements Wasabi's settings menu and its submenus.
"""
#
# Wasabi Project
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

from __future__ import division
from Wasabi import Menu, Icon, Hardware
from BZEngine.UI import Texture
from BZEngine import Util
import os, md5, cPickle, Image, glob


class SettingsMenu(Menu.ArcMenu):
    def __init__(self, book):
        items = [
            # 'background' icon, loads the backgrounds then runs the backgrounds menu
            Menu.PageItem(Icon.load('background'), [
            BackgroundLoader,
            Menu.defaultFades(BackgroundsMenu),
            ])
            ]
        Hardware.dev.showStatus("Settings")
        Menu.ArcMenu.__init__(self, book, items, "Settings")


backgroundItems = None

class BackgroundLoader(Menu.LoaderPage):
    """Loads backgrounds into memory before the backgrounds menu itself runs"""
    def run(self):
        global backgroundItems
        if not backgroundItems:
            backgroundItems = []
            bgc = BackgroundCache()
            for file in getBackgroundList():
                item = Menu.Item(Icon.Icon(bgc.load(file), imageAspect=4/3))
                item.file = file
                backgroundItems.append(item)


class BackgroundsMenu(Menu.ImageMenu):
    def __init__(self, book):
        global backgroundItems
        Hardware.dev.showStatus("Backgrounds")
        Menu.ImageMenu.__init__(self, book, backgroundItems)

        # Find the current background, and make it the default selection
        current = getCurrentBackground()
        i = 0
        for item in backgroundItems:
            if item.file == current:
                self.dock.selectionIndex = i
                self.dock.jump()
                break
            i += 1

    def onSelected(self, item):
        setCurrentBackground(item.file)

        # We're done with the backgroundItems list, free up a big chunk of memory
        global backgroundItems
        backgroundItems = None


def getBackgroundList():
    """Return a list of available background filenames by executing
       the backgrounds.py data file. Functions and modules helpful for
       locating backgrounds are placed in its an
       """
    ns = {
        'glob': glob.glob,
        'dataFile': Util.dataFile,
        'os': os,
        'list': []
        }
    exec open(Util.dataFile('backgrounds.py')) in ns
    return ns['list']


def getCurrentBackground():
    """Return the path of the current background file"""
    try:
        return open(os.path.expanduser("~/.wasabi/current_background")).read().strip()
    except IOError:
        # Return the default background
        return getBackgroundList()[0]


def setCurrentBackground(path):
    """Save the given path as the current background file"""
    open(os.path.expanduser("~/.wasabi/current_background"), "w").write(path + "\n")


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
