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

using System.Collections;

class ColorMap
{
	Hashtable		data;
}

class AMCFrame
{
	public Hashtable		data;

	public
	AMCFrame ()
	{
		data = new Hashtable ();
	}

	public void
	AddBone (string[] tokens)
	{
		string name = tokens[0];

		// hack to take care of trailing space
		if (tokens[tokens.Length - 1].Length == 0) {
			string[] values = new string[tokens.Length - 2];

			System.Array.Copy (tokens, 1, values, 0, values.Length - 1);
			data.Add (name, values);
		} else {
			string[] values = new string[tokens.Length - 1];

			System.Array.Copy (tokens, 1, values, 0, values.Length);
			data.Add (name, values);
		}
	}

	public IDictionaryEnumerator
	GetEnumerator ()
	{
		return data.GetEnumerator ();
	}
};

class AMCFile
{
	ArrayList		comments;
	public ArrayList	frames;

	protected
	AMCFile ()
	{
		comments = new ArrayList ();
		frames = new ArrayList ();
	}

	public static AMCFile
	Load (string filename)
	{
		AMCFile f = new AMCFile ();
		AMCFrame frame = null;

		System.IO.StreamReader file = System.IO.File.OpenText (filename);
		if (file == null)
			return null;

		string line;
		while ((line = file.ReadLine ()) != null) {
			// comments
			if (line[0] == '#' || line[0] == ':') {
				f.comments.Add (line);
				continue;
			}

			// are we starting a new frame?
			if (line.IndexOf (' ') == -1) {
				if (frame != null)
					f.frames.Add (frame);
				frame = new AMCFrame ();
				continue;
			}

			string[] tokens = line.Split (' ');
			frame.AddBone (tokens);
		}
		if (frame != null)
			f.frames.Add (frame);

		file.Close ();

		return f;
	}

	public void
	Save (string filename)
	{
		System.IO.StreamWriter file = System.IO.File.CreateText (filename);
		if (file == null)
			return;

		foreach (string line in comments)
			file.Write (System.String.Format ("{0}\n", line));

		file.Close ();
	}
}

class CurveEditor : Gtk.DrawingArea
{
	// Widget data
	Gtk.Adjustment		hadj;
	Gtk.Adjustment		vadj;

	// Drawing data
	Gdk.GC			grey_gc;
	Gdk.GC			black_gc;
	Gdk.Pixmap		back_buffer;
	Pango.Layout		layout;

	// Information about the AMC data
	int 			nframes;
	public Gtk.TreeStore	bone_store;
	public ArrayList	enabled_bones;

	// Window information
	int			width;
	int			height;

	// The AMC file
	AMCFile			amc;
	public AMCFile		AMC
	{
		set
		{
			amc = value;
			nframes = amc.frames.Count;
			CreateBackBuffer ();
			Draw ();

			hadj.Upper = nframes * 40;
			hadj.StepIncrement = 40;

			Gdk.Rectangle r = Allocation;
			r.X = 0; r.Y = 0;
			GdkWindow.InvalidateRect (r, true);
		}
	}

	public
	CurveEditor ()
	{
		enabled_bones = new ArrayList ();
	}

	void
	CreateGCs ()
	{
		Gdk.Color grey  = new Gdk.Color (0xde, 0xde, 0xde);
		Gdk.Color black = new Gdk.Color (0x00, 0x00, 0x00);

		grey_gc  = new Gdk.GC (GdkWindow);
		black_gc = new Gdk.GC (GdkWindow);

		GdkWindow.Colormap.AllocColor (ref grey,  true, true);
		GdkWindow.Colormap.AllocColor (ref black, true, true);

		grey_gc.Foreground  = grey;
		black_gc.Foreground = black;

		layout = CreatePangoLayout (null);
		layout.FontDescription = Pango.FontDescription.FromString ("Bitstream Vera Sans 7");
	}

	void
	CreateBackBuffer ()
	{
		back_buffer = new Gdk.Pixmap (GdkWindow, Allocation.Width, Allocation.Height);
	}

	void Draw ()
	{
		if (grey_gc == null)
			CreateGCs ();

		Gdk.Rectangle area = Allocation;
		area.X = 0; area.Y = 0;

		// Draw background
		back_buffer.DrawRectangle (grey_gc, true, area);

		if (amc == null)
			return;

		ArrayList bones = new ArrayList ();
		foreach (string p in enabled_bones) {
			Gtk.TreePath path = new Gtk.TreePath (p);
			Gtk.TreeIter iter;

			bone_store.GetIter (out iter, path);
			int num = System.Int32.Parse ((string) bone_store.GetValue (iter, 0));

			Gtk.TreeIter parent;
			bone_store.IterParent (out parent, iter);
			string s = (string) bone_store.GetValue (parent, 0);

			object[] pair = {
				num, s
			};
			bones.Add (pair);
		}

		// Draw frame lines and numbers
		for (int i = 0; i < amc.frames.Count; i++) {
			int pos = i * 40 + 20;
			if (IsVisible (pos)) {
				back_buffer.DrawLine (black_gc,
					(int) (pos - hadj.Value), 0,
					(int) (pos - hadj.Value), Allocation.Height - 20);

				layout.SetText (i.ToString ());
				int lw, lh;
				layout.GetPixelSize (out lw, out lh);

				back_buffer.DrawLayout (black_gc, (int) (pos - (lw / 2) - hadj.Value), Allocation.Height - 18, layout);
			}

			if (IsVisible (pos) || IsVisible (pos + 40)) {
				// Iterate through all the bones, draw those that are active.
				foreach (object[] pair in bones) {
					int angle = (int) pair[0];
					string name = (string) pair[1];

					if (i + 1 != amc.frames.Count) {
						AMCFrame frame1 = (AMCFrame) amc.frames[i];
						AMCFrame frame2 = (AMCFrame) amc.frames[i + 1];
						string s1 = ((string[]) (frame1.data[name]))[angle];
						string s2 = ((string[]) (frame2.data[name]))[angle];
						float val1 = (float) System.Double.Parse (s1);
						float val2 = (float) System.Double.Parse (s2);

						if (val1 < 0) val1 = 360.0f - val1;
						if (val2 < 0) val2 = 360.0f - val2;
						if (val1 > 360) val1 = val1 - 360.0f;
						if (val2 > 360) val2 = val2 - 360.0f;

						// draw box at current position and line to next position
						int height1 = FindValueHeight (val1);
						int height2 = FindValueHeight (val2);

						Gdk.Rectangle r = new Gdk.Rectangle ();
						r.X = (int) (pos - hadj.Value - 2);
						r.Y = height1 - 2;
						r.Width = 5;
						r.Height = 5;
						back_buffer.DrawRectangle (black_gc, true, r);

						// FIXME - color
						back_buffer.DrawLine (black_gc, (int) (pos - hadj.Value), height1, (int) (pos + 40 - hadj.Value), height2);
					} else {
						AMCFrame frame = (AMCFrame) amc.frames[i];
						string s = ((string[]) (frame.data[name]))[angle];
						float val = (float) System.Double.Parse (s);

						if (val < 0) val = 360.0f - val;
						if (val > 360) val = val - 360.0f;

						int height = FindValueHeight (val);
						Gdk.Rectangle r = new Gdk.Rectangle ();
						r.X = (int) (pos - hadj.Value - 2);
						r.Y = height - 2;
						r.Width = 5;
						r.Height = 5;
						back_buffer.DrawRectangle (black_gc, true, r);
					}
				}
			}
		}

		// Draw border between frame # and edit region
		back_buffer.DrawLine (black_gc, 0, Allocation.Height - 20, Allocation.Width, Allocation.Height - 20);
	}

	bool
	IsVisible (int x)
	{
		return ((x > hadj.Value) && (x < (hadj.Value + Allocation.Width)));
	}

	int FindValueHeight (float f)
	{
		float pc = f / 360.0f;
		float height = (Allocation.Height - 21) * pc;
		return (int) ((Allocation.Height - 21) - height);
	}

	protected override bool
	OnConfigureEvent (Gdk.EventConfigure ev)
	{
		hadj.PageSize = ev.Width;
		hadj.PageIncrement = ev.Width / 2;

		CreateBackBuffer ();
		Draw ();
		return true;
	}

	protected override bool
	OnExposeEvent (Gdk.EventExpose ev)
	{
		GdkWindow.DrawDrawable (grey_gc, back_buffer, ev.Area.X, ev.Area.Y, ev.Area.X, ev.Area.Y, ev.Area.Width, ev.Area.Height);

		return true;
	}

	protected override void
	OnSetScrollAdjustments (Gtk.Adjustment hadj, Gtk.Adjustment vadj)
	{
		this.hadj = hadj;
		this.vadj = vadj;

		hadj.Lower         = 0;
		hadj.Upper         = 1;
		hadj.StepIncrement = 0;
		hadj.PageSize      = 1;
		hadj.PageIncrement = 0;
		hadj.Value         = 0;

		vadj.Lower         = 0;
		vadj.Upper         = 1;
		vadj.StepIncrement = 0;
		vadj.PageSize      = 1;
		vadj.PageIncrement = 0;
		vadj.Value         = 0;

		hadj.ValueChanged += new System.EventHandler (HAdjustmentChanged);
	}

	void
	HAdjustmentChanged (object o, System.EventArgs e)
	{
		Gdk.Rectangle area = Allocation;
		area.X = 0; area.Y = 0;
		Draw ();
		GdkWindow.InvalidateRect (area, true);
	}
}

class AMCEditor
{
	[Glade.Widget] Gtk.Window		toplevel;
	[Glade.Widget] Gtk.ScrolledWindow	editor_swin;
	[Glade.Widget] Gtk.TreeView		bone_list;
	[Glade.Widget] CurveEditor		curve_editor;

	Gtk.TreeStore				bone_store;
	AMCFile					AMCData;
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
				typeof (bool));		// whether toggle is visible
		bone_list.Model = bone_store;

		// Create our text column
		Gtk.TreeViewColumn text_column  = new Gtk.TreeViewColumn ();
		Gtk.CellRenderer text_renderer = new Gtk.CellRendererText ();
		text_column.PackStart (text_renderer, true);
		text_column.AddAttribute (text_renderer, "text",           0);
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
			Gtk.Stock.Cancel, Gtk.ResponseType.Reject,
			Gtk.Stock.Open,   Gtk.ResponseType.Accept,
		};
		Gtk.FileChooserDialog fs = new Gtk.FileChooserDialog ("Open AMC...", null, Gtk.FileChooserAction.Open, responses);

		Gtk.ResponseType response = (Gtk.ResponseType) fs.Run ();
		fs.Hide ();

		if (response == Gtk.ResponseType.Accept) {
			Filename = fs.Filename;
			AMCData = AMCFile.Load (Filename);

			bone_store.Clear ();

			if (AMCData == null) {
				// FIXME - pop up an error dialog
				System.Console.WriteLine ("Error loading {0}", Filename);
				Filename = null;
			} else {
				curve_editor.AMC = AMCData;
				SetTitle ();

				AMCFrame f = (AMCFrame) AMCData.frames[0];

				IDictionaryEnumerator e = f.GetEnumerator ();
				e.Reset ();
				while (e.MoveNext ()) {
					Gtk.TreeIter iter;
					iter = bone_store.AppendNode ();
					bone_store.SetValue (iter, 0, e.Key);
					bone_store.SetValue (iter, 1, new Gdk.Color (0x00, 0x00, 0x00));

					string[] s = (string[]) e.Value;
					for (int i = 0; i < s.Length; i++) {
						Gtk.TreeIter citer = bone_store.AppendNode (iter);
						bone_store.SetValue (citer, 0, i.ToString ());
						bone_store.SetValue (citer, 1, new Gdk.Color (0x00, 0x00, 0x00));
						bone_store.SetValue (citer, 3, true);
					}
				}
			}
		}
		fs.Destroy ();
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

	void
	SetTitle ()
	{
		if (Filename == null) {
			toplevel.Title = "AMC Editor - None";
		} else {
			if (modified)
				toplevel.Title = "AMC Editor - " + System.IO.Path.GetFileName (Filename) + "*";
			else
				toplevel.Title = "AMC Editor - " + System.IO.Path.GetFileName (Filename);
		}
	}
}
