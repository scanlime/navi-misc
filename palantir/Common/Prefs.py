''' Prefs.py

Handles reading and writing of user prefs.
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

import string

class Prefs:
  def __init__(self):
    try:
      # FIXME: filename.
      file = open('palantirrc', 'r')
    except IOError:
      return

    lines = file.readlines()
    for line in lines:
      pref = line.split('=')
      setattr(self, pref[0].strip(), string.join(pref[1:]).strip())
    file.close()

  def Save(self):
    ''' Write the current preference settings to ~/.palantirrc '''
    # FIXME: filename.
    file = open('~/.palantirrc', 'w')
    print self.__dict__
    file.close()
