#!/usr/bin/env python
#
# amc_editor.py - An editor for AMC files, based loosely on the IPO
#       interface in blender
#
# Copyright (C) 2005-2006 David Trowbridge
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
import gobject
import gtk
import gtk.glade

from Motion import AMC
from amc_editor import Color
from amc_editor.CurveEditor import CurveEditor

class AMCEditor:
    def __init__ (self):
        self.widgets = {}
        self.modified = False
        self.filename = None

        gtk.glade.set_custom_handler (self.glade_custom_handler)

        gladefile = os.path.join (os.path.dirname(sys.argv[0]), 'amc_editor', 'amc-editor.glade')
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
        get_widget ('hscroll')
        get_widget ('vscroll')

        self._bone_store = gtk.TreeStore (str,           # name
                                          gtk.gdk.Color, # color
                                          'gboolean',    # shown on curve window
                                          'gboolean',    # whether toggle is visible
                                          str,           # bone index
                                          int,           # dof index
                                          int            # color index
                                          )
        self.widgets['bone_list'].set_model (self._bone_store)

        self.widgets['curve_editor'].set_scroll_adjustments (self.widgets['hscroll'].get_adjustment (),
                                                             self.widgets['vscroll'].get_adjustment ())
        self.widgets['curve_editor']._bone_store = self._bone_store

        # Create text column
        text_column   = gtk.TreeViewColumn ()
        text_renderer = gtk.CellRendererText ()
        text_column.pack_start (text_renderer, True)
        text_column.add_attribute (text_renderer, 'markup'        , 0)
        text_column.add_attribute (text_renderer, 'foreground-gdk', 1)
        self.widgets['bone_list'].append_column (text_column)

        # Create visibility toggle column
        visible_column   = gtk.TreeViewColumn ()
        visible_renderer = gtk.CellRendererToggle ()
        visible_column.pack_start (visible_renderer, False)
        visible_column.add_attribute (visible_renderer, 'active',  2)
        visible_column.add_attribute (visible_renderer, 'visible', 3)
        visible_renderer.activatable = True
        visible_renderer.connect ('toggled', self.row_toggled)
        self.widgets['bone_list'].append_column (visible_column)

        self.update_toolbar_sensitivity ()
        self.set_title ()
        self.widgets['toplevel'].show_all ()

    def main (self):
        gtk.main ()

    def row_toggled (self, cell, path):
        iter = self._bone_store.get_iter (path)
        t = not self._bone_store.get_value (iter, 2)
        self._bone_store.set_value (iter, 2, t)

        if (t):
            self.widgets['curve_editor'].enable_bone (path)
        else:
            self.widgets['curve_editor'].disable_bone (path)

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

                for (bone, data) in amc.bones.iteritems ():
                    iter = self._bone_store.append (parent = None)
                    self._bone_store.set_value (iter, 0, bone)
                    self._bone_store.set_value (iter, 1, gtk.gdk.Color (0x00, 0x00, 0x00));
                    self._bone_store.set_value (iter, 3, 0)

                    for degree in range (data.shape[1]):
                        iter2 = self._bone_store.append (parent = iter)
                        color = Color.alloc_color ()
                        self._bone_store.set (iter2, 0, '<b>%d</b>' % degree,
                                                     1, Color.colors[color],
                                                     2, 0,
                                                     3, 1,
                                                     4, bone,
                                                     5, degree,
                                                     6, color,
                                                     )
                self.widgets['curve_editor'].set_amc (amc)
                self.modified = False
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

    def glade_custom_handler (self, glade, func_name, name, str1, str2, int1, int2):
        if func_name == 'CurveEditor':
            return CurveEditor ()

if __name__ == '__main__':
    editor = AMCEditor ()
    editor.main ()
