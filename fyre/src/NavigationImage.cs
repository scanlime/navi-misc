/*
 * NavigationImage.cs - Widget that handles the navigation image in the lower
 * 	right corner.
 *
 * Copyright (C) 2005 David Trowbridge
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
		Gdk.Pixmap	pixmap;
		Gdk.GC		white, black;
		int		width, height;

		void AllocGCs ()
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

		void DrawBackground ()
		{
			if (white == null)
				AllocGCs ();

			pixmap.DrawRectangle (white, true, 0, 0, width - 1, height - 1);
			pixmap.DrawRectangle (black, false, 0, 0, width - 1, height - 1);
		}

		public NavigationWindow (int w, int h) : base (Gtk.WindowType.Popup)
		{
			width = w;
			height = h;

			// Before we can access any gdk internals, we need
			// to realize the window.
			Realize ();

			Resize (w, h);

			pixmap = new Gdk.Pixmap (GdkWindow, w, h, -1);

			DrawBackground ();
		}

		protected override bool OnExposeEvent (Gdk.EventExpose ev)
		{
			Gdk.Rectangle r = ev.Area;
			GdkWindow.DrawDrawable (white, pixmap, r.X, r.Y, r.X, r.Y, r.Width, r.Height);
			return true;
		}
	}

	class NavigationImage : Gtk.EventBox
	{
		Gtk.Image		image;
		NavigationWindow	window;
		Gdk.Rectangle		position;

		public NavigationImage()
		{
			position = new Gdk.Rectangle ();

			Gdk.Pixbuf pixbuf = new Gdk.Pixbuf (null, "navigation.png");
			image = new Gtk.Image (pixbuf);
			Add (image);

			ShowAll ();
		}

		int GetWindowPosition (int mouse, int winsize, int screensize)
		{
			if (mouse - (winsize / 2) < 0)
				return 0;
			if (mouse + (winsize / 2) > screensize)
				return screensize - winsize;
			return mouse - (winsize / 2);
		}

		protected override bool OnButtonPressEvent (Gdk.EventButton ev)
		{
			Gdk.Screen screen = GdkWindow.Screen;

			int mouse_x = (int) ev.XRoot;
			int mouse_y = (int) ev.YRoot;

			// FIXME: replace this with document extents * 0.1
			position.Width  = 200;
			position.Height = 150;

			window = new NavigationWindow (position.Width, position.Height);

			position.X = GetWindowPosition (mouse_x, position.Width,  screen.Width);
			position.Y = GetWindowPosition (mouse_y, position.Height, screen.Height);

			window.Move (position.X, position.Y);
			window.Show ();

			return true;
		}

		protected override bool OnButtonReleaseEvent (Gdk.EventButton ev)
		{
			window.Hide ();
			window = null;

			return true;
		}

		void ClampMouse (ref int mouse, int size)
		{
			if (mouse < 0)
				mouse = 0;
			else if (mouse >= size)
				mouse = size - 1;
		}

		protected override bool OnMotionNotifyEvent (Gdk.EventMotion ev)
		{
			int mx = ((int) ev.XRoot) - position.X;
			int my = ((int) ev.YRoot) - position.Y;

			ClampMouse (ref mx, position.Width);
			ClampMouse (ref my, position.Height);

			// FIXME: Add drawing the boxey thing
			return true;
		}
	}

}
