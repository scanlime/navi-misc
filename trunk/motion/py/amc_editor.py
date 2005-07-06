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

    def main (self):
        gtk.main ()

    def set_title (self):
        if self.filename is None:
            toplevel.set_title ('AMC Editor')
            return

        # FIXME - should really just extract the last bit of the filename
        if self.modified:
            toplevel.set_title (os.path.basename (self.filename) + '*')
        else:
            toplevel.set_title (os.path.basename (self.filename))

    def update_toolbar_sensitivity (self):
        self.widgets['menu_save'].set_sensitive   (self.modified)
        self.widgets['menu_saveas'].set_sensitive (self.filename is not None)

if __name__ == '__main__':
    editor = AMCEditor ()
    editor.main ()
