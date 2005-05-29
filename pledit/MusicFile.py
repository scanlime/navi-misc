'''
'''

# Copyright (C) 2005 W. Evan Sheehan
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License
# as published by the Free Software Foundation; either version 2
# of the License, or (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

import gobject, re


def Factory (filename):
    ''' A MusicFile factory that creates an object representing the file given
        by filename of the appropriate type. If the filetype is unknown it
        throws an exception.
        '''
    # FIXME
    pass


class _File (gobject.GObject):
    ''' A generic music file class. '''
    def __init__ (self):
        gobject.GObject.__init__ (self)

    def toString (self, format):
        ''' Create a string representing the file using format. '''
        # FIXME
        pass


class Ogg (_File):
    ''' Class representing an ogg file. '''
    def __init__ (self):
        _File.__init__ (self)


class Mp3 (_File):
    ''' Class representing an mp3 file. '''
    def __init__ (self):
        _File.__init__ (self)



### The End ###
# vim:ts=4:sw=4:et:tw=80
