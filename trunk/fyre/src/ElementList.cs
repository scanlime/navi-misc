/*
 * ElementList.cs - Custom aggregate widget that handles our element
 *	list on the left side of the window.
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

	class ElementList : Gtk.EventBox
	{
		// Widget data
		private Gtk.TreeView		element_view;
		private Gtk.TreeViewColumn	column;
		private Gtk.TreeStore		element_store;
		private Gtk.TreeModelSort	sorted_store;

		// D-n-D data
		int				click_x, click_y;
		bool				dragging;
		bool				check_drag;

		// Tooltip data
		ElementTooltip			current_tooltip;
		uint				tooltip_timeout;
		Gdk.Rectangle			tip_rect;
		private Gtk.Window		toplevel;

		public ElementList (Gtk.Widget window) : base ()
		{
			SetupWidgets ();

			// Start these out nulled
			click_x = -1;
			click_y = -1;
			dragging = false;
			tooltip_timeout = 0;

			toplevel = (Gtk.Window) window;
		}

		void SetupWidgets ()
		{
			// Tree View
			element_view = new Gtk.TreeView ();
			element_view.Show ();
			Add (element_view);

			// Create the column and the renderers
			column = new Gtk.TreeViewColumn ();
			column.Title = "Elements";

			// Set up the data store
			element_store = new Gtk.TreeStore (
					typeof (Gdk.Pixbuf),		// Icon
					typeof (string),		// Name
					typeof (System.Type),		// Type
					typeof (ElementTooltip));	// Tooltip Window

			// We sort the element list in alphabetical order, since we have
			// no foreknowledge of what order the plugins will be loaded in,
			// and alphabetical order makes sense to people
			sorted_store = new Gtk.TreeModelSort (element_store);
			sorted_store.SetSortColumnId (1, Gtk.SortType.Ascending);

			// Use the sorted proxy as our model
			element_view.Model = sorted_store;

			// Icon
			Gtk.CellRenderer pixbuf_renderer = new Gtk.CellRendererPixbuf ();
			column.PackStart (pixbuf_renderer, false);
			column.AddAttribute (pixbuf_renderer, "pixbuf", 0);

			// Name
			Gtk.CellRenderer text_renderer = new Gtk.CellRendererText ();
			column.PackStart (text_renderer, true);
			column.AddAttribute (text_renderer, "text", 1);

			element_view.AppendColumn (column);

			// Set up drag-and-drop
			Gtk.TargetEntry[] targets = PipelineEditor.DragTargets;
			Gtk.Drag.SourceSet (element_view, Gdk.ModifierType.Button1Mask, targets, Gdk.DragAction.Copy);
		}

		void ViewDragBegin (object o, Gtk.DragBeginArgs args)
		{
			Gtk.TreePath		path;
			Gtk.TreeViewColumn	column;
			int			cell_x, cell_y;

			if (element_view.GetPathAtPos (click_x, click_y, out path, out column, out cell_x, out cell_y)) {
				Gdk.Pixmap pixmap = new Gdk.Pixmap (null, 200, 150, 24);

				Gdk.GC gc = new Gdk.GC (pixmap);
				Gdk.Color white = new Gdk.Color (0xff, 0xff, 0xff);
				Gdk.Colormap.System.AllocColor (ref white, true, true);
				gc.Foreground = white;

				pixmap.DrawRectangle (gc, true, 0, 0, 200, 150);
				Gdk.Pixbuf icon = new Gdk.Pixbuf (Gdk.Colorspace.Rgb, false, 8, 200, 150);
				icon.GetFromDrawable (pixmap, pixmap.Colormap, 0, 0, 0, 0, 200, 150);
				Gtk.Drag.SetIconPixbuf (args.Context, icon, click_x + 1, cell_y);
			}
		}

		void ViewDragEnd (object o, Gtk.DragEndArgs args)
		{
			dragging = false;
			click_x = -1;
			click_y = -1;
		}

		protected override bool OnButtonPressEvent (Gdk.EventButton ev)
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

			return true;
		}

		protected override bool OnButtonReleaseEvent (Gdk.EventButton ev)
		{
			click_x = -1;
			click_y = -1;
			check_drag = false;

			return true;
		}

		protected override bool OnMotionNotifyEvent (Gdk.EventMotion ev)
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
				// If we're just continuing a drag, don't do anything
				if (dragging)
					return true;
				// If we've already checked this drag, don't do anything either
				if (check_drag)
					return true;
				// Check that we have a path
				if (element_view.GetPathAtPos ((int) ev.X, (int) ev.Y, out path) == false) {
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
				if (!sorted_store.IterParent (out iter, iter)) {
					check_drag = true;
					return true;
				}
				// Start a drag
				dragging = true;
				Gtk.Drag.Begin (element_view, PipelineEditor.TargetList, Gdk.DragAction.Copy, 1, ev);
				return true;
			}

			if (element_view.GetPathAtPos ((int) ev.X, (int) ev.Y, out path)) {
				tip_rect = element_view.GetCellArea (path, column);
				tooltip_timeout = GLib.Timeout.Add (200, new GLib.TimeoutHandler(TooltipTimeout));
			}
			return true;
		}

		protected override bool OnLeaveNotifyEvent (Gdk.EventCrossing ev)
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

		bool TooltipTimeout ()
		{
			Gtk.TreePath path;
			Gtk.TreeIter iter;

			// Check that we've got a valid path to show a tip on
			if (!element_view.GetPathAtPos (tip_rect.X, tip_rect.Y, out path))
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
	}

}
