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

		protected override bool OnButtonPressEvent (Gdk.EventButton ev)
		{
			window = new Gtk.Window (Gtk.WindowType.Popup);
			Gtk.DrawingArea da = new Gtk.DrawingArea ();
			window.Add (da);
			window.DefaultWidth = 200;
			window.DefaultHeight = 150;

			int x = (int) ev.XRoot - 100;
			int y = (int) ev.YRoot - 75;
			window.Move (x, y);
			window.ShowAll ();

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
