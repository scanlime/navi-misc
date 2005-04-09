/*
 * NavigationImage.cs - Widget that handles the navigation image in the lower
 * 	right corner.
 *
 * Fyre - a generic framework for computational art
 * Copyright (C) 2004-2005 Fyre Team (see AUTHORS)
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 *
 */

namespace Fyre
{

	class NavigationWindow : Gtk.Window
	{
		public int Width, Height;

		public
		NavigationWindow (PipelineDrawing drawing) : base (Gtk.WindowType.Popup)
		{
			Gdk.Rectangle canvas = drawing.CanvasExtents;

			// We always use a 200px box for our navigation window. Height varies
			// depending on the aspect ratio of the current drawing.
			float aspect = ((float) canvas.Width) / ((float) canvas.Height);
			Width = 200;
			Height = (int) (Width / aspect);

			// We need to realize before we can access any of the gdk internals,
			// like setting size/position
			Realize ();
			Resize (Width, Height);
		}
	}

	class NavigationImage : Gtk.EventBox
	{
		Gtk.Image		image;
		NavigationWindow	window;
		Gdk.Rectangle		window_rect;
		public PipelineDrawing	Drawing;

		public
		NavigationImage ()
		{
			Gdk.Pixbuf pixbuf = new Gdk.Pixbuf (null, "navigation.png");
			image = new Gtk.Image (pixbuf);
			Add (image);

			ShowAll ();
		}

		int
		GetWindowPosition (int mouse, int winsize, int screensize)
		{
			if (mouse - (winsize / 2) < 0)
				return 0;
			if (mouse + (winsize / 2) > screensize)
				return screensize - winsize;
			return mouse - (winsize / 2);
		}

		protected override bool
		OnButtonPressEvent (Gdk.EventButton ev)
		{
			if (Drawing == null)
				return true;

			Gdk.Screen screen = GdkWindow.Screen;

			int mouse_x = (int) ev.XRoot;
			int mouse_y = (int) ev.YRoot;

			window = new NavigationWindow (Drawing);
			window.Move (GetWindowPosition (mouse_x, window.Width,  screen.Width),
				     GetWindowPosition (mouse_y, window.Height, screen.Height));

			window.Show ();

			return true;
		}

		protected override bool
		OnButtonReleaseEvent (Gdk.EventButton ev)
		{
			// Hide and destroy our window
			window.Hide ();
			window = null;

			return true;
		}

		protected override bool
		OnMotionNotifyEvent (Gdk.EventMotion ev)
		{
			return true;
		}
	}

	/************************************************************************
	 *******************          CRUFT BARRIER          ********************
	 ***********************************************************************/

	class NavigationWindow2 : Gtk.Window
	{
		Gdk.Pixmap		background;
		Gdk.Pixmap		backing;
		Gdk.GC			white, black;
		int[]			size, visible, mouse;

		public
		NavigationWindow2 (int w, int h, int vw, int vh) : base (Gtk.WindowType.Popup)
		{
			size = new int[2];
			visible = new int[2];
			mouse = new int[2];

			// Set window size
			size[0] = w;
			size[1] = h;

			// Set visible size
			visible[0] = vw;
			visible[1] = vh;

			// Initialize mouse to nothing
			mouse[0] = -1;
			mouse[1] = -1;

			// Before we can access any gdk internals, we need
			// to realize the window.
			Realize ();

			Resize (w, h);

			background = new Gdk.Pixmap (GdkWindow, w, h, -1);
			backing = new Gdk.Pixmap (GdkWindow, w, h, -1);

			DrawBackground ();
		}

		void
		AllocGCs ()
		{
			Gdk.Color wc = new Gdk.Color (0xff, 0xff, 0xff);
			Gdk.Color bc = new Gdk.Color (0x00, 0x00, 0x00);
			Gdk.Colormap.System.AllocColor (ref wc, true, true);
			Gdk.Colormap.System.AllocColor (ref bc, true, true);
			white = new Gdk.GC (this.GdkWindow);
			black = new Gdk.GC (this.GdkWindow);
			white.Foreground = wc;
			black.Foreground = bc;
		}

		void
		DrawBackground ()
		{
			if (white == null)
				AllocGCs ();

			background.DrawRectangle (white, true, 0, 0, size[0] - 1, size[1] - 1);
			background.DrawRectangle (black, false, 0, 0, size[0] - 1, size[1] - 1);

			// FIXME - we want to draw our pipeline onto this window at reduced size
		}

		void
		DrawArea (Gdk.Rectangle area)
		{
			// Update the backing store
			int vx = visible[0] / 2;
			int vy = visible[1] / 2;
			backing.DrawDrawable (white, background, area.X, area.Y, area.X, area.Y, area.Width, area.Height);
			if (mouse[0] == -1)
				return;

			Gdk.Rectangle mouse_r = new Gdk.Rectangle ();
			mouse_r.X = mouse[0] - vx;
			mouse_r.Y = mouse[1] - vy;
			mouse_r.Width = visible[0];
			mouse_r.Height = visible[1];

			Gdk.Rectangle i;
			if (area.Intersect (mouse_r, out i))
				DrawMouseBox (mouse_r);
		}

		void
		DrawMouseBox (Gdk.Rectangle mouse)
		{
			backing.DrawRectangle (black, false, mouse);
		}

		protected override bool
		OnExposeEvent (Gdk.EventExpose ev)
		{
			Gdk.Rectangle r = ev.Area;

			// Update the backing store for the part of the image we need
			// to redraw, then copy it to the window.
			DrawArea (r);

			GdkWindow.DrawDrawable (white, backing, r.X, r.Y, r.X, r.Y, r.Width, r.Height);

			return true;
		}

		public void
		SetMouse (int mx, int my)
		{
			// If the mouse hasn't actually moved (usually triggered by moving
			// it around outside the window), don't do anything.
			if (mouse[0] == mx && mouse[1] == my)
				return;

			// We want to only redraw what we have to, so take a union of the
			// old and new mouse boxes, and use that for our redraw area
			Gdk.Rectangle oldMouse = new Gdk.Rectangle ();
			Gdk.Rectangle newMouse = new Gdk.Rectangle ();
			Gdk.Rectangle area;

			int vx = visible[0] / 2;
			int vy = visible[1] / 2;

			oldMouse.X = mouse[0] - vx - 1;
			oldMouse.Y = mouse[1] - vy - 1;
			newMouse.X = mx - vx - 1;
			newMouse.Y = my - vy - 1;

			oldMouse.Width  = newMouse.Width  = visible[0] + 2;
			oldMouse.Height = newMouse.Height = visible[1] + 2;

			mouse[0] = mx;
			mouse[1] = my;

			area = oldMouse.Union (newMouse);
			GdkWindow.InvalidateRect (area, true);
		}
	}

	class NavigationImage2 : Gtk.EventBox
	{
		Gtk.Image		image;
		NavigationWindow2	window;
		public PipelineDrawing	drawing;

		// Position and size of the NavigationWindow
		Gdk.Rectangle		position;

		// Size of the visible region within the NavigationWindow
		int[]			visible;

		// The ratio between navigation-window pixels and document pixels.
		float			scale;

		public
		NavigationImage2 ()
		{
			position = new Gdk.Rectangle ();
			visible = new int[2];

			Gdk.Pixbuf pixbuf = new Gdk.Pixbuf (null, "navigation.png");
			image = new Gtk.Image (pixbuf);
			Add (image);

			ShowAll ();
		}

		protected override bool
		OnButtonPressEvent (Gdk.EventButton ev)
		{
			if (drawing == null)
				return true;

			Gdk.Screen screen = GdkWindow.Screen;

			int mouse_x = (int) ev.XRoot;
			int mouse_y = (int) ev.YRoot;

			Gdk.Rectangle canvas = drawing.CanvasExtents;

			// Always use a 200px box for our navigation window
			float aspect = ((float) canvas.Height) / ((float) canvas.Width);
			position.Width  = 200;
			position.Height = (int) (200 * aspect);

			// Determine visible window extents
			Gdk.Rectangle vwin = drawing.DrawingExtents;
			float ratio = ((float) vwin.Width) / ((float) canvas.Width);
			aspect = ((float) vwin.Height) / ((float) vwin.Width);
			visible[0] = (int) (198 * ratio);
			visible[1] = (int) (visible[0] * aspect);

			// Clamp visible box size to be completely within our window
			if (visible[0] > position.Width - 3)
				visible[0] = position.Width - 3;
			if (visible[1] > position.Height - 3)
				visible[1] = position.Height - 3;

			// Create our window
			window = new NavigationWindow2 (position.Width, position.Height, visible[0], visible[1]);

			//position.X = GetWindowPosition (mouse_x, position.Width,  screen.Width);
			//position.Y = GetWindowPosition (mouse_y, position.Height, screen.Height);

			window.Move (position.X, position.Y);

			mouse_x -= position.X;
			mouse_y -= position.Y;
			ClampMouse (ref mouse_x, position.Width,  visible[0]);
			ClampMouse (ref mouse_y, position.Height, visible[1]);
			window.SetMouse (mouse_x, mouse_y);

			window.Show ();

			return true;
		}

		protected override bool
		OnButtonReleaseEvent (Gdk.EventButton ev)
		{
			window.Hide ();
			window = null;

			return true;
		}

		void
		ClampMouse (ref int mouse, int size, int visible)
		{
			int v = visible / 2;
			if (mouse < v)
				mouse = v + 1;
			else if (mouse + v >= size - 1)
				mouse = size - v - 2;
		}

		protected override bool
		OnMotionNotifyEvent (Gdk.EventMotion ev)
		{
			int mx = ((int) ev.XRoot) - position.X;
			int my = ((int) ev.YRoot) - position.Y;

			ClampMouse (ref mx, position.Width,  visible[0]);
			ClampMouse (ref my, position.Height, visible[1]);

			window.SetMouse (mx, my);
			return true;
		}
	}
}
