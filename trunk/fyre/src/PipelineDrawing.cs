/*
 * PipelineDrawing.cs - Gtk.DrawingArea subclass that handles drawing of
 *	a Fyre pipeline
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
	public enum
	DrawingDragType
	{
		None,
		Document,
		Element,
		PadConnection,
	};

	static class Cursor
	{
		/*
		 * We use a bunch of different cursors here:
		 *
		 * Open hand: normal cursor when the mouse is just over the background
		 * Closed hand: used when dragging the document canvas around
		 * Pointer: cursor when the mouse is hovering over an element
		 * Fleur: cursor when moving an element
		 * Plus: cursor when hovering over a pad
		 * Cross: cursor when dragging out a new edge
		 */
		static Gdk.Cursor		hand_open;
		static Gdk.Cursor		hand_closed;
		static Gdk.Cursor		pointer;
		static Gdk.Cursor		fleur;
		static Gdk.Cursor		plus;
		static Gdk.Cursor		cross;

		public static Gdk.Cursor	HandOpen
		{
			get {
				if (hand_open == null)
					hand_open = CreateCursor ("hand-open-data.png", "hand-open-mask.png", 20, 20, 10, 10);
				return hand_open;
			}
		}

		public static Gdk.Cursor	HandClosed
		{
			get {
				if (hand_closed == null)
					hand_closed = CreateCursor ("hand-closed-data.png", "hand-closed-mask.png", 20, 20, 10, 10);
				return hand_closed;
			}
		}

		public static Gdk.Cursor	Pointer
		{
			get {
				if (pointer == null)
					pointer = new Gdk.Cursor (Gdk.CursorType.LeftPtr);
				return pointer;
			}
		}

		public static Gdk.Cursor	Fleur
		{
			get {
				if (fleur == null)
					fleur = new Gdk.Cursor (Gdk.CursorType.Fleur);
				return fleur;
			}
		}

		public static Gdk.Cursor	Plus
		{
			get {
				if (plus == null)
					plus = new Gdk.Cursor (Gdk.CursorType.Plus);
				return plus;
			}
		}

		public static Gdk.Cursor	Cross
		{
			get {
				if (cross == null)
					cross = new Gdk.Cursor (Gdk.CursorType.Cross);
				return cross;
			}
		}

		static Gdk.Cursor
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
	};

	class PipelineDrawing : Gtk.DrawingArea
	{
		Gtk.Scrollbar		hscroll;
		Gtk.Scrollbar		vscroll;
		Gtk.Adjustment		hadj;
		Gtk.Adjustment		vadj;

		// Back buffer
		Gdk.Pixmap		backing;
		Gdk.Rectangle		visible_rect;

		// We use a 50ms timeout when we to redraw
		uint			redraw_timeout;
		uint			scrollbars_timeout;

		// Whether or not we should update the canvas sizes
		bool			update_sizes;

		Document		document;
		public Document		Document
		{
			get {
				return document;
			}
			set {
				document = value;

				// Attach our event handler - this queues redraws
				document.Layout.Changed += new System.EventHandler (LayoutChanged);
			}
		}
		public CommandManager	command_manager;

		/* The drawing extents are the size of our current drawing area. The
		 * position depends on the scrollbars, and the size is always the pixel
		 * size of the drawing area.
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
		public Gdk.Rectangle	CanvasExtents
		{
			get { return ExpandRect (Document.Layout.Extents, layout_buffer).Union (drawing_extents); }
		}

		// Size of the buffer of pixels around the items on the canvas, giving
		// the user something to grab on to if they want to move the diagram.
		static int		layout_buffer = 50;

		// We can't get events on a drawing area, so we have an event box on top
		// of it.
		Gtk.EventBox		event_box;

		// Internal data to keep track of dragging
		DrawingDragType		dragging;
		int			drag_x, drag_y;
		int			old_x, old_y;

		public
		PipelineDrawing (Glade.XML xml) : base ()
		{
			drawing_extents = new Gdk.Rectangle ();
			visible_rect = new Gdk.Rectangle ();

			drawing_extents.X = 0;
			drawing_extents.Y = 0;

			visible_rect.X = 0;
			visible_rect.Y = 0;

			dragging = DrawingDragType.None;

			// Get a handle to our event box and set event handlers
			event_box = (Gtk.EventBox) xml.GetWidget ("pipeline_window");
			event_box.ButtonPressEvent   += new Gtk.ButtonPressEventHandler   (ButtonPressHandler);
			event_box.ButtonReleaseEvent += new Gtk.ButtonReleaseEventHandler (ButtonReleaseHandler);
			event_box.MotionNotifyEvent  += new Gtk.MotionNotifyEventHandler  (MotionNotifyHandler);

			update_sizes = true;

			redraw_timeout = 0;
			scrollbars_timeout = 0;

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

		protected override bool
		OnExposeEvent (Gdk.EventExpose ev)
		{
			Gdk.Color white = new Gdk.Color (0xff, 0xff, 0xff);
			Gdk.Colormap.System.AllocColor (ref white, true, true);
			Gdk.GC gc = new Gdk.GC (backing);
			gc.Foreground = white;

			backing.DrawRectangle (gc, true, ev.Area);

			// Set the transform the graphics surface to reflect the window position.
			// Clipping is handled automatically by System.Drawing
			System.Drawing.Graphics g = Gtk.DotNet.Graphics.FromDrawable (backing);
			g.ResetTransform ();
			g.TranslateTransform ((float) -drawing_extents.X, (float) -drawing_extents.Y);

			System.Drawing.Rectangle re = new System.Drawing.Rectangle();
			re.X      = drawing_extents.X;
			re.Y      = drawing_extents.Y;
			re.Width  = drawing_extents.Width;
			re.Height = drawing_extents.Height;
			Document.Layout.Draw (g, re);

			Gdk.Rectangle r = ev.Area;
			GdkWindow.DrawDrawable (gc, backing, r.X, r.Y, r.X, r.Y, r.Width, r.Height);

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
			event_box.GdkWindow.Cursor = Cursor.HandOpen;

			// Create the backing store
			backing = new Gdk.Pixmap (GdkWindow, ev.Width, ev.Height, -1);

			visible_rect.Width  = ev.Width;
			visible_rect.Height = ev.Height;

			CanvasElement.Drawable = GdkWindow;

			return base.OnConfigureEvent (ev);
		}

		public void
		SetScrollbars ()
		{
			if (scrollbars_timeout == 0)
				scrollbars_timeout = GLib.Timeout.Add (50, new GLib.TimeoutHandler (SetScrollbarsTimeout));
		}

		bool
		SetScrollbarsTimeout ()
		{
			Gdk.Rectangle r = Document.Layout.Extents;
			Gdk.Rectangle size;

			if (r.Width == 0 && r.Height == 0) {
				size = drawing_extents;
			} else {
				Gdk.Rectangle exp_layout = ExpandRect (r, layout_buffer);
				size = exp_layout.Union (drawing_extents);
			}

			hadj.Lower = size.X;
			vadj.Lower = size.Y;

			hadj.Upper = size.X + size.Width;
			vadj.Upper = size.Y + size.Height;

			hadj.PageIncrement = drawing_extents.Width  / 2;
			vadj.PageIncrement = drawing_extents.Height / 2;
			hadj.PageSize = drawing_extents.Width;
			vadj.PageSize = drawing_extents.Height;

			scrollbars_timeout = 0;

			return false;
		}

		public void
		AddScrollbars (Gtk.Scrollbar hscroll, Gtk.Scrollbar vscroll)
		{
			this.hscroll = hscroll;
			this.vscroll = vscroll;
			hadj = hscroll.Adjustment;
			vadj = vscroll.Adjustment;

			hadj.StepIncrement = 10.0;
			vadj.StepIncrement = 10.0;

			hadj.ValueChanged += new System.EventHandler (HValueChanged);
			vadj.ValueChanged += new System.EventHandler (VValueChanged);

			SetScrollbars ();

			hadj.Value = Document.Layout.Extents.X - layout_buffer;
			vadj.Value = Document.Layout.Extents.Y - layout_buffer;
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
			Gtk.Widget widget = (Gtk.Widget) o;

			// See MotionNotifyHandler for an explaination of this
			int win_x, win_y;
			widget.GdkWindow.GetOrigin (out win_x, out win_y);

			int layout_x = ((int) ev.XRoot) - win_x + drawing_extents.X;
			int layout_y = ((int) ev.YRoot) - win_y + drawing_extents.Y;
			LayoutHover h = Document.Layout.GetHoverType (layout_x, layout_y);

			Document.Layout.DeselectAll ();

			if (h == LayoutHover.None) {
				if (ev.Button == 1) {
					// We're not mousing over anything. Drag the document around.
					drag_x = (int) ev.X;
					drag_y = (int) ev.Y;

					dragging = DrawingDragType.Document;

					event_box.GdkWindow.Cursor = Cursor.HandClosed;
				}
				if (ev.Button == 3) {
					// Pop up a context menu. This one is pretty simple, since there aren't
					// many actions you can perform in the general document context.
					Gtk.Menu context = new Gtk.Menu ();

					Gtk.MenuItem paste_item = new Gtk.ImageMenuItem (Gtk.Stock.Paste, null);
					context.Append (paste_item);
					context.ShowAll ();
					context.Popup (null, null, null, System.IntPtr.Zero, ev.Button, ev.Time);
				}
			}

			if (h == LayoutHover.Element) {
				if (ev.Button == 1) {

					if (ev.Type == Gdk.EventType.ButtonPress) {
						dragging = DrawingDragType.Element;

						// Select the element
						Document.Layout.SelectHoverElement ();

						// Grab the canvas element to get the start position if we
						// decide to start moving things
						System.Guid id = Document.Layout.GetHoverElement ();
						Element e = (Element) Document.Pipeline.element_store[id.ToString ()];
						CanvasElement ce = Document.Layout.Get (e);

						// Setup drag coordinates
						drag_x = (int) ev.X;
						drag_y = (int) ev.Y;
						old_x = ce.Position.X;
						old_y = ce.Position.Y;

						event_box.GdkWindow.Cursor = Cursor.Fleur;
					} else if (ev.Type == Gdk.EventType.TwoButtonPress) {
						// Make sure the element stays selected.
						Document.Layout.SelectHoverElement ();

						// Don't continue dragging.
						dragging = DrawingDragType.None;

						System.Guid id = Document.Layout.GetHoverElement ();
						Element e = (Element) Document.Pipeline.element_store[id.ToString ()];

						e.Edit (null);
					}
				}
				if (ev.Button == 3) {
					// Select the element
					Document.Layout.SelectHoverElement ();

					// Create a context menu for this element
					Gtk.Menu context = new Gtk.Menu ();

					Gtk.MenuItem cut_item        = new Gtk.ImageMenuItem (Gtk.Stock.Cut,        null);
					Gtk.MenuItem copy_item       = new Gtk.ImageMenuItem (Gtk.Stock.Copy,       null);
					Gtk.MenuItem delete_item     = new Gtk.ImageMenuItem (Gtk.Stock.Delete,     null);
					Gtk.MenuItem properties_item = new Gtk.ImageMenuItem (Gtk.Stock.Properties, null);

					Gtk.ImageMenuItem flip_item = new Gtk.ImageMenuItem ("Flip");
					flip_item.Image = new Gtk.Image (new Gdk.Pixbuf (null, "flip.png"));

					delete_item.Activated += new System.EventHandler (ContextDelete);
					properties_item.Activated += new System.EventHandler (ContextProperties);

					context.Append (flip_item);
					context.Append (new Gtk.SeparatorMenuItem ());
					context.Append (cut_item);
					context.Append (copy_item);
					context.Append (new Gtk.SeparatorMenuItem ());
					context.Append (delete_item);
					context.Append (new Gtk.SeparatorMenuItem ());
					context.Append (properties_item);
					context.ShowAll ();
					context.Popup (null, null, null, System.IntPtr.Zero, ev.Button, ev.Time);
				}
			}

			if (h == LayoutHover.OutputPad && ev.Button == 1) {
				dragging = DrawingDragType.PadConnection;

				Document.Layout.BeginConnection ();
			}
		}

		void
		ContextProperties (object o, System.EventArgs args)
		{
			System.Guid id = Document.Layout.GetHoverElement ();
			Element e = (Element) Document.Pipeline.element_store[id.ToString ()];

			e.Edit (null);
		}

		void
		ContextDelete (object o, System.EventArgs args)
		{
			// Figure out which element is selected
			System.Guid id = Document.Layout.GetSelectedElement ();
			Element e = (Element) Document.Pipeline.element_store[id.ToString ()];
			CanvasElement ce = Document.Layout.Get (e);

			// Grab the X and Y coordinates of it
			int x = ce.Position.X;
			int y = ce.Position.Y;

			// Pass the element name to the new command, and execute it
			Commands.Delete deletee = new Commands.Delete (e, x, y);

			command_manager.Do (deletee);
			Document.Layout.DeselectAll ();
		}

		void
		ButtonReleaseHandler (object o, Gtk.ButtonReleaseEventArgs args)
		{
			Gdk.EventButton ev = args.Event;
			Gtk.Widget widget = (Gtk.Widget) o;

			// See MotionNotifyHandler for an explaination of this
			int win_x, win_y;
			widget.GdkWindow.GetOrigin (out win_x, out win_y);

			int layout_x = ((int) ev.XRoot) - win_x + drawing_extents.X;
			int layout_y = ((int) ev.YRoot) - win_y + drawing_extents.Y;
			LayoutHover h = Document.Layout.GetHoverType (layout_x, layout_y);

			if (dragging == DrawingDragType.Element) {
				// To facilitate undo/redo, we turn the drag movements into a command
				// and execute it.  This makes the move official and undoable.
				System.Guid id = Document.Layout.GetHoverElement ();
				Element e = (Element) Document.Pipeline.element_store[id.ToString ()];
				CanvasElement ce = Document.Layout.Get (e);

				// We want to execute this command only if we actually moved the element.  Otherwise,
				// this is completely pointless, and only adds an unnecessary entry into the undo/redo
				// stack.
				if (!(old_x == ce.Position.X && old_y == ce.Position.Y))
				{
					Commands.Move movee = new Commands.Move (e, old_x, old_y, ce.Position.X, ce.Position.Y);
					command_manager.Do (movee);
				}
			}

			if (dragging == DrawingDragType.PadConnection) {
				if (h == LayoutHover.InputPad) {
					// FIXME - we need to check if we already have a connection for this input
					Commands.Connect connecte = new Commands.Connect (Document.Layout.GetConnection ());
					command_manager.Do (connecte);
				}

				Document.Layout.EndConnection ();
			}

			if (dragging != DrawingDragType.None) {
				dragging = DrawingDragType.None;

				switch (h) {
				case LayoutHover.Element:
				case LayoutHover.InputPad:
					event_box.GdkWindow.Cursor = Cursor.Pointer;
					break;
				case LayoutHover.OutputPad:
					event_box.GdkWindow.Cursor = Cursor.Plus;
					break;
				case LayoutHover.None:
					event_box.GdkWindow.Cursor = Cursor.HandOpen;
					break;
				}
			}
		}

		void
		MotionNotifyHandler (object o, Gtk.MotionNotifyEventArgs args)
		{
			Gdk.EventMotion ev = args.Event;
			Gtk.Widget widget = (Gtk.Widget) o;

			// For some reason, the numbers we get from ev.X & ev.Y when a button isn't
			// pressed act really strangely. I'm not sure if this is a problem with
			// gentoo's gtk/gtk-sharp packages or something upstream, but the root-based
			// numbers are right. Get the window-local values by subtracting the window's
			// origin.
			int win_x, win_y;
			widget.GdkWindow.GetOrigin (out win_x, out win_y);

			int evX = ((int) ev.XRoot) - win_x;
			int evY = ((int) ev.YRoot) - win_y;

			if (dragging == DrawingDragType.Document) {
				// Compute the offset from the last event we got, and move
				// our view appropriately.
				int offset_x = ((int) ev.X) - drag_x;
				int offset_y = ((int) ev.Y) - drag_y;

				if (offset_x > 0 && hadj.Value == hadj.Lower)
					hadj.Lower -= offset_x;
				if (offset_y > 0 && vadj.Value == vadj.Lower)
					vadj.Lower -= offset_y;

				SetViewPosition ((float) (drawing_extents.X - offset_x), (float) (drawing_extents.Y - offset_y));
				SetScrollbars ();

				// Set these so we'll get proper offsets next time there's an event.
				drag_x = (int) ev.X;
				drag_y = (int) ev.Y;
				return;
			}

			if (dragging == DrawingDragType.Element) {
				// Compute the offset from the last event we got, move the element
				int offset_x = ((int) ev.X) - drag_x;
				int offset_y = ((int) ev.Y) - drag_y;

				Document.Layout.MoveHoverElement (offset_x, offset_y);

				// Set these for the next event
				drag_x = (int) ev.X;
				drag_y = (int) ev.Y;
				return;
			}

			if (dragging == DrawingDragType.PadConnection) {
				int layout_x = evX + drawing_extents.X;
				int layout_y = evY + drawing_extents.Y;
				Document.Layout.MoveConnection (layout_x, layout_y);
				LayoutHover h = Document.Layout.GetHoverType (layout_x, layout_y);

				if (h == LayoutHover.InputPad)
					event_box.GdkWindow.Cursor = Cursor.Cross;
				else
					event_box.GdkWindow.Cursor = Cursor.Plus;
			}

			if (dragging == DrawingDragType.None) {
				int layout_x = evX + drawing_extents.X;
				int layout_y = evY + drawing_extents.Y;
				LayoutHover h = Document.Layout.GetHoverType (layout_x, layout_y);

				switch (h) {
				case LayoutHover.Element:
				case LayoutHover.InputPad:
					event_box.GdkWindow.Cursor = Cursor.Pointer;
					break;
				case LayoutHover.OutputPad:
					event_box.GdkWindow.Cursor = Cursor.Plus;
					break;
				case LayoutHover.None:
					event_box.GdkWindow.Cursor = Cursor.HandOpen;
					break;
				}
			}
		}

		public void
		SetViewPosition (float x, float y)
		{
			update_sizes = false;
			hadj.Value = x;
			vadj.Value = y;
			update_sizes = true;

			if (redraw_timeout == 0)
				redraw_timeout = GLib.Timeout.Add (50, new GLib.TimeoutHandler (RedrawTimeout));
		}

		public void
		Redraw ()
		{
			GdkWindow.InvalidateRect (visible_rect, false);
		}

		bool
		RedrawTimeout ()
		{
			GdkWindow.InvalidateRect (visible_rect, false);

			redraw_timeout = 0;

			return false;
		}

		void
		LayoutChanged (object o, System.EventArgs e)
		{
			Redraw ();
		}

		public void
		RedrawScrollbars ()
		{
			hscroll.GdkWindow.InvalidateRect (hscroll.Allocation, true);
			vscroll.GdkWindow.InvalidateRect (hscroll.Allocation, true);
			hscroll.GdkWindow.ProcessUpdates (true);
			vscroll.GdkWindow.ProcessUpdates (true);
		}
	}

}
