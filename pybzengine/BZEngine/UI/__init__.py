""" BZFlag.UI

This is a Python package providing various user interface frontends
for displaying the game state. This module contains an interface
for setting up a UI by name.
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

# Map short names to module names
nameMap = {
    'overhead': 'OverheadView',
    '3D':       'ThreeDView',
    'radar':    'RadarView',
    'anaglyph':	'AnaglyphGame',
    'game':     'GameView',
    'editor':   'EditorView',
    'stereo':	'StereoGame',
    }


def find(name):
    """Given the short name of a UI module, load and return it"""
    return __import__(nameMap[name], globals(), locals())


def Setup(name, *args, **kw):
    """Attach a UI to a game state and event loop, given the UI's name
       followed by the parameters normally passed to its attach method.
       """
    if name:
        module = find(name)
        return module.Setup(*args, **kw)


def list():
    """List the available UIs"""
    return nameMap.keys()


class Any:
    """A helper class for implementing optional UIs in command line
       apps. This class can be handed to CommandLine.Parse(), and the
       UI will automatically be added to the command line.
       Whichever UI is selected on the command line then will have its
       Setup class instantiated, attaching it to the current game state
       and event loop.
       """
    def __init__(self):
        self.options = {
            'ui': None,
            }

    def setOptions(self, **options):
        self.options.update(options)

    def Setup(self, *args, **kw):
        return Setup(self.options['ui'], *args, **kw)

### The End ###
