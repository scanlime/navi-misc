""" BZEngine.UI.Viewport

A Viewport in this context is an area that can be drawn to and
provides event handling. The idea is that views associate with
viewports for event handling and some of their context management
rather than directly with the underlying graphics library, so that
it's easy to show multiple views in the same frame.

For every underlying graphics library supported, this module
provides a class that manages event handling for that library.
This includes redraw events, user-supplied events, and connecting
the UI library with the event loop. In turn, the viewport provides
events that views and controllers can hook on to.

The reasoning behind having events multiplex through the view as
well as rendering is so that mouse events can be made relative to
the view, keyboard events can have focus handled, etc. depending
on the needs of a particular view multiplexer class.
"""
#
# Python BZEngine Package
# Copyright (C) 2003 Micah Dowty <micahjd@users.sourceforge.net>
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

from Base import *

# Lazily import the actual viewport implementations, to keep dependencies in check

def PygameViewport(*args, **kw):
    global Pygame
    import Pygame
    return Pygame.PygameViewport(*args, **kw)

def OpenGLViewport(*args, **kw):
    global GL
    import GL
    return GL.OpenGLViewport(*args, **kw)

def StereoGLViewport(*args, **kw):
    global GL
    import GL
    return GL.StereoGLViewport(*args, **kw)

### The End
