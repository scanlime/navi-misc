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
import Playlist

class Main:
    def __init__ (self):
        self.xml = gtk.glade.XML ("pledit.glade")
        self.playlist = Playlist.m3u ()
        treeview = self.xml.get_widget ("playlist")

        treeview.set_model (self.playlist.list)

        # Make a column that displays the desired track info for the playlist.
        cell = gtk.CellRendererText ()
        cell.mode = gtk.CELL_RENDERER_MODE_EDITABLE
        col = gtk.TreeViewColumn ("Track", cell, text=0)
        treeview.append_column (col)

        # Connect to all our signals.
        self.xml.signal_autoconnect (self)

    def _RemoveClicked (self, button, data=None):
        ''' Remove button callback. '''
        treeview = self.xml.get_widget ("playlist")
        model, iter = treeview.get_selection ().get_selected ()
        self.playlist.Remove (iter)

    def _AddClicked (self, button, data=None):
        ''' Add button callback. '''
        file = _FileChooser ()
        if file == None:
            return

        self.playlist.Add (file)

    def _NewActivated (self, item, data=None):
        ''' New menu item clicked. '''
        treeview = self.xml.get_widget ("playlist")
        # Replace the old playlist with the new
        if not self.playlist.saved:
            _SavePrompt ()

        self.playlist = Playlist.m3u ()

        treeview.set_model (self.playlist.list)

    def _OpenActivate (self, item, data=None):
        ''' Open menu item clicked. '''
        if not self.playlist.saved:
            _SavePrompt ()

        file = _FileChooser ()

    def _SaveActivated (self, item, data=None):
        ''' Save menu item clicked. '''
        if self.playlist.filename == None:
            file = _FileChooser ()

        self.playlist.Write (file)

    def _SaveAsActivated (self, item, data=None):
        ''' Save As menu item clicked. '''
        file = _FileChooser ()

        if file == None:
            return

        self.playlist.Write (file)

    def _QuitActivated (self, item, data=None):
        ''' Quit menu item clicked. '''
        if not self.playlist.saved:
            _SavePrompt ()

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


class _FileChooser:
    ''' File selection dialog. '''
    def __init__ (self):
        xml = gtk.glade.XML ("data/filechooser.glade")
        dialog = xml.get_widget ("filechooserdialog")
        response = dialog.run ()

        if response == gtk.RESPONSE_OK:
            return dialog.filename

        return None

class _SavePrompt:
    ''' Prompts the user to save before closing the current document. '''
    def __init__ (self):
        # FIXME
        print "Not implemented"


class _AboutBox:
    ''' Represents the about dialog. '''
    def __init__ (self):
        # FIXME
        print "Not implemented"


### The End ###
# vim:ts=4:sw=4:et:tw=80
