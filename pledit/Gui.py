''' Gui.py

GUI for pledit.
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

import pygtk, gtk, gtk.glade

class Main:
    def __init__ (self):
        window = gtk.glade.XML ("pledit.glade")
        playlist = window.get_widget ("playlist")

        # Create a ListStore for the playlist and add it to the TreeView.
        self.songList = gtk.ListStore (gobject.TYPE_STRING,
                                       gobject.TYPE_INT,
                                       gobject.TYPE_STRING)
        playlist.set_model (self.songList)

        # Make a column that displays the desired track info for the playlist.
        cell = gtk.CellRendererText ()
        cell.mode = gtk.CELL_RENDERER_MODE_EDITABLE
        col = gtk.TreeViewColumn ("Track", cell, text=0)
        playlist.append_column (col)

        # Connect to all our signals.
        window.signal_autoconnect (self)

    def _RemoveClicked (self, button, data=None):
        ''' Remove button callback. '''
        print "remove"

    def _AddClicked (self, button, data=None):
        ''' Add button callback. '''
        SongChooser (self.songList)

    def _NewActivated (self, item, data=None):
        ''' New menu item clicked. '''
        pass

    def _OpenActivate (self, item, data=None):
        ''' Open menu item clicked. '''
        pass

    def _SaveActivated (self, item, data=None):
        ''' Save menu item clicked. '''
        pass

    def _SaveAsActivated (self, item, data=None):
        ''' Save As menu item clicked. '''
        pass

    def _QuitActivated (self, item, data=None):
        ''' Quit menu item clicked. '''
        # FIXME Check if the playlist has been saved, if not prompt.
        gtk.main_quit ()

    def _CutActivated (self, item, data=None):
        ''' Cut menu item clicked. '''
        pass

    def _CopyActivated (self, item, data=None):
        ''' Copy menu item clicked. '''
        pass

    def _PasteActivated (self, item, data=None):
        ''' Paste menu item clicked. '''
        pass

    def _DeleteActivated (self, item, data=None):
        ''' Delete menu item clicked. '''
        pass

    def _AboutActivated (self, item, data=None):
        ''' About menu item clicked. '''
        pass

    def _Delete (self, widget, event, data=None):
        ''' Bye bye. '''
        gtk.main_quit ()


class SongChooser:
    def __init__ (self, list):
        pass


class AboutBox:
    def __init__ (self):
        pass


### The End ###
# vim:ts=4:sw=4:et:tw=80
