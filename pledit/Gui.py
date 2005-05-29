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
        self.xml = gtk.glade.XML ("data/pledit.glade")
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

        try:
            self.playlist.Add (file)
        except KeyError:
            _ErrDialog ("Unknown File Type", "%s has an unsupported file type."
                    % (file))

    def _NewActivated (self, item, data=None):
        ''' New menu item clicked. '''
        treeview = self.xml.get_widget ("playlist")
        # Replace the old playlist with the new
        if not self.playlist.saved:
            if _CloseDialog () == gtk.RESPONSE_CANCEL:
                return

        self.playlist = Playlist.m3u ()

        treeview.set_model (self.playlist.list)

    def _OpenActivate (self, item, data=None):
        ''' Open menu item clicked. '''
        if not self.playlist.saved:
            if _CloseDialog () == gtk.RESPONSE_CANCEL:
                return

        file = _FileChooser ()
        self.playlist = Playlist.m3u (file)

        treeview = self.xml.get_widget ("playlist")
        treeview.set_model (self.playlist.list)

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
            if _CloseDialog () == gtk.RESPONSE_CANCEL:
                return

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


################################ Dialogs ###################################
def _FileChooser ():
    ''' File selection dialog. '''
    xml = gtk.glade.XML ("data/filechooser.glade")
    dialog = xml.get_widget ("filechooserdialog")
    response = dialog.run ()

    dialog.destroy ()

    if response == gtk.RESPONSE_OK:
        return dialog.get_filename ()

    return None

def _CloseDialog (doc):
    ''' Prompts the user to save before closing the current document. '''
    xml = gtk.glade.XML ("data/closedialog.glade")
    if doc.file != None:
        text = xml.get_widget ("dialog text")
        text.label = '<b>Save changes to playlist "' + doc.file + '"before closing?</b>\n\nThe playlist has unsaved changes, do you wish to quit anyway?'

    dialog = xml.get_widget ("close dialog")
    response = dialog.run ()

    if response == gtk.RESPONSE_YES:
        if doc.file == None:
            doc.file = _FileChooser ()

        doc.Write ()
        return gtk.RESPONSE_CLOSE

    return response


def _ErrDialog (primary, secondary=""):
    ''' A generic error dialog box. '''
    xml = gtk.glade.XML ("data/errordialog.glade")

    msg = "<b>%s</b>\n\n%s" % (primary, secondary)
    xml.get_widget ("text").label = msg

    return xml.get_widget ("error dialog").run ()


def _AboutBox ():
    ''' Represents the about dialog. '''
    # FIXME
    print "Not implemented"


### The End ###
# vim:ts=4:sw=4:et:tw=80
