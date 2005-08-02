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
    def __init__ (self):
        gtk.DrawingArea.__init__ (self)

        self.amc = None
        self._nframes = 0

        self._colors = None
        self._enabled_bones = {}
        self._pad_positions = []
        self._selected_pads = []

        self._redraw_timeout = 0

        self._visible_range = [0, 0]
        self._grabbed = False
        self._lasso = False

        self.hadj = None
        self.vadj = None

        self.connect ('configure-event', self.configure_event)
        self.connect ('expose-event', self.expose_event)

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

        self._big_layout.set_font_description   (pango.FontDescription ('Bitstream Vera Sans 8'))
        self._small_layout.set_font_description (pango.FontDescription ('Bitstream Vera Sans 6'))

    def _create_back_buffer (self):
        self._back_buffer = gtk.gdk.Pixmap (self.window, self.allocation.width, self.allocation.height)

    def _draw_frame_line (self, number, position):
        if not self._is_visible (position):
            return

        self._back_buffer.draw_line (self._colors['black'],
                                     int (position - self.hadj.value), 0,
                                     int (position - self.hadj.value), self.allocation.height - 30)

    def _draw (self):
        if self._colors is None:
            self._init_graphics ()

        self._back_buffer.draw_rectangle (self._colors['grey'], True, 0, 0, self.allocation.width, self.allocation.height)

        if self.amc is None:
            return

        self._pad_positions = []

        for i in range(self._visible_range[0], self._visible_range[1]):
            pos = i * 40 + 20
            self._draw_frame_line (i, pos)

            # Iterate through all the bones, drawing the ones that are enabled
            for bone in self._enabled_bones:
                self._draw_bone (i, pos, bone)

        # FIXME - more

    def set_scroll_adjustments (self, hadjustment, vadjustment, data=None):
        self.hadj = hadjustment
        self.vadj = vadjustment

        self.hadj.lower          = 0
        self.hadj.upper          = 1
        self.hadj.step_increment = 0
        self.hadj.page_size      = 1
        self.hadj.page_increment = 0
        self.hadj.value          = 0

        self.vadj.lower          = 0
        self.vadj.upper          = 1
        self.vadj.step_increment = 0
        self.vadj.page_size      = 1
        self.vadj.page_increment = 0
        self.vadj.value          = 0

    def configure_event (self, widget, event):
        if self.hadj is not None:
            self.hadj.page_size      = event.width
            self.hadj.page_increment = event.width
            self.hadj.step_increment = 120

        self._create_back_buffer ()
        self._draw ()

        return True

    def expose_event (self, widget, event):
        self.window.draw_drawable (self._colors['grey'], self._back_buffer, event.area.x, event.area.y, event.area.x, event.area.y, event.area.width, event.area.height)

    def set_amc (self, amc):
        self.amc = amc

        self._enabled_bones = []
        self._nframes = amc.bones['root'].shape[0]

        self.hadj.upper = self._nframes * 40;
        self.hadj.value = 0

        self._recompute_range ()
        self._draw ()

    def _recompute_range (self):
        self._visible_range[0] = int (self.hadj.value / 40) - 1
        i = self._visible_range[0] + 1
        while self._is_visible (i * 40 + 20) and i < self._nframes:
            i = i + 1
        self._visible_range[1] = i

    def _is_visible (self, x):
        return ((x > self.hadj.value) and (x < (self.hadj.value, + self.allocation.width)))
