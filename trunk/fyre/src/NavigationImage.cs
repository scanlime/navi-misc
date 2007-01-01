/*
 * NavigationImage.cs - Widget that handles the navigation image in the lower
 * 	right corner.
 *
 * Fyre - a generic framework for computational art
 * Copyright (C) 2004-2007 Fyre Team (see AUTHORS)
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

namespace Fyre.Editor.Widgets
{

	class NavigationWindow : Gtk.Window
	{
		Gdk.Pixmap		background;
		Gdk.Pixmap		backing;
		Gdk.GC			white, black;
		Gdk.Rectangle		visible;

		public int		Width, Height;
		float			ratio;
		Gdk.Rectangle		mouse_box;
		Gdk.Rectangle		canvas;

		PipelineDrawing		drawing;
		Layout			layout;

		public
		NavigationWindow (PipelineDrawing drawing, Layout layout) : base (Gtk.WindowType.Popup)
		{
			this.drawing = drawing;
			this.layout = layout;
			canvas = drawing.CanvasExtents;

			// We want the navigation window to have a maximum of 200px in either
			// dimension, but still represent a scaled version of the document.
			// Since this is a complicated setup, split it into two functions here.
			if (canvas.Width > canvas.Height)
				CreateWithWidth ();
			else
				CreateWithHeight ();

			// Create our background and backing store
			background = new Gdk.Pixmap (GdkWindow, Width + 2, Height + 2, -1);
			backing    = new Gdk.Pixmap (GdkWindow, Width + 2, Height + 2, -1);

			AllocGCs ();
			DrawBackground ();

			mouse_box.X = -1;
		}

		void CreateWithWidth ()
		{
			float aspect = ((float) canvas.Width) / ((float) canvas.Height);
			Width = 200;
			Height = (int) (Width / aspect);

			// We need to realize before we can access any of the gdk internals,
			// like setting size/position.
			Realize ();

			// We set the actual Gtk.Window to be our size + 2 in each dimension,
			// for the black border around the edges.
			Resize (Width + 2, Height + 2);

			// Determine the size of the visible window.
			Gdk.Rectangle vwin = drawing.DrawingExtents;

			// Ratio between visible window and entire canvas
			ratio = ((float) vwin.Width) / ((float) canvas.Width);
			aspect = ((float) vwin.Width) / ((float) vwin.Height);
			visible.Width  = (int) (200 * ratio);
			visible.Height = (int) (visible.Width / aspect);
			// Clamp height.
			if (visible.Height > Height)
				visible.Height = Height;

			// Set this to the ratio between window size and canvas size
			ratio = ((float) canvas.Width) / 200.0f;
		}

		void CreateWithHeight ()
		{
			float aspect = ((float) canvas.Height) / ((float) canvas.Width);
			Height = 200;
			Width = (int) (Height / aspect);

			// We need to realize before we can access any of the gdk internals,
			// like setting size/position.
			Realize ();

			// We set the actual Gtk.Window to be our size + 2 in each dimension,
			// for the black border around the edges.
			Resize (Width + 2, Height + 2);

			// Determine the size of the visible window.
			Gdk.Rectangle vwin = drawing.DrawingExtents;

			// Ratio between visible window and entire canvas
			ratio = ((float) vwin.Height) / ((float) canvas.Height);
			aspect = ((float) vwin.Height) / ((float) vwin.Width);
			visible.Height  = (int) (200 * ratio);
			visible.Width = (int) (visible.Height / aspect);
			// Clamp width.
			if (visible.Width > Width)
				visible.Width = Width;

			// Set this to the ratio between window size and canvas size
			ratio = ((float) canvas.Height) / 200.0f;
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
			background.DrawRectangle (white, true,  1, 1, Width, Height);

			// Translate to our document origin, scale down
			System.Drawing.Graphics g = Gtk.DotNet.Graphics.FromDrawable (background);
			g.ResetTransform ();
			g.ScaleTransform (1 / ratio, 1 / ratio);
			g.TranslateTransform (-canvas.X, -canvas.Y);

			// Pipeline
			System.Drawing.Rectangle r = Util.GdkRectToSD (canvas);
			layout.Draw (g, r);

			// Border
			background.DrawRectangle (black, false, 0, 0, Width + 1, Height + 1);
		}

		void
		DrawArea (Gdk.Rectangle area)
		{
			// Update the backing store
			backing.DrawDrawable (white, background, area.X, area.Y, area.X, area.Y, area.Width + 1, area.Height + 1);

			if (mouse_box.X == -1)
				return;

			// If the area we want to redraw intersects with the mouse box, draw that.
			Gdk.Rectangle mouse = mouse_box;
			mouse.X += 1;
			mouse.Y += 1;

			Gdk.Rectangle i;
			if (area.Intersect (mouse, out i))
				DrawMouseBox ();
		}

		void
		DrawMouseBox ()
		{
			backing.DrawRectangle (black, false, mouse_box.X + 1, mouse_box.Y + 1, mouse_box.Width - 1, mouse_box.Height - 1);
		}

		protected override bool
		OnExposeEvent (Gdk.EventExpose ev)
		{
			Gdk.Rectangle r = ev.Area;

			// Copy the relevant area of the backing store onto the screen.
			DrawArea (r);
			GdkWindow.DrawDrawable (white, backing, r.X, r.Y, r.X, r.Y, r.Width + 2, r.Height + 2);

			return true;
		}

		public void
		SetMouse (int x, int y)
		{
			// These values have been transformed into window-space by the NavigationImage,
			// but we still need to clamp the visible box to be within our window.
			int hvx = visible.Width  / 2;
			int hvy = visible.Height / 2;

			if (x - hvx < 0)
				x = hvx;
			if (y - hvy < 0)
				y = hvy;

			if (x + hvx > Width - 1)
				x = Width - hvx;
			if (y + hvy > Height - 1)
				y = Height - hvy;

			Gdk.Rectangle new_mouse = visible;
			new_mouse.X = x - hvx;
			new_mouse.Y = y - hvy;

			// Make sure we don't go too far.
			if (new_mouse.X + new_mouse.Width > Width)
				new_mouse.X = Width - new_mouse.Width;
			if (new_mouse.Y + new_mouse.Height > Height)
				new_mouse.Y = Height - new_mouse.Height;

			// Move the visible window to the place indicated by the mouse.
			float mouse_x = new_mouse.X * ratio + canvas.X;
			float mouse_y = new_mouse.Y * ratio + canvas.Y;
			drawing.SetViewPosition (mouse_x, mouse_y);

			// Take a union of the old and new mouse boxes, so we can redraw only
			// what's necessary. This is probably a silly optimization given the
			// size of the navigation window, but it's not hard to do.
			Gdk.Rectangle area = mouse_box.Union (new_mouse);
			area.X -= 1;     area.Y -= 1;
			area.Width += 2; area.Height += 2;

			mouse_box = new_mouse;
			GdkWindow.InvalidateRect (area, true);
		}
	}

	class NavigationImage : Gtk.EventBox
	{
		Gtk.Image		image;
		NavigationWindow	window;
		int			win_x, win_y;
		public PipelineDrawing	Drawing;
		public Document		Document;

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

			window = new NavigationWindow (Drawing, Document.Layout);

			// Position the window such that the entire thing is always visible
			// on the screen, even if the navigation image is right next to
			// one of the edges of the screen. If possible, we stick the center
			// of the window on the mouse pointer.
			win_x = GetWindowPosition (mouse_x, window.Width,  screen.Width);
			win_y = GetWindowPosition (mouse_y, window.Height, screen.Height);
			window.Move (win_x, win_y);

			window.SetMouse (mouse_x - win_x, mouse_y - win_y);

			window.Show ();

			return true;
		}

		protected override bool
		OnButtonReleaseEvent (Gdk.EventButton ev)
		{
			// Hide and destroy our window
			window.Destroy ();
			window = null;

			// Reset the size of the document, now that the user has finished
			// moving the document around.
			Drawing.SetScrollbars ();

			// This is a complete hack, but for some reason the scrollbars
			// weren't redrawing after we close the navigation window.
			Drawing.RedrawScrollbars ();
			return true;
		}

		protected override bool
		OnMotionNotifyEvent (Gdk.EventMotion ev)
		{
			int mouse_x = (int) ev.XRoot;
			int mouse_y = (int) ev.YRoot;

			window.SetMouse (mouse_x - win_x, mouse_y - win_y);
			return true;
		}
	}

}
