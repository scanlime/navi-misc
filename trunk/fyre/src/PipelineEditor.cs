/*
 * PipelineEditor.cs - pipeline editor. At this moment, this also
 *	stands in as the main program
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

	public class PipelineEditor
	{
		// Plugin stuff
		PluginManager plugin_manager;

		// Widgets
		[Glade.Widget] Gtk.Window		toplevel;

		// Element list (left)
		Gtk.TreeStore				element_store;
		Gtk.TreeModelSort			sorted_store;
		Gtk.TreeViewColumn			column;
		[Glade.Widget] Gtk.TreeView		element_list;

		// Editor workspace (right)
		[Glade.Widget] Gtk.Frame		pipeline_window;
		PipelineDrawing				pipeline_drawing;
		[Glade.Widget] Gtk.Scrollbar		drawing_hscroll;
		[Glade.Widget] Gtk.Scrollbar		drawing_vscroll;

		// Current tooltip
		ElementTooltip				current_tooltip;
		uint					tooltip_timeout;
		Gdk.Rectangle				tip_rect;

		// Status bar
		[Glade.Widget] Gtk.Statusbar		statusbar;

		// Image browsing
		[Glade.Widget] Gtk.Image		navigation_image;
		[Glade.Widget] Gtk.EventBox		navigation_event;

		// D-n-D private data
		int					click_x, click_y;
		bool					dragging;
		bool					check_drag;

		static Gtk.TargetEntry[]		targets;
		static Gtk.TargetList			target_list;
		public static Gtk.TargetEntry[]		DragTargets
		{
			get {
				if (targets == null) {
					targets = new Gtk.TargetEntry[1];
					targets[0] = new Gtk.TargetEntry ("fyre element drag", Gtk.TargetFlags.App, 0);
					target_list = new Gtk.TargetList (targets);
				}
				return targets;
			}
		}

		public static void Main (string[] args)
		{
			new PipelineEditor (args);
		}

		public PipelineEditor (string[] args)
		{
			Gtk.Application.Init();

			Glade.XML gxml = new Glade.XML (null, "pipeline-editor.glade", "toplevel", null);
			gxml.Autoconnect (this);

			// Set up the major parts of the UI
			SetupElementList ();
			SetupDrawingCanvas ();
			SetupNavigationBox ();

			// These start out nulled
			current_tooltip = null;
			tooltip_timeout = 0;
			click_x = -1;
			click_y = -1;
			dragging = false;

			// Set up plugins directory
			plugin_manager = new PluginManager (Defines.PLUGINSDIR);
			foreach (System.Type t in plugin_manager.plugin_types)
				AddElementType (t);

			// Finally, run the application
			Gtk.Application.Run();
		}

		void SetupElementList ()
			{
			//                                         Icon                 Name             Type                  Tooltip Window
			element_store = new Gtk.TreeStore (typeof (Gdk.Pixbuf), typeof (string), typeof (System.Type), typeof (ElementTooltip));

			// We sort the element list in alphabetical order by name/category,
			// since we don't know what order plugins will be loaded in, and
			// alphabetical order makes sense to people.
			sorted_store = new Gtk.TreeModelSort (element_store);
			sorted_store.SetSortColumnId (1, Gtk.SortType.Ascending);

			// And set it as the model
			element_list.Model = sorted_store;

			// Create the column and the renderers */
			column = new Gtk.TreeViewColumn ();
			column.Title = "Elements";

			// Icon
			Gtk.CellRenderer pixbuf_renderer = new Gtk.CellRendererPixbuf ();
			column.PackStart (pixbuf_renderer, false);
			column.AddAttribute (pixbuf_renderer, "pixbuf", 0);

			// Name
			Gtk.CellRenderer text_renderer   = new Gtk.CellRendererText ();
			column.PackStart (text_renderer, true);
			column.AddAttribute (text_renderer, "text", 1);

			element_list.AppendColumn (column);

			// Set up drag-and-drop for our tree view
			Gtk.Drag.SourceSet (element_list, Gdk.ModifierType.Button1Mask, DragTargets, Gdk.DragAction.Copy);
		}

		void SetupDrawingCanvas ()
		{
			// create the canvas
			pipeline_drawing = new PipelineDrawing();
			pipeline_window.Add (pipeline_drawing);
			pipeline_drawing.Show ();

			pipeline_drawing.SetScrollAdjustments (drawing_hscroll.Adjustment, drawing_vscroll.Adjustment);

			// Set up drag-and-drop for the frame. This looks better than setting it
			// up for the drawing area, but it doesn't really affect structure
			Gtk.Drag.DestSet (pipeline_window, Gtk.DestDefaults.All, DragTargets, Gdk.DragAction.Copy);
		}

		void SetupNavigationBox ()
		{
			Gdk.Pixbuf pixbuf = new Gdk.Pixbuf (null, "navigation.png");
			navigation_image.Pixbuf = pixbuf;
		}

		void ElementListDragBegin (object o, Gtk.DragBeginArgs args)
		{
			Gtk.TreePath path;
			Gtk.TreeViewColumn column;
			int cell_x, cell_y;

			if (element_list.GetPathAtPos (click_x, click_y, out path, out column, out cell_x, out cell_y)) {
				/*
				Gdk.Pixmap pixmap = element_list.CreateRowDragIcon (path);
				Gdk.Rectangle rect = pixmap.VisibleRegion.Clipbox;
				Gdk.Pixbuf icon = new Gdk.Pixbuf (Gdk.Colorspace.Rgb, false, 8, rect.Width, rect.Height);
				icon.GetFromDrawable (pixmap, pixmap.Colormap, 0, 0, 0, 0, rect.Width, rect.Height);
				Gtk.Drag.SetIconPixbuf (args.Context, icon, click_x + 1, cell_y);
				*/

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

		void ElementListDragEnd (object o, Gtk.DragEndArgs args)
		{
			dragging = false;
			click_x = -1;
			click_y = -1;
		}

		void ElementListButtonPressEvent (object o, Gtk.ButtonPressEventArgs args)
		{
			if (current_tooltip != null) {
				current_tooltip.Hide ();
				current_tooltip = null;
			}
			if (tooltip_timeout != 0) {
				GLib.Source.Remove (tooltip_timeout);
				tooltip_timeout = 0;
			}
			click_x = (int) args.Event.X;
			click_y = (int) args.Event.Y;
			check_drag = false;
		}

		void ElementListButtonReleaseEvent (object o, Gtk.ButtonReleaseEventArgs args)
		{
			click_x = -1;
			click_y = -1;
			check_drag = false;
		}

		void ElementListMotionNotifyEvent (object o, Gtk.MotionNotifyEventArgs args)
		{
			Gdk.EventMotion ev = args.Event;
			Gtk.TreePath path;

			if (tooltip_timeout != 0) {
				if ((ev.Y > tip_rect.Y) && ((ev.Y - tip_rect.Height) < tip_rect.Y))
					return;
				// We've moved outside the current cell. Destroy the
				// timeout and create a new one
				GLib.Source.Remove (tooltip_timeout);
			}

			if (ev.State == Gdk.ModifierType.Button1Mask) {
				// If we're just continuing a drag, don't do anything
				if (dragging)
					return;
				// If we've already checked this drag, don't do anything either
				if (check_drag)
					return;
				// Check that we have a path
				if (element_list.GetPathAtPos ((int) ev.X, (int) ev.Y, out path) == false) {
					check_drag = true;
					return;
				}

				// Check that the path exists
				Gtk.TreeIter iter;
				if (sorted_store.GetIter (out iter, path) == false) {
					check_drag = true;
					return;
				}
				// Check that we're in a leaf node
				if (!sorted_store.IterParent (out iter, iter)) {
					check_drag = true;
					return;
				}
				// Start a drag
				dragging = true;
				Gtk.Drag.Begin (element_list, target_list, Gdk.DragAction.Copy, 1, ev);
				return;
			}

			if (element_list.GetPathAtPos ((int) ev.X, (int) ev.Y, out path)) {
				tip_rect = element_list.GetCellArea (path, column);
				tooltip_timeout = GLib.Timeout.Add (200, new GLib.TimeoutHandler(TooltipTimeout));
			}
		}

		bool TooltipTimeout ()
		{
			Gtk.TreePath path;
			Gtk.TreeIter iter;
			if (!element_list.GetPathAtPos (tip_rect.X, tip_rect.Y, out path))
				return false;
			sorted_store.GetIter (out iter, path);
			if (current_tooltip != null)
				current_tooltip.Hide ();
			current_tooltip = (ElementTooltip) sorted_store.GetValue (iter, 3);
			if (current_tooltip == null)
				return false;

			int x, y;
			toplevel.GetPosition (out x, out y);
			current_tooltip.Move (x + tip_rect.X + 160, y + tip_rect.Y + 80);
			current_tooltip.Show ();
			tooltip_timeout = 0;
			return false;
		}

		void ElementListLeaveNotifyEvent (object o, Gtk.LeaveNotifyEventArgs args)
		{
			if (tooltip_timeout != 0) {
				GLib.Source.Remove (tooltip_timeout);
				tooltip_timeout = 0;
			}
			if (current_tooltip != null) {
				current_tooltip.Hide ();
				current_tooltip = null;
			}
		}

		private void AddElementType (System.Type t)
		{
			object[] i = {};
			Element e = (Element) t.GetConstructor(System.Type.EmptyTypes).Invoke(i);
			ElementTooltip tt = new ElementTooltip (e);

			string name = e.Name ();
			string category = e.Category ();
			Gdk.Pixbuf pixbuf = e.Icon ();
			bool found = false;

			Gtk.TreeIter iter;
			if (element_store.GetIterFirst (out iter)) {
				do {
					string cat = (string) element_store.GetValue (iter, 1);
					if (cat.Equals (category)) {
						found = true;
						element_store.AppendValues (iter, pixbuf, name, t, tt);
					}
				} while (element_store.IterNext (ref iter));
			}
			if (!found) {
				iter = element_store.AppendValues (null, category);
				element_store.AppendValues (iter, pixbuf, name, t, tt);
				element_list.ExpandAll ();
			}
		}

		// Event handlers - most of these come from the glade file
		// Window events
		public void OnDeleteEvent (object o, Gtk.DeleteEventArgs args)
		{
			Gtk.Application.Quit ();
			args.RetVal = true;
		}

		// Shared events - menus/toolbars
		public void OnNew (object o, System.EventArgs args)
		{
		}

		public void OnOpen (object o, System.EventArgs args)
		{
		}

		public void OnSave (object o, System.EventArgs args)
		{
		}

		public void OnCut (object o, System.EventArgs args)
		{
		}

		public void OnCopy (object o, System.EventArgs args)
		{
		}

		public void OnPaste (object o, System.EventArgs args)
		{
		}

		// 'File' Menu events
		public void OnMenuFileSaveAs (object o, System.EventArgs args)
		{
		}

		public void OnMenuFileQuit (object o, System.EventArgs args)
		{
			Gtk.Application.Quit ();
		}

		// 'Edit' Menu events
		public void OnMenuEditDelete (object o, System.EventArgs args)
		{
		}

		// 'View' Menu events
		public void OnMenuViewKeepClean (object o, System.EventArgs args)
		{
		}

		// 'Help' Menu events
		public void OnMenuHelpContents (object o, System.EventArgs args)
		{
		}
	}

}
