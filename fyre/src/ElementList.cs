/*
 * ElementList.cs - Custom aggregate widget that handles our element
 *	list on the left side of the window.
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

	class ElementList : Gtk.TreeView
	{
		// Widget data
		private Gtk.TreeViewColumn	column;
		private Gtk.TreeStore		element_store;
		private Gtk.TreeModelSort	sorted_store;

		// D-n-D data
		public PipelineEditor		window;
		int				click_x, click_y;
		bool				dragging;
		bool				check_drag;
		string				drag_type;

		// Tooltip data
		ElementTooltip			current_tooltip;
		uint				tooltip_timeout;
		Gdk.Rectangle			tip_rect;
		private Gtk.Window		toplevel;

		public
		ElementList (Gtk.Widget window) : base ()
		{
			SetupWidgets ();

			// Start these out nulled
			click_x = -1;
			click_y = -1;
			dragging = false;
			tooltip_timeout = 0;

			toplevel = (Gtk.Window) window;

			ShowAll ();
		}

		void
		SetupWidgets ()
		{
			// Tree View data
			HeadersVisible = false;

			// Create the column and the renderers
			column = new Gtk.TreeViewColumn ();
			column.Title = "Elements";

			// Set up the data store
			element_store = new Gtk.TreeStore (
					typeof (Gdk.Pixbuf),		// Icon
					typeof (string),		// Name
					typeof (System.Type),		// Type
					typeof (ElementTooltip),	// Tooltip Window
					typeof (Element));		// Element

			// We sort the element list in alphabetical order, since we have
			// no foreknowledge of what order the plugins will be loaded in,
			// and alphabetical order makes sense to people
			sorted_store = new Gtk.TreeModelSort (element_store);
			sorted_store.SetSortColumnId (1, Gtk.SortType.Ascending);

			// Use the sorted proxy as our model
			Model = sorted_store;

			// Icon
			Gtk.CellRenderer pixbuf_renderer = new Gtk.CellRendererPixbuf ();
			column.PackStart (pixbuf_renderer, false);
			column.AddAttribute (pixbuf_renderer, "pixbuf", 0);

			// Name
			Gtk.CellRenderer text_renderer = new Gtk.CellRendererText ();
			column.PackStart (text_renderer, true);
			column.AddAttribute (text_renderer, "text", 1);

			AppendColumn (column);
		}

		protected override void
		OnDragBegin (Gdk.DragContext context)
		{
			Gtk.TreePath		path;
			Gtk.TreeViewColumn	column;
			int			cell_x, cell_y;

			if (!dragging) {
				Gdk.Drag.Abort (context, 0);
				return;
			}

			if (GetPathAtPos (click_x, click_y, out path, out column, out cell_x, out cell_y)) {
				// Set the drag data to be the name of the Element type. This allows
				// the element to be created by the factory when we drop.
				Gtk.TreeIter	iter;
				Element		e;

				sorted_store.GetIter (out iter, path);
				drag_type = (string) sorted_store.GetValue (iter, 1);
				e = (Element) sorted_store.GetValue (iter, 4);

				// Create a new CanvasElement for this Element.
				CanvasElement canvas_element = new CanvasElement (e);

				int w = canvas_element.Width;
				int h = canvas_element.Height;

				// Create a pixmap for the element.
				Gdk.Pixmap pixmap = new Gdk.Pixmap (GdkWindow, w, h, -1);

				// Get a C# Graphics object from the pixmap and draw the element on it.
				System.Drawing.Graphics graphics = Gtk.DotNet.Graphics.FromDrawable (pixmap);
				canvas_element.Draw (graphics);

				// Create a pixmap for the transparency mask.
				Gdk.Pixmap mask = new Gdk.Pixmap (null, w, h, 1);

				canvas_element.DrawMask (mask);

				Gtk.Drag.SetIconPixmap (context, GdkWindow.Colormap, pixmap, mask, 0, 0);
			}
		}

		protected override void
		OnDragEnd (Gdk.DragContext context)
		{
			dragging = false;
			click_x = -1;
			click_y = -1;
			drag_type = null;
		}

		protected override void
		OnDragDataGet (Gdk.DragContext context, Gtk.SelectionData data, uint info, uint time_)
		{
			// Copy the string we set in DragBegin into the drag data
			data.Set (data.Target, 8, System.Text.Encoding.UTF8.GetBytes (drag_type));
		}

		protected override bool
		OnButtonPressEvent (Gdk.EventButton ev)
		{
			if (current_tooltip != null) {
				current_tooltip.Hide ();
				current_tooltip = null;
			}
			if (tooltip_timeout != 0) {
				GLib.Source.Remove (tooltip_timeout);
				tooltip_timeout = 0;
			}
			click_x = (int) ev.X;
			click_y = (int) ev.Y;
			check_drag = false;
			dragging = false;

			return base.OnButtonPressEvent (ev);
		}

		protected override bool
		OnButtonReleaseEvent (Gdk.EventButton ev)
		{
			click_x = -1;
			click_y = -1;
			check_drag = false;
			dragging = false;

			return base.OnButtonReleaseEvent (ev);
		}

		protected override bool
		OnMotionNotifyEvent (Gdk.EventMotion ev)
		{
			Gtk.TreePath path;

			if (tooltip_timeout != 0) {
				if ((ev.Y > tip_rect.Y) && ((ev.Y - tip_rect.Height) < tip_rect.Y))
					return true;
				// We've moved outside the current cell. Destroy the
				// timeout and create a new one
				GLib.Source.Remove (tooltip_timeout);
				tooltip_timeout = 0;
			}

			if (ev.State == Gdk.ModifierType.Button1Mask) {
				// If we're just continuing a drag or we've already checked
				// the validity of the drag, don't do anything
				if (dragging || check_drag)
					return true;

				// Check that we have a path
				if (GetPathAtPos (click_x, click_y, out path) == false) {
					check_drag = true;
					return true;
				}

				// Check that the path exists
				Gtk.TreeIter iter;
				if (sorted_store.GetIter (out iter, path) == false) {
					check_drag = true;
					return true;
				}

				// Check that we're in a leaf node
				if (sorted_store.GetValue (iter, 3) == null) {
					check_drag = true;
					return true;
				}

				// Start a drag
				dragging = true;
				Gtk.Drag.Begin (this, window.TargetList, Gdk.DragAction.Copy, 1, ev);
				return true;
			}

			if (GetPathAtPos ((int) ev.X, (int) ev.Y, out path)) {
				tip_rect = GetCellArea (path, column);
				tooltip_timeout = GLib.Timeout.Add (200, new GLib.TimeoutHandler(TooltipTimeout));
			}
			return true;
		}

		protected override bool
		OnLeaveNotifyEvent (Gdk.EventCrossing ev)
		{
			if (tooltip_timeout != 0) {
				GLib.Source.Remove (tooltip_timeout);
				tooltip_timeout = 0;
			}
			if (current_tooltip != null) {
				current_tooltip.Hide ();
				current_tooltip = null;
			}
			return true;
		}

		bool
		TooltipTimeout ()
		{
			Gtk.TreePath path;
			Gtk.TreeIter iter;

			// Check that we've got a valid path to show a tip on
			if (!GetPathAtPos (tip_rect.X, tip_rect.Y, out path))
				return false;
			sorted_store.GetIter (out iter, path);
			if (current_tooltip != null)
				current_tooltip.Hide ();

			// Get the tooltip window, make sure it's valid
			current_tooltip = (ElementTooltip) sorted_store.GetValue (iter, 3);
			if (current_tooltip == null)
				return false;

			// Show it
			int x, y;
			toplevel.GetPosition (out x, out y);
			current_tooltip.Move (x + tip_rect.X + 160, y + tip_rect.Y + 80);
			current_tooltip.Show ();
			tooltip_timeout = 0;
			return false;
		}

		public void
		AddType (System.Type t)
		{
			ElementFactory factory = ElementFactory.Instance;
			Element e = factory.Create (t);
			ElementTooltip tt = new ElementTooltip (e);

			string name       = e.Name ();
			string category   = e.Category ();
			Gdk.Pixbuf pixbuf = e.Icon ();

			bool found = false;
			Gtk.TreeIter iter;

			// If we can find the category already in the list, use it.
			// If not, add the category before we store the element data.
			if (element_store.GetIterFirst (out iter)) {
				do {
					string cat = (string) element_store.GetValue (iter, 1);
					if (cat.Equals (category)) {
						found = true;
						element_store.AppendValues (iter, pixbuf, name, t, tt, e);
					}
				} while (element_store.IterNext (ref iter));
			}
			if (!found) {
				iter = element_store.AppendValues (null, category, null, null);
				element_store.AppendValues (iter, pixbuf, name, t, tt, e);
				ExpandAll ();
			}
		}
	}

}
