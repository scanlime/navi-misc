#!/usr/bin/env python
#
# amc_editor.py - An editor for AMC files, based loosely on the IPO
#       interface in blender
#
# Copyright (C) 2005 David Trowbridge
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
#

import os
import sys

import pygtk
pygtk.require ('2.0')
import gtk
import gtk.glade

from Motion import AMC

class AMCEditor:
    def __init__ (self):
        self.widgets = {}
        self.modified = False
        self.filename = None

        gladefile = os.path.join (os.path.dirname(sys.argv[0]), 'amc-editor', 'amc-editor.glade')
        self._glade = gtk.glade.XML (gladefile)
        self._glade.signal_autoconnect (self)

        def get_widget (name):
            self.widgets[name] = self._glade.get_widget (name)
        get_widget ('bone_list')
        get_widget ('curve_editor')
        get_widget ('eventbox')
        get_widget ('menu_save')
        get_widget ('menu_saveas')
        get_widget ('toplevel')

        self._bone_store = gtk.TreeStore (str,           # name
                                          gtk.gdk.Color, # color
                                          object,        # bool - shown on curve window
                                          object,        # bool - whether toggle is visible
                                          str,           # bone index
                                          int,           # dof index
                                          int            # color index
                                          )
        self.widgets['bone_list'].set_model (self._bone_store)

        self.update_toolbar_sensitivity ()
        self.set_title ()
        self.widgets['toplevel'].show_all ()

    def main (self):
        gtk.main ()

    def set_title (self):
        if self.filename is None:
            self.widgets['toplevel'].set_title ('AMC Editor')
            return

        if self.modified:
            self.widgets['toplevel'].set_title (os.path.basename (self.filename) + '*')
        else:
            self.widgets['toplevel'].set_title (os.path.basename (self.filename))

    def update_toolbar_sensitivity (self):
        self.widgets['menu_save'].set_sensitive   (self.modified)
        self.widgets['menu_saveas'].set_sensitive (self.filename is not None)

    # signal handlers
    def on_quit (self, data=None):
        gtk.main_quit ()

    def on_delete_event (self, widget, data=None):
        return False

    def on_open (self, data=None):
        chooser = gtk.FileChooserDialog ('Open...', action = gtk.FILE_CHOOSER_ACTION_OPEN,
                                         buttons = (gtk.STOCK_CANCEL, gtk.RESPONSE_CANCEL,
                                                    gtk.STOCK_OPEN,   gtk.RESPONSE_OK))
        # File filter - *.amc
        filter = gtk.FileFilter ()
        filter.add_pattern ('*.amc')
        filter.set_name ('Acclaim Motion Capture (*.amc)')
        chooser.add_filter (filter)

        # File filter - all files
        filter = gtk.FileFilter ()
        filter.add_pattern ('*')
        filter.set_name ('All files')
        chooser.add_filter (filter)

        response = chooser.run ()
        chooser.hide ()

        if response == gtk.RESPONSE_OK:
            self.filename = chooser.get_filename ()
            amc = AMC.from_file (self.filename)
            self._bone_store.clear ()

            if amc is None:
                # FIXME - pop up error dialog
                print 'Error loading',self.filename
                self.filename = None
            else:
                self.set_title ()

        chooser.destroy ()

    def on_save (self, data=None):
        if self.filename is None:
            chooser = gtk.FileChooserDialog ('Save...', action = gtk.FILE_CHOOSER_ACTION_SAVE,
                                             buttons = (gtk.STOCK_CANCEL, gtk.RESPONSE_CANCEL,
                                                        gtk.STOCK_SAVE,   gtk.RESPONSE_OK))
            chooser.set_default_response (gtk.RESPONSE_OK)
            chooser.set_current_name (self.filename)

            response = chooser.run ()

            if response == gtk.RESPONSE_OK:
                self.filename = chooser.get_filename ()
                self.amc.save (self.filename)
                self.update_toolbar_sensitivity ()

            chooser.destroy ()
        else:
            self.amc.save (self.filename)
            self.modified = False

if __name__ == '__main__':
    editor = AMCEditor ()
    editor.main ()
