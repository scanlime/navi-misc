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

		public NavigationImage()
		{
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
			window = new Gtk.Window (Gtk.WindowType.Popup);
			Gtk.DrawingArea da = new Gtk.DrawingArea ();
			window.Add (da);

			window.DefaultWidth = 0;
			window.DefaultHeight = 0;
			window.Show ();

			Gdk.Screen screen = window.GdkWindow.Screen;

			int mouse_x = (int) ev.XRoot;
			int mouse_y = (int) ev.YRoot;

			// FIXME: replace this with document extents * 0.1
			int width = 200;
			int height = 150;

			int win_x = GetWindowPosition (mouse_x, width, screen.Width);
			int win_y = GetWindowPosition (mouse_y, height, screen.Height);

			window.Move (win_x, win_y);
			window.Resize (width, height);

			return true;
		}

		protected override bool OnButtonReleaseEvent (Gdk.EventButton ev)
		{
			window.Hide ();
			window = null;

			return true;
		}
	}

}
