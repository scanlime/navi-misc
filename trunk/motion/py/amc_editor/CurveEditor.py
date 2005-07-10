# CurveEditor.py - a widget which provides blender-IPO-like editing
#       features for AMC data
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

import gobject
import gtk
import gtk.gdk
import pango

class CurveEditor (gtk.DrawingArea):
    __gsignals__ = {
        'set-scroll-adjustments' : (gobject.SIGNAL_RUN_LAST, gobject.TYPE_NONE, (gtk.Adjustment, gtk.Adjustment))
    }

    def __init__ (self):
        gobject.GObject.__init__ (self)

        self._enabled_bones = {}
        self._pad_positions = []
        self._selected_pads = []

        self._redraw_timeout = 0

        self._visible_range = (0, 0)
        self._grabbed = False
        self._lasso = False

    def _init_graphics (self):
        self._colors = {}

        white = gtk.gdk.Color (0xff, 0xff, 0xff)
        grey  = gtk.gdk.Color (0xde, 0xde, 0xde)
        black = gtk.gdk.Color (0x00, 0x00, 0x00)
        lasso = gtk.gdk.Color (0xa0, 0xa0, 0xa0)

        self._colors['white'] = gtk.gdk.GC (self.window)
        self._colors['grey']  = gtk.gdk.GC (self.window)
        self._colors['black'] = gtk.gdk.GC (self.window)
        self._colors['lasso'] = gtk.gdk.GC (self.window)

        colormap = self.window.get_colormap ()

        colormap.alloc_color (white, writeable = True, best_match = True)
        colormap.alloc_color (grey,  writeable = True, best_match = True)
        colormap.alloc_color (black, writeable = True, best_match = True)
        colormap.alloc_color (lasso, writeable = True, best_match = True)

        self._colors['white'].set_foreground (white)
        self._colors['grey'].set_foreground  (grey)
        self._colors['black'].set_foreground (black)
        self._colors['lasso'].set_foreground (lasso)

        self._big_layout   = self.create_pango_layout ('')
        self._small_layout = self.create_pango_layout ('')

        self._big_layout.set_font_description   ('Bitstream Vera Sans 8')
        self._small_layout.set_font_description ('Bitstream Vera Sans 6')

    def _create_back_buffer (self):
        self._back_buffer = gtk.gdk.Pixmap (self.window, self.allocation.width, self.allocation.height)

    def set_scroll_adjustments (self, hadjustment, vadjustment, data=None):
        self.hadj = hadjustment
        self.vadj = vadjustment

        self.hadj.lower          = 0;
        self.hadj.upper          = 1;
        self.hadj.step_increment = 0;
        self.hadj.page_size      = 1;
        self.hadj.page_increment = 0;
        self.hadj.value          = 0;

        self.vadj.lower          = 0;
        self.vadj.upper          = 1;
        self.vadj.step_increment = 0;
        self.vadj.page_size      = 1;
        self.vadj.page_increment = 0;
        self.vadj.value          = 0;
