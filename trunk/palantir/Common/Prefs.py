''' Prefs.py

Handles reading and writing of user prefs. Instantiating the Prefs
object will read in the prefs from either the default file or from
the user's custom file.
'''
# Copyright (C) 2004 W. Evan Sheehan <evan@navi.cx>
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

import string, os, sys
from Common.Palantir import dataDir

class Prefs:
  def __init__(self):
    defaults = {}
    # If we're on linux and we've got a ~/.palantirrc use that.
    if string.find(sys.platform, 'linux') != -1 and \
       os.path.exists(os.path.join(os.environ['HOME'],'.palantirrc')):
      file = open(os.path.join(dataDir, 'palantirrc'))
      lines = file.readlines()

      # Load default values to check against user defined prefs.
      for line in lines:
        pref = line.split('=')
        defaults[pref[0].strip()] = pref[1].strip()
      file = open(os.path.join(os.environ['HOME'],'.palantirrc'))

    # Otherwise use the config in the data directory.
    else:
      file = open(os.path.join(dataDir, 'palantirrc'))

    lines = file.readlines()
    # Set the preferences' values as attributes of the object.
    for line in lines:
      pref = line.split('=')
      setattr(self, pref[0].strip(), pref[1].strip())
    file.close()

    # Compare the default values against the ones we loaded to make sure
    # we didn't miss anything.
    if defaults:
      for key,value in defaults.iteritems():
        if key not in self.__dict__:
          setattr(self, key, value)

  def Save(self):
    ''' Write the current preference settings to ~/.palantirrc '''
    prefs = {}

    if string.find(sys.platform, 'linux') != -1:
      file = open(os.path.join(os.environ['HOME'], '.palantirrc'), 'w')
    else:
      # FIXME: Do we really want to overwrite the default rc file in windows?
      file = open(os.path.join(dataDir, 'palantirrc'), 'w')

    # For now we'll just write all of the prefs to the file, in the future
    # we may only write the values that differ from defaults.
    file.writelines([string.join(key, value, ' = ') \
        for key, value in self.__dict__.iteritems()])

    file.close()
