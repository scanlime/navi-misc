''' Playlist.py

Classes for creating playlists in various formats. Currently only m3u is
supported.
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


import gtk, gobject, MusicFile


class IPlaylist:
    ''' Playlist interface. All playlists should impelement this. '''
    def __init__ (self, file=None):
        self.file = file
        if file:
            self.Open (file)

    def Add (self, filename):
        pass

    def Remove (self, iterator):
        pass

    def Write (self, filename=None):
        pass

    def Open (self, filename):
        pass


class m3u (IPlaylist):
    ''' Class that represents an m3u playlist. '''
    def __init__ (self, file=None):
        self.list = gtk.ListStore (gobject.TYPE_STRING, MusicFile._File)
        self.saved = True
        self.format = "%t - %a - %p"
        IPlaylist.__init__ (self, file)

    def Add (self, filename):
        f = MusicFile.Factory (filename)
        #title = f.toString (self.format)
        self.list.set (self.list.append (), 0, filename, 1, f)

    def Remove (self, iterator):
        self.list.remove (iterator)

    def Write (self, filename=None):
        # FIXME
        pass


### The End ###
# vim:ts=4:sw=4:et:tw=80
