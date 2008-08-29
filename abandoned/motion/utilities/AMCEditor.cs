/*
 * AMCEditor.cs - An editor for AMC files, based loosely on the IPO
 *	interface in blender
 *
 * Copyright (C) 2005-2006 David Trowbridge
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

using System.Collections;

class AMCEditor
{
	[Glade.Widget] Gtk.Window		toplevel;
	[Glade.Widget] Gtk.TreeView		bone_list;
	[Glade.Widget] CurveEditor		curve_editor;
	[Glade.Widget] Gtk.EventBox		eventbox;
	[Glade.Widget] Gtk.ImageMenuItem	menu_save;
	[Glade.Widget] Gtk.ImageMenuItem	menu_saveas;

	Gtk.TreeStore				bone_store;
	AMC.File				AMCData;
	string					Filename;

	bool					modified;

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
				typeof (bool),		// shown on curve window
				typeof (bool),		// whether toggle is visible
				typeof (int),		// data index
				typeof (int));		// color index
		bone_list.Model = bone_store;

		// Create our text column
		Gtk.TreeViewColumn text_column  = new Gtk.TreeViewColumn ();
		Gtk.CellRendererText text_renderer = new Gtk.CellRendererText ();
		text_column.PackStart (text_renderer, true);
		text_column.AddAttribute (text_renderer, "markup",         0);
		text_column.AddAttribute (text_renderer, "foreground-gdk", 1);
		bone_list.AppendColumn (text_column);

		// Create our visible column
		Gtk.TreeViewColumn visible_column = new Gtk.TreeViewColumn ();
		Gtk.CellRendererToggle visible_renderer = new Gtk.CellRendererToggle ();
		visible_column.PackStart (visible_renderer, false);
		visible_column.AddAttribute (visible_renderer, "active",  2);
		visible_column.AddAttribute (visible_renderer, "visible", 3);
		visible_renderer.Activatable = true;
		visible_renderer.Toggled += new Gtk.ToggledHandler (RowToggled);
		bone_list.AppendColumn (visible_column);

		Filename = null;
		modified = false;

		curve_editor.bone_store = bone_store;

		UpdateToolbarSensitivity ();

		toplevel.ShowAll ();
	}

	void
	RowToggled (object o, Gtk.ToggledArgs args)
	{
		Gtk.TreeIter iter;
		bone_store.GetIter (out iter, new Gtk.TreePath (args.Path));
		bool t = (bool) bone_store.GetValue (iter, 2);
		bone_store.SetValue (iter, 2, !t);

		if (!t)
			curve_editor.enabled_bones.Add (args.Path);
		else
			curve_editor.enabled_bones.Remove (args.Path);

		curve_editor.VisibilityChanged ();
		eventbox.GrabFocus ();
	}

	static Gtk.Widget
	GladeCustomHandler (Glade.XML xml, string func_name, string name, string str1, string str2, int int1, int int2)
	{
		if (func_name == "CurveEditor")
			return new CurveEditor ();
		return null;
	}

	// Signal handlers
	public void
	OnOpen (object o, System.EventArgs args)
	{
		object[] responses = {
			Gtk.Stock.Cancel, Gtk.ResponseType.Cancel,
			Gtk.Stock.Open,   Gtk.ResponseType.Accept,
		};
		Gtk.FileChooserDialog fs = new Gtk.FileChooserDialog ("Open AMC...", null, Gtk.FileChooserAction.Open, responses);

		// File filter - *.amc
		Gtk.FileFilter ff1 = new Gtk.FileFilter ();
		ff1.AddPattern ("*.amc");
		ff1.Name = "Acclaim Motion Capture (*.amc)";
		fs.AddFilter (ff1);

		// File filter - all files
		Gtk.FileFilter ff2 = new Gtk.FileFilter ();
		ff2.AddPattern ("*");
		ff2.Name = "All Files";
		fs.AddFilter (ff2);

		Gtk.ResponseType response = (Gtk.ResponseType) fs.Run ();
		fs.Hide ();

		if (response == Gtk.ResponseType.Accept) {
			Filename = fs.Filename;
			AMCData = AMC.File.Load (Filename);

			bone_store.Clear ();

			if (AMCData == null) {
				// FIXME - pop up an error dialog
				System.Console.WriteLine ("Error loading {0}", Filename);
				Filename = null;
			} else {
				curve_editor.AMC = AMCData;
				SetTitle ();

				AMC.Frame f = (AMC.Frame) AMCData.frames[0];

				IDictionaryEnumerator e = f.GetEnumerator ();
				e.Reset ();
				while (e.MoveNext ()) {
					Gtk.TreeIter iter;
					iter = bone_store.AppendNode ();
					bone_store.SetValue (iter, 0, e.Key);
					bone_store.SetValue (iter, 1, new Gdk.Color (0x00, 0x00, 0x00));

					float[] s = (float[]) e.Value;
					for (int i = 0; i < s.Length; i++) {
						int color = ColorMap.AllocColor ();
						Gtk.TreeIter citer = bone_store.AppendNode (iter);
						bone_store.SetValue (citer, 0, System.String.Format ("<b>{0}</b>", i.ToString ()));
						bone_store.SetValue (citer, 1, ColorMap.Colors[color]);
						bone_store.SetValue (citer, 3, true);
						bone_store.SetValue (citer, 4, color);
						bone_store.SetValue (citer, 5, i);
					}
				}

				modified = false;
				UpdateToolbarSensitivity ();
			}
		}
		fs.Destroy ();
	}

	public void
	OnSave (object o, System.EventArgs args)
	{
		if (Filename == null) {
			object[] responses = {
				Gtk.Stock.Cancel, Gtk.ResponseType.Cancel,
				Gtk.Stock.Save,   Gtk.ResponseType.Accept,
			};

			Gtk.FileChooserDialog fs = new Gtk.FileChooserDialog ("Save...", null, Gtk.FileChooserAction.Save, responses);
			fs.DefaultResponse = Gtk.ResponseType.Accept;
			fs.CurrentName = Filename;

			Gtk.ResponseType response = (Gtk.ResponseType) fs.Run ();

			if (response == Gtk.ResponseType.Accept) {
				Filename = fs.Filename;
				AMCData.Save (Filename);
				UpdateToolbarSensitivity ();
			}
			fs.Destroy ();
		} else {
			AMCData.Save (Filename);
			modified = false;
		}
	}

	public void
	OnSaveAs (object o, System.EventArgs args)
	{
		object[] responses = {
			Gtk.Stock.Cancel, Gtk.ResponseType.Cancel,
			Gtk.Stock.Save,   Gtk.ResponseType.Accept,
		};

		Gtk.FileChooserDialog fs = new Gtk.FileChooserDialog ("Save As...", null, Gtk.FileChooserAction.Save, responses);
		fs.DefaultResponse = Gtk.ResponseType.Accept;
		fs.CurrentName = Filename;

		Gtk.ResponseType response = (Gtk.ResponseType) fs.Run ();

		if (response == Gtk.ResponseType.Accept) {
			Filename = fs.Filename;
			AMCData.Save (Filename);
			UpdateToolbarSensitivity ();
		}
		fs.Destroy ();
	}

	public void
	OnQuit (object o, System.EventArgs args)
	{
		Gtk.Application.Quit ();
	}

	void
	SetTitle ()
	{
		if (Filename == null) {
			toplevel.Title = "AMC Editor";
		} else {
			if (modified)
				toplevel.Title = System.IO.Path.GetFileName (Filename) + "*";
			else
				toplevel.Title = System.IO.Path.GetFileName (Filename);
		}
	}

	void
	UpdateToolbarSensitivity ()
	{
		menu_save.Sensitive = modified;
		menu_saveas.Sensitive = (Filename != null);
	}

	public void
	CurveButtonPressEvent (object o, Gtk.ButtonPressEventArgs args)
	{
		eventbox.GrabFocus ();
		curve_editor.ButtonPress (args.Event);
	}

	public void
	CurveButtonReleaseEvent (object o, Gtk.ButtonReleaseEventArgs args)
	{
		curve_editor.ButtonRelease (args.Event);
	}

	public void
	CurveMotionNotifyEvent (object o, Gtk.MotionNotifyEventArgs args)
	{
		curve_editor.MotionNotify (args.Event);
	}

	public void
	CurveKeyPressEvent (object o, Gtk.KeyPressEventArgs args)
	{
		curve_editor.KeyPress (args.Event);
	}
}
