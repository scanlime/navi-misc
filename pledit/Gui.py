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
        window = gtk.glade.xml ("pledit.glade")

        # FIXME Should store the ListStore.

        window.signal_autoconnect (self)
        #callbacks = { "on_remove_clicked" : self._RemoveClicked,
                      #"on_add_clicked"    : self._AddClicked,
                      #"on_new_activate"   : self._NewActivated,
                      #"on_open_activate"  : self._OpenActivated,
                      #"on_save_actiavte"  : self._SaveActivated,
                      #"on_save_as_activate" : self._SaveAsActivated,
                      #"on_quit_activate"    : self._QuitActivated,
                      #"on_cut_activate"     : self._CutActivated,
                      #"on_copy_activate"    : self._CopyActivated,
                      #"on_paste_activate"   : self._PasteActivated,

    def _RemoveClicked (self, button, data=None):
        ''' Remove button callback. '''
        pass

    def _AddClicked (self, button, data=None):
        ''' Add button callback. '''
        pass

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
        pass

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


### The End ###
# vim:ts=4:sw=4:et:tw=80
