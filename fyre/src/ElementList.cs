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
		private Gtk.TreeView		element_view;
		private Gtk.TreeViewColumn	column;
		private Gtk.TreeStore		element_store;
		private Gtk.TreeModelSort	sorted_store;

		private ElementList () : base ()
		{
			// Tree View
			element_view = new Gtk.TreeView ();
			element_view.Show ();

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

		public static ElementList GladeCustomHandler (Glade.XML xml, string func_name, string name, string str1, string str2, int int1, int int2)
		{
			return new ElementList ();
		}
	}

}
