/*
 * Main.cs - the main program
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

using System;
using Gtk;
using Glade;

public class PipelineEditor
{
	/* Plugin stuff */
	PluginManager plugin_manager;

	/* Widgets */
	/* Element list (left) */
	private Gtk.TreeStore		element_store;
	[Widget] Gtk.TreeView		element_list;

	/* Editor workspace (right) */
	[Widget] Gtk.ScrolledWindow	pipeline_window;
	[Widget] Gtk.DrawingArea	pipeline_drawing;

	public static void Main (string[] args)
	{
		new PipelineEditor (args);
	}

	public PipelineEditor (string[] args)
	{
		Application.Init();

		Glade.XML gxml = new Glade.XML (null, "pipeline-editor.glade", "window1", null);
		gxml.Autoconnect (this);

		/* Do all the setup for the element tree view */
		this.element_store = new Gtk.TreeStore (typeof (Gdk.Pixbuf), typeof (string), typeof (Type));
		element_list.Model = this.element_store;

		Gtk.CellRenderer pixbuf_renderer = new Gtk.CellRendererPixbuf ();
		Gtk.CellRenderer text_renderer   = new Gtk.CellRendererText ();
		Gtk.TreeViewColumn column = new Gtk.TreeViewColumn ();

		column.Title = "Elements";
		column.PackStart (pixbuf_renderer, false);
		column.AddAttribute (pixbuf_renderer, "pixbuf", 0);
		column.PackStart (text_renderer, true);
		column.AddAttribute (text_renderer, "text", 1);

		element_list.AppendColumn (column);

		/* Set up plugins directory */
		plugin_manager = new PluginManager ("/usr/share/fyre/2.0");
		foreach (Type t in plugin_manager.plugin_types) {
			object[] i = {};
			Element e = (Element) t.GetConstructor(Type.EmptyTypes).Invoke(i);

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
						element_store.AppendValues (iter, pixbuf, name, t);
					}
				} while (element_store.IterNext (ref iter));
			}
			if (!found) {
				iter = element_store.AppendValues (null, category);
				element_store.AppendValues (iter, pixbuf, name, t);
				element_list.ExpandAll ();
			}
		}

		/* Finally, run the application */
		Application.Run();
        }

	/* Event handlers - most of these come from the glade file */
	/* Window events */
	public void OnDeleteEvent (object o, DeleteEventArgs args)
	{
		Application.Quit ();
		args.RetVal = true;
	}

	/* Shared events - menus/toolbars */
	public void OnNew (object o, EventArgs args)
	{
	}

	public void OnOpen (object o, EventArgs args)
	{
	}

	public void OnSave (object o, EventArgs args)
	{
	}

	public void OnCut (object o, EventArgs args)
	{
	}

	public void OnCopy (object o, EventArgs args)
	{
	}

	public void OnPaste (object o, EventArgs args)
	{
	}

	/* 'File' Menu events */
	public void OnMenuFileSaveAs (object o, EventArgs args)
	{
	}

	public void OnMenuFileQuit (object o, EventArgs args)
	{
		Application.Quit ();
	}

	/* 'Edit' Menu events */
	public void OnMenuEditDelete (object o, EventArgs args)
	{
	}
}
