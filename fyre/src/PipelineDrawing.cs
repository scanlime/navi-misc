/*
 * PipelineDrawing.cs - Gtk.DrawingArea subclass that handles drawing of
 *	a Fyre pipeline
 *
 * Fyre - rendering and interactive exploration of chaotic functions
 * Copyright (C) 2004-2005 David Trowbridge and Micah Dowty
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
	class PipelineDrawing : Gtk.DrawingArea
	{
		Gtk.Adjustment		hadj;
		Gtk.Adjustment		vadj;

		/* The drawing extents are the size of our current drawing area. The
		 * position depends on the scrollbars, and the size is always the pixel
		 * size of the drawing area.
		 *
		 * The layout extents are the size of the drawing itself. This is
		 * maintained via the graph layout algorithms.
		 *
		 * Scrollbars are determined by the union of the drawing and the layout
		 * boxes, plus some buffer around the layout (maybe 50px?) size to make
		 * it easier to drag the document for a bigger size.
		 */
		Gdk.Rectangle		drawing_extents;
		Gdk.Rectangle		layout_extents;

		Gtk.EventBox		event_box;

		// FIXME - Monodoc says that Gdk.Pixmap.CreateBitmapFromData is unimplemented,
		// but according to tberman it's not. However, we *are* getting garbage
		// cursors. At the moment, my best guess is that the string data type here
		// is doing some funky unicode conversion that's screwing up our XBM data.

		/* String data for our hand cursors. The XBM data here has been
		 * taken pretty much verbatim from eog. Remind me why these aren't
		 * standard GDK cursors?
		 */
		static string hand_open_data =
			"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x06\x00" +
			"\x60\x36\x00\x60\x36\x00\xc0\x36\x01\xc0\xb6\x01" +
			"\x80\xbf\x01\x98\xff\x01\xb8\xff\x00\xf0\xff\x00" +
			"\xe0\xff\x00\xe0\x7f\x00\xc0\x7f\x00\x80\x3f\x00" +
			"\x00\x3f\x00\x00\x3f\x00\x00\x00\x00\x00\x00\x00";
		static string hand_open_mask =
			"\x00\x00\x00\x00\x00\x00\x00\x06\x00\x60\x3f\x00" +
			"\xf0\x7f\x00\xf0\x7f\x01\xe0\xff\x03\xe0\xff\x03" +
			"\xd8\xff\x03\xfc\xff\x03\xfc\xff\x01\xf8\xff\x01" +
			"\xf0\xff\x01\xf0\xff\x00\xe0\xff\x00\xc0\x7f\x00" +
			"\x80\x7f\x00\x80\x7f\x00\x00\x00\x00\x00\x00\x00";
		static string hand_closed_data =
			"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00" +
			"\x00\x00\x00\x00\x00\x00\x00\x06\x00\x80\x3f\x00" +
			"\x80\xff\x00\x80\xff\x00\xb0\xff\x00\xf0\xff\x00" +
			"\xe0\xff\x00\xe0\x7f\x00\xc0\x7f\x00\x80\x3f\x00" +
			"\x00\x3f\x00\x00\x3f\x00\x00\x00\x00\x00\x00\x00";
		static string hand_closed_mask =
			"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00" +
			"\x00\x00\x00\x00\x06\x00\x80\x3f\x00\xc0\xff\x00" +
			"\xc0\xff\x01\xf0\xff\x01\xf8\xff\x01\xf8\xff\x01" +
			"\xf0\xff\x01\xf0\xff\x00\xe0\xff\x00\xc0\x7f\x00" +
			"\x80\x7f\x00\x80\x7f\x00\x00\x00\x00\x00\x00\x00";
		Gdk.Cursor hand_open_cursor;
		Gdk.Cursor HandOpenCursor
		{
			get {
				if (hand_open_cursor == null)
					hand_open_cursor   = CreateCursor (hand_open_data,   hand_open_mask);
				return hand_open_cursor;
			}
		}
		Gdk.Cursor hand_closed_cursor;
		Gdk.Cursor HandClosedCursor
		{
			get {
				if (hand_closed_cursor == null)
					hand_closed_cursor = CreateCursor (hand_closed_data, hand_closed_mask);
				return hand_closed_cursor;
			}
		}
		Gdk.Cursor pointer_cursor;
		Gdk.Cursor PointerCursor
		{
			get {
				if (pointer_cursor == null)
					pointer_cursor = new Gdk.Cursor (Gdk.CursorType.LeftPtr);
				return pointer_cursor;
			}
		}

		public
		PipelineDrawing (Glade.XML xml) : base ()
		{
			drawing_extents = new Gdk.Rectangle ();
			layout_extents = new Gdk.Rectangle ();

			// For now, initialize the layout extents to 400x400, centered at the origin
			layout_extents.X = -200;
			layout_extents.Y = -200;
			layout_extents.Width  = 400;
			layout_extents.Height = 400;

			drawing_extents.X = 0;
			drawing_extents.Y = 0;


			// Get a handle to our event box and set event handlers
			event_box = (Gtk.EventBox) xml.GetWidget ("pipeline_window");
			event_box.ButtonPressEvent   += new Gtk.ButtonPressEventHandler   (ButtonPressHandler);
			event_box.ButtonReleaseEvent += new Gtk.ButtonReleaseEventHandler (ButtonReleaseHandler);
			event_box.MotionNotifyEvent  += new Gtk.MotionNotifyEventHandler  (MotionNotifyHandler);
			event_box.LeaveNotifyEvent   += new Gtk.LeaveNotifyEventHandler   (LeaveNotifyHandler);

			Show ();
		}


		Gdk.Cursor
		CreateCursor (string data, string mask)
		{
			Gdk.Color fore = new Gdk.Color (0xff, 0xff, 0xff);
			Gdk.Color back = new Gdk.Color (0x00, 0x00, 0x00);
			Gdk.Pixmap source = Gdk.Pixmap.CreateBitmapFromData (GdkWindow, data, 20, 20);
			Gdk.Pixmap cmask  = Gdk.Pixmap.CreateBitmapFromData (GdkWindow, mask, 20, 20);

			return new Gdk.Cursor (source, cmask, fore, back, 10, 10);
		}

		protected override bool
		OnExposeEvent (Gdk.EventExpose ev)
		{
			Gdk.Drawable d = GdkWindow;
			Gdk.GC gc = Style.BackgroundGC (Gtk.StateType.Prelight);

			d.DrawRectangle (gc, true, ev.Area);
			return true;
		}

		protected override bool
		OnConfigureEvent (Gdk.EventConfigure ev)
		{
			if (hadj != null) {
				hadj.PageSize = ev.Width;
				vadj.PageSize = ev.Height;

				hadj.PageIncrement = ev.Width;
				vadj.PageIncrement = ev.Height;

				// FIXME - Temporarily set the range here. This will need to
				// set the document extents and modify scrollbars
				hadj.Upper = ev.Width * 8;
				vadj.Upper = ev.Height * 8;
			}

			drawing_extents.Width = ev.Width;
			drawing_extents.Height = ev.Height;

			return base.OnConfigureEvent (ev);
		}

		protected override void
		OnSetScrollAdjustments (Gtk.Adjustment hadj, Gtk.Adjustment vadj)
		{
			hadj.Lower         = 0.0;
			hadj.Upper         = 0.0;
			hadj.Value         = 0.0;
			hadj.PageIncrement = 0.0;
			hadj.PageSize      = 0.0;
			hadj.StepIncrement = 0.0;

			vadj.Lower         = 0.0;
			vadj.Upper         = 0.0;
			vadj.Value         = 0.0;
			vadj.PageIncrement = 0.0;
			vadj.PageSize      = 0.0;
			vadj.StepIncrement = 0.0;

			this.hadj = hadj;
			this.vadj = vadj;
		}

		void
		ButtonPressHandler (object o, Gtk.ButtonPressEventArgs args)
		{
		}

		void
		ButtonReleaseHandler (object o, Gtk.ButtonReleaseEventArgs args)
		{
		}

		void
		MotionNotifyHandler (object o, Gtk.MotionNotifyEventArgs args)
		{
		}

		void
		LeaveNotifyHandler (object o, Gtk.LeaveNotifyEventArgs args)
		{
		}
	}

}
