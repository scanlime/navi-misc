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
		Gdk.Pixmap	background;
		Gdk.Pixmap	backing;
		Gdk.GC		white, black;
		int[]		size, visible, mouse;

		public NavigationWindow (int w, int h, int vw, int vh) : base (Gtk.WindowType.Popup)
		{
			size = new int[2];
			visible = new int[2];
			mouse = new int[2];

			// set window size
			size[0] = w;
			size[1] = h;

			// set visible size
			visible[0] = vw;
			visible[1] = vh;

			// Before we can access any gdk internals, we need
			// to realize the window.
			Realize ();

			Resize (w, h);

			background = new Gdk.Pixmap (GdkWindow, w, h, -1);
			backing = new Gdk.Pixmap (GdkWindow, w, h, -1);

			DrawBackground ();
		}

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

			background.DrawRectangle (white, true, 0, 0, size[0] - 1, size[1] - 1);
			background.DrawRectangle (black, false, 0, 0, size[0] - 1, size[1] - 1);
		}

		protected override bool OnExposeEvent (Gdk.EventExpose ev)
		{
			Gdk.Rectangle r = ev.Area;
			backing.DrawDrawable (white, background, r.X, r.Y, r.X, r.Y, r.Width, r.Height);
			// FIXME - mouse box
			GdkWindow.DrawDrawable (white, backing, r.X, r.Y, r.X, r.Y, r.Width, r.Height);
			return true;
		}

		public void SetMouse (int mx, int my)
		{
			// FIXME: trigger draw
		}

		public void DrawVisible (int mx, int my)
		{
			int vx =  visible[0] / 2;
			int vy =  visible[1] / 2;
			backing.DrawDrawable (white, background, 0, 0, 0, 0, size[0], size[1]);
			backing.DrawRectangle (black, false, mx - vx, my - vy, visible[0], visible[1]);
			GdkWindow.DrawDrawable (white, backing, 0, 0, 0, 0, size[0], size[1]);
		}
	}

	class NavigationImage : Gtk.EventBox
	{
		Gtk.Image		image;
		NavigationWindow	window;
		Gdk.Rectangle		position;
		int[]			visible;

		public NavigationImage()
		{
			position = new Gdk.Rectangle ();
			visible = new int[2];

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

			// FIXME: determine window extents
			visible[0] = 40;
			visible[1] = 30;

			window = new NavigationWindow (position.Width, position.Height, visible[0], visible[1]);

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

		void ClampMouse (ref int mouse, int size, int visible)
		{
			int v = visible / 2;
			if (mouse < v)
				mouse = v;
			else if (mouse >= size - v)
				mouse = size - v - 1;
		}

		protected override bool OnMotionNotifyEvent (Gdk.EventMotion ev)
		{
			int mx = ((int) ev.XRoot) - position.X;
			int my = ((int) ev.YRoot) - position.Y;

			ClampMouse (ref mx, position.Width, visible[0]);
			ClampMouse (ref my, position.Height, visible[1]);

			window.DrawVisible (mx, my);
			return true;
		}
	}

}
