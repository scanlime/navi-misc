""" Wasabi.Icon

"""
#
# Wasabi Project
# Copyright (C) 2003-2004 Micah Dowty and David Trowbridge
#
#  This library is free software; you can redistribute it and/or
#  modify it under the terms of the GNU Lesser General Public
#  License as published by the Free Software Foundation; either
#  version 2.1 of the License, or (at your option) any later version.
#
#  This library is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
#  Lesser General Public License for more details.
#
#  You should have received a copy of the GNU Lesser General Public
#  License along with this library; if not, write to the Free Software
#  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
#

from gtk import gdk
import os

class Icon:
    """This is a pretty simple object on top of gdk_pixbuf that provides
       an icon at a couple predefined sizes."""
    def __init__(self, filename):
        realfile = os.path.join('data', filename)
        self.base = gdk.pixbuf_new_from_file(realfile)
        # assumes images are 3:4 !!
        self.i120 = self.base.scale_simple(160, 120, gdk.INTERP_HYPER)
        self.i240 = self.base.scale_simple(320, 240, gdk.INTERP_HYPER)
