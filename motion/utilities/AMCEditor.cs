/*
 * AMCEditor.cs - An editor for AMC files, based loosely on the IPO
 *	interface in blender
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

class CellRendererColor : Gtk.CellRenderer
{
}

class AMCEditor
{
	[Glade.Widget] Gtk.Window		toplevel;
	[Glade.Widget] Gtk.ScrolledWindow	editor_swin;
	[Glade.Widget] Gtk.TreeView		bone_list;

	Gtk.TreeStore				bone_store;

	public static void
	Main (string[] args)
	{
		Gtk.Application.Init ();
		new AMCEditor ();
		Gtk.Application.Run ();
	}

	private
	AMCEditor ()
	{
		Glade.XML.SetCustomHandler (new Glade.XMLCustomWidgetHandler (GladeCustomHandler));
		Glade.XML gxml = new Glade.XML (null, "amc-editor.glade", "toplevel", null);
		gxml.Autoconnect (this);

		// Create the tree store
		bone_store = new Gtk.TreeStore (
				typeof (string),	// name
				typeof (Gdk.Color),	// color
				typeof (bool));		// shown
		bone_list.Model = bone_store;

		// Create our text column
		Gtk.TreeViewColumn text_column  = new Gtk.TreeViewColumn ();
		Gtk.CellRenderer text_renderer = new Gtk.CellRendererText ();
		text_column.PackStart (text_renderer, true);
		text_column.AddAttribute (text_renderer, "text", 0);
		bone_list.AppendColumn (text_column);

		// Create our color column
		Gtk.TreeViewColumn color_column = new Gtk.TreeViewColumn ();
		Gtk.CellRenderer color_renderer = new CellRendererColor ();
		color_column.PackStart (color_renderer, false);
		color_column.AddAttribute (color_renderer, "color",   1);
		color_column.AddAttribute (color_renderer, "enabled", 2);
	}

	static Gtk.Widget
	GladeCustomHandler (Glade.XML xml, string func_name, string name, string str1, string str2, int int1, int int2)
	{
		return null;
	}

	// Signal handlers
	public void
	OnOpen (object o, System.EventArgs args)
	{
	}

	public void
	OnSave (object o, System.EventArgs args)
	{
	}

	public void
	OnSaveAs (object o, System.EventArgs args)
	{
	}

	public void
	OnQuit (object o, System.EventArgs args)
	{
		Gtk.Application.Quit ();
	}
}
