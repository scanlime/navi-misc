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
		 * size of the drawing area
		 *
		 * The layout extents are the size of the drawing itself. This is
		 * maintained via the graph layout algorithms.
		 *
		 * Scrollbars are determined by the union of the drawing and the layout
		 * boxes.
		 */
		Gdk.Rectangle		drawing_extents;
		Gdk.Rectangle		layout_extents;

		Gdk.Cursor		hand_open_cursor;
		Gdk.Cursor		hand_closed_cursor;
		Gdk.Cursor		pointer_cursor;

		Gtk.EventBox		event_box;

		/* String data for our hand cursors. These have been taken pretty much
		 * verbatim from eog */
		static byte[] hand_open_data_bits =
		{
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x06, 0x00,
			0x60, 0x36, 0x00, 0x60, 0x36, 0x00, 0xc0, 0x36, 0x01, 0xc0, 0xb6, 0x01,
			0x80, 0xbf, 0x01, 0x98, 0xff, 0x01, 0xb8, 0xff, 0x00, 0xf0, 0xff, 0x00,
			0xe0, 0xff, 0x00, 0xe0, 0x7f, 0x00, 0xc0, 0x7f, 0x00, 0x80, 0x3f, 0x00,
			0x00, 0x3f, 0x00, 0x00, 0x3f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0
		};
		static byte[] hand_open_mask_bits =
		{
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x06, 0x00, 0x60, 0x3f, 0x00,
			0xf0, 0x7f, 0x00, 0xf0, 0x7f, 0x01, 0xe0, 0xff, 0x03, 0xe0, 0xff, 0x03,
			0xd8, 0xff, 0x03, 0xfc, 0xff, 0x03, 0xfc, 0xff, 0x01, 0xf8, 0xff, 0x01,
			0xf0, 0xff, 0x01, 0xf0, 0xff, 0x00, 0xe0, 0xff, 0x00, 0xc0, 0x7f, 0x00,
			0x80, 0x7f, 0x00, 0x80, 0x7f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
		};
		static byte[] hand_closed_data_bits =
		{
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x06, 0x00, 0x80, 0x3f, 0x00,
			0x80, 0xff, 0x00, 0x80, 0xff, 0x00, 0xb0, 0xff, 0x00, 0xf0, 0xff, 0x00,
			0xe0, 0xff, 0x00, 0xe0, 0x7f, 0x00, 0xc0, 0x7f, 0x00, 0x80, 0x3f, 0x00,
			0x00, 0x3f, 0x00, 0x00, 0x3f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
		};
		static byte[] hand_closed_mask_bits =
		{
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			0x00, 0x00, 0x00, 0x00, 0x06, 0x00, 0x80, 0x3f, 0x00, 0xc0, 0xff, 0x00,
			0xc0, 0xff, 0x01, 0xf0, 0xff, 0x01, 0xf8, 0xff, 0x01, 0xf8, 0xff, 0x01,
			0xf0, 0xff, 0x01, 0xf0, 0xff, 0x00, 0xe0, 0xff, 0x00, 0xc0, 0x7f, 0x00,
			0x80, 0x7f, 0x00, 0x80, 0x7f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
		};
		static string hand_open_data, hand_open_mask, hand_closed_data, hand_closed_mask;

		public
		PipelineDrawing (Glade.XML xml) : base ()
		{
			drawing_extents = new Gdk.Rectangle ();
			layout_extents = new Gdk.Rectangle ();

			// Initialize our cursors, if we need to
			if (hand_open_data == null) {
				/*
				hand_open_data = new string (hand_open_data_bits);
				hand_open_mask = new string (hand_open_mask_bits);
				hand_closed_data = new string (hand_open_data_bits);
				hand_closed_mask = new string (hand_open_mask_bits);
				*/
				pointer_cursor = new Gdk.Cursor (Gdk.CursorType.LeftPtr);
				/*
				hand_open_cursor = new Gdk.Cursor (FIXME);
				hand_closed_cursor = new Gdk.Cursor (FIXME);
				*/
			}

			// For now, initialize the layout extents to 400x400, centered at the origin
			layout_extents.X = -200;
			layout_extents.Y = -200;
			layout_extents.Width  = 400;
			layout_extents.Height = 400;

			drawing_extents.X = 0;
			drawing_extents.Y = 0;


			// Get a handle to our event box
			event_box = (Gtk.EventBox) xml.GetWidget ("pipeline_window");
			event_box.ButtonPressEvent   += new Gtk.ButtonPressEventHandler   (ButtonPressHandler);
			event_box.ButtonReleaseEvent += new Gtk.ButtonReleaseEventHandler (ButtonReleaseHandler);
			event_box.MotionNotifyEvent  += new Gtk.MotionNotifyEventHandler  (MotionNotifyHandler);
			event_box.LeaveNotifyEvent   += new Gtk.LeaveNotifyEventHandler   (LeaveNotifyHandler);

			Show ();
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

				// Temporarily set the range here. This will need to query
				// the main canvas for extents once that exists
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
