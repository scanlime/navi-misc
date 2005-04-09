/*
 * PipelineDrawing.cs - Gtk.DrawingArea subclass that handles drawing of
 *	a Fyre pipeline
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
	class PipelineDrawing : Gtk.DrawingArea
	{
		Gtk.Adjustment		hadj;
		Gtk.Adjustment		vadj;

		// Whether or not we should update the canvas sizes
		bool			update_sizes;

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
		public Gdk.Rectangle	DrawingExtents
		{
			get { return drawing_extents; }
		}
		Gdk.Rectangle		layout_extents;
		public Gdk.Rectangle	CanvasExtents
		{
			get { return ExpandRect (layout_extents, layout_buffer).Union (drawing_extents); }
		}

		// Size of the buffer of pixels around the items on the canvas, giving
		// the user something to grab on to if they want to move the diagram.
		static int		layout_buffer = 50;

		// We can't get events on a drawing area, so we have an event box on top
		// of it.
		Gtk.EventBox		event_box;

		// Internal data to keep track of dragging
		bool			dragging;
		int			drag_x, drag_y;

		/*
		 * We use a bunch of different cursors here:
		 *
		 * Open hand: normal cursor when the mouse is just over the background
		 * Closed hand: used when dragging the document canvas around
		 * Pointer: cursor when the mouse is hovering over an element
		 * Fleur: cursor when moving an element
		 * FIXME: cursor when hovering over a pad
		 * FIXME: cursor when dragging out a new edge
		 */
		Gdk.Cursor hand_open_cursor;
		Gdk.Cursor HandOpenCursor
		{
			get {
				if (hand_open_cursor == null)
					hand_open_cursor = CreateCursor ("hand-open-data.png", "hand-open-mask.png", 20, 20, 10, 10);
				return hand_open_cursor;
			}
		}
		Gdk.Cursor hand_closed_cursor;
		Gdk.Cursor HandClosedCursor
		{
			get {
				if (hand_closed_cursor == null)
					hand_closed_cursor = CreateCursor ("hand-closed-data.png", "hand-closed-mask.png", 20, 20, 10, 10);
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
		Gdk.Cursor fleur_cursor;
		Gdk.Cursor FleurCursor
		{
			get {
				if (fleur_cursor == null)
					fleur_cursor = new Gdk.Cursor (Gdk.CursorType.Fleur);
				return fleur_cursor;
			}
		}

		public
		PipelineDrawing (Glade.XML xml) : base ()
		{
			drawing_extents = new Gdk.Rectangle ();
			layout_extents = new Gdk.Rectangle ();

			// For now, initialize the layout extents to 300x300, centered at the origin
			layout_extents.X = -150;
			layout_extents.Y = -150;
			layout_extents.Width  = 300;
			layout_extents.Height = 300;

			drawing_extents.X = 0;
			drawing_extents.Y = 0;

			// Get a handle to our event box and set event handlers
			event_box = (Gtk.EventBox) xml.GetWidget ("pipeline_window");
			event_box.ButtonPressEvent   += new Gtk.ButtonPressEventHandler   (ButtonPressHandler);
			event_box.ButtonReleaseEvent += new Gtk.ButtonReleaseEventHandler (ButtonReleaseHandler);
			event_box.MotionNotifyEvent  += new Gtk.MotionNotifyEventHandler  (MotionNotifyHandler);
			event_box.LeaveNotifyEvent   += new Gtk.LeaveNotifyEventHandler   (LeaveNotifyHandler);

			update_sizes = true;

			Show ();
		}

		Gdk.Rectangle
		ExpandRect (Gdk.Rectangle source, int pixels)
		{
			Gdk.Rectangle ret = new Gdk.Rectangle ();
			ret.X = source.X - pixels;
			ret.Y = source.Y - pixels;
			ret.Width  = source.Width  + pixels*2;
			ret.Height = source.Height + pixels*2;
			return ret;
		}

		Gdk.Cursor
		CreateCursor (string data, string mask, int width, int height, int x_hotspot, int y_hotspot)
		{
			Gdk.Pixbuf data_pixbuf = new Gdk.Pixbuf (null, data);
			Gdk.Pixbuf mask_pixbuf = new Gdk.Pixbuf (null, mask);
			Gdk.Pixmap data_pixmap = new Gdk.Pixmap (null, width, height, 1);
			Gdk.Pixmap mask_pixmap = new Gdk.Pixmap (null, width, height, 1);
			data_pixbuf.RenderThresholdAlpha (data_pixmap, 0, 0, 0, 0, width, height, 1);
			mask_pixbuf.RenderThresholdAlpha (mask_pixmap, 0, 0, 0, 0, width, height, 1);

			Gdk.Color fore = new Gdk.Color (0xff, 0xff, 0xff);
			Gdk.Color back = new Gdk.Color (0x00, 0x00, 0x00);

			return new Gdk.Cursor (data_pixmap, mask_pixmap, fore, back, x_hotspot, y_hotspot);
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
			drawing_extents.Width = ev.Width;
			drawing_extents.Height = ev.Height;

			if (hadj != null)
				SetScrollbars ();

			// Make sure our cursor is set
			event_box.GdkWindow.Cursor = HandOpenCursor;


			return base.OnConfigureEvent (ev);
		}

		public void
		SetScrollbars ()
		{
			Gdk.Rectangle exp_layout = ExpandRect (layout_extents, layout_buffer);
			Gdk.Rectangle size = exp_layout.Union (drawing_extents);

			hadj.Lower = size.X;
			vadj.Lower = size.Y;

			hadj.Upper = size.X + size.Width;
			vadj.Upper = size.Y + size.Height;

			hadj.PageIncrement = drawing_extents.Width  / 2;
			vadj.PageIncrement = drawing_extents.Height / 2;
			hadj.PageSize = drawing_extents.Width;
			vadj.PageSize = drawing_extents.Height;
		}

		protected override void
		OnSetScrollAdjustments (Gtk.Adjustment hadj, Gtk.Adjustment vadj)
		{
			hadj.StepIncrement = 10.0;
			vadj.StepIncrement = 10.0;

			this.hadj = hadj;
			this.vadj = vadj;

			hadj.ValueChanged += new System.EventHandler (HValueChanged);
			vadj.ValueChanged += new System.EventHandler (VValueChanged);

			SetScrollbars ();

			hadj.Value = layout_extents.X - layout_buffer;
			vadj.Value = layout_extents.Y - layout_buffer;
		}

		void
		HValueChanged (object o, System.EventArgs e)
		{
			Gtk.Adjustment a = (Gtk.Adjustment) o;
			drawing_extents.X = (int) a.Value;
			if (update_sizes)
				SetScrollbars ();
		}

		void
		VValueChanged (object o, System.EventArgs e)
		{
			Gtk.Adjustment a = (Gtk.Adjustment) o;
			drawing_extents.Y = (int) a.Value;
			if (update_sizes)
				SetScrollbars ();
		}

		void
		ButtonPressHandler (object o, Gtk.ButtonPressEventArgs args)
		{
			Gdk.EventButton ev = args.Event;

			drag_x = (int) ev.X;
			drag_y = (int) ev.Y;

			dragging = true;

			event_box.GdkWindow.Cursor = HandClosedCursor;
		}

		void
		ButtonReleaseHandler (object o, Gtk.ButtonReleaseEventArgs args)
		{
			dragging = false;
			event_box.GdkWindow.Cursor = HandOpenCursor;
		}

		void
		MotionNotifyHandler (object o, Gtk.MotionNotifyEventArgs args)
		{
			Gdk.EventMotion ev = args.Event;

			if (dragging && ev.State == Gdk.ModifierType.Button1Mask) {
				// Compute the offset from the last event we got, and move
				// our view appropriately.
				int offset_x = ((int) ev.X) - drag_x;
				int offset_y = ((int) ev.Y) - drag_y;

				drawing_extents.X -= offset_x;
				drawing_extents.Y -= offset_y;
				SetScrollbars ();
				hadj.Value = drawing_extents.X;
				vadj.Value = drawing_extents.Y;

				// Set these so we'll get proper offsets next time there's
				// an event.
				drag_x = (int) ev.X;
				drag_y = (int) ev.Y;
			}
		}

		void
		LeaveNotifyHandler (object o, Gtk.LeaveNotifyEventArgs args)
		{
		}

		public void
		SetViewPosition (float x, float y)
		{
			update_sizes = false;
			hadj.Value = x;
			vadj.Value = y;
			update_sizes = true;
		}
	}

}
