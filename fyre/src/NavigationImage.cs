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

	class NavigationImage : Gtk.EventBox
	{
		Gtk.Image	image;
		Gtk.Window	window;
		Gdk.GC		white;
		Gdk.GC		black;

		Gdk.Pixmap	pixmap;

		public NavigationImage()
		{
			Gdk.Pixbuf pixbuf = new Gdk.Pixbuf (null, "navigation.png");
			image = new Gtk.Image (pixbuf);
			Add (image);

			ShowAll ();
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

		int GetWindowPosition (int mouse, int winsize, int screensize)
		{
			if (mouse - (winsize / 2) < 0)
				return 0;
			if (mouse + (winsize / 2) > screensize)
				return screensize - winsize;
			return mouse - (winsize / 2);
		}

		void CreatePixmap (Gdk.Drawable screen)
		{
			pixmap = new Gdk.Pixmap (screen, 200, 150, -1);

			if (white == null)
				AllocGCs ();

			pixmap.DrawRectangle (white, true, 0, 0, 199, 149);
			pixmap.DrawRectangle (black, false, 0, 0, 199, 149);
		}

		protected override bool OnButtonPressEvent (Gdk.EventButton ev)
		{
			window = new Gtk.Window (Gtk.WindowType.Popup);

			Gdk.Screen screen = GdkWindow.Screen;

			int mouse_x = (int) ev.XRoot;
			int mouse_y = (int) ev.YRoot;

			// FIXME: replace this with document extents * 0.1
			int width = 200;
			int height = 150;

			window.Realize ();
			CreatePixmap (window.GdkWindow);

			int win_x = GetWindowPosition (mouse_x, width, screen.Width);
			int win_y = GetWindowPosition (mouse_y, height, screen.Height);

			window.ExposeEvent += new Gtk.ExposeEventHandler (WindowExpose);
			window.MotionNotifyEvent += new Gtk.MotionNotifyEventHandler (WindowMotionNotify);

			window.Move (win_x, win_y);
			window.Resize (width, height);
			window.Show ();

			return true;
		}

		void WindowExpose (object o, Gtk.ExposeEventArgs args)
		{
			Gtk.Window w = (Gtk.Window) o;
			Gdk.Drawable d = w.GdkWindow;

			Gdk.Rectangle r = args.Event.Area;

			d.DrawDrawable (white, pixmap, r.X, r.Y, r.X, r.Y, r.Width, r.Height);
		}

		void WindowMotionNotify (object o, Gtk.MotionNotifyEventArgs args)
		{
		}

		protected override bool OnButtonReleaseEvent (Gdk.EventButton ev)
		{
			window.Hide ();
			window = null;

			pixmap = null;

			return true;
		}
	}

}
