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
	static int			position = 0;
	// wow this is terrible.
	public static Gdk.Color[]	Colors = {
		new Gdk.Color (  8,   8,   8),
		new Gdk.Color ( 68,  44,  44),
		new Gdk.Color ( 80,   8,  12),
		new Gdk.Color ( 72,  56,  56),
		new Gdk.Color (104,  84,  68),
		new Gdk.Color (128, 116,  96),
		new Gdk.Color (116,  96,  80),
		new Gdk.Color ( 84,  56,  44),
		new Gdk.Color (140, 104,  88),
		new Gdk.Color (172, 116,  92),
		new Gdk.Color ( 68,  44,  56),
		new Gdk.Color ( 88,  72,  68),
		new Gdk.Color (112,  84,  76),
		new Gdk.Color (  8,   8,  28),
		new Gdk.Color (120, 116, 116),
		new Gdk.Color (104, 100, 108),
		new Gdk.Color (128, 124, 140),
		new Gdk.Color (132, 144, 164),
		new Gdk.Color (144, 152, 168),
		new Gdk.Color (136, 152, 172),
		new Gdk.Color (144, 156, 176),
		new Gdk.Color (148, 164, 184),
		new Gdk.Color ( 96,   8,   8),
		new Gdk.Color (116, 112, 128),
		new Gdk.Color (144, 116,  96),
		new Gdk.Color (128, 132, 152),
		new Gdk.Color (132, 132, 124),
		new Gdk.Color (128, 104,  84),
		new Gdk.Color ( 56,  40,  40),
		new Gdk.Color ( 84,  56,  56),
		new Gdk.Color (104,  96,  84),
		new Gdk.Color (196, 144, 108),
		new Gdk.Color (160, 124, 100),
		new Gdk.Color (136, 128, 108),
		new Gdk.Color ( 96,  80,  76),
 		new Gdk.Color ( 56,  24,  24),
		new Gdk.Color (148,  96,  80),
		new Gdk.Color (140,  56,  16),
 		new Gdk.Color ( 60,   8,  12),
		new Gdk.Color (160, 168, 184),
 		new Gdk.Color ( 60 , 52,  52),
		new Gdk.Color (152, 152, 160),
		new Gdk.Color (184, 152, 112),
 		new Gdk.Color ( 88,   8,  32),
 		new Gdk.Color ( 92,  56,  68),
		new Gdk.Color (120, 116, 140),
 		new Gdk.Color ( 68,  12,  44),
 		new Gdk.Color ( 24,   8,   8),
		new Gdk.Color (128,  84,  76),
		new Gdk.Color (192, 120,  20),
 		new Gdk.Color ( 24,  12,  28),
		new Gdk.Color (148, 148, 128),
		new Gdk.Color (148, 136, 112),
		new Gdk.Color (112, 108,  96),
		new Gdk.Color (140,  40,  44),
 		new Gdk.Color ( 80,  12,  48),
		new Gdk.Color (136, 100,  72),
		new Gdk.Color (164, 136, 104),
 		new Gdk.Color ( 80,  44,  48),
		new Gdk.Color (112, 104, 116),
 		new Gdk.Color ( 72,  12,  36),
		new Gdk.Color (124,  36,  32),
 		new Gdk.Color ( 44,  20,  40),
		new Gdk.Color (104,  72,  68),
 		new Gdk.Color ( 56,  40,  24),
		new Gdk.Color (132,  92,  80),
		new Gdk.Color (140, 132, 148),
		new Gdk.Color (104,  68,  56),
		new Gdk.Color (184,  64,   8),
 		new Gdk.Color ( 44,   8,   8),
 		new Gdk.Color ( 68,  44,  28),
		new Gdk.Color (112,   8,  12),
 		new Gdk.Color ( 28,  24,  40),
 		new Gdk.Color ( 92,  40,  44),
 		new Gdk.Color ( 40,   8,  24),
		new Gdk.Color (112,  68,  68),
 		new Gdk.Color ( 56,   8,  36),
		new Gdk.Color (128,  84,  56),
		new Gdk.Color (144, 104,  72),
		new Gdk.Color (100,  32,  12),
		new Gdk.Color (176,  28,   8),
		new Gdk.Color ( 68,  36,  44),
		new Gdk.Color (152, 108,  80),
		new Gdk.Color (120,  96,  64),
		new Gdk.Color (116,   8,  36),
		new Gdk.Color ( 88,  44,  56),
		new Gdk.Color ( 92,  68,  56),
		new Gdk.Color (228, 228, 204),
 		new Gdk.Color ( 56,  24,  40),
		new Gdk.Color (124,  56,  44),
		new Gdk.Color (180, 136, 104),
		new Gdk.Color (152,  96,  72),
		new Gdk.Color ( 80,  40,   8),
		new Gdk.Color ( 88,  12,  60),
		new Gdk.Color ( 80,  60,  68),
		new Gdk.Color (172, 172, 172),
		new Gdk.Color (164, 116,  84),
		new Gdk.Color (140, 140, 156),
		new Gdk.Color (124,  88,  68),
		new Gdk.Color (156, 112,  92),
		new Gdk.Color (124, 120, 132),
		new Gdk.Color ( 76,  44,  56),
		new Gdk.Color (124,  36,   8),
		new Gdk.Color (120,  72,  44),
		new Gdk.Color ( 72,  56,  64),
		new Gdk.Color (172, 128,  92),
		new Gdk.Color (176,  92,  12),
		new Gdk.Color ( 92,  56,  56),
		new Gdk.Color ( 44,  24,  24),
		new Gdk.Color (136,  76,  64),
		new Gdk.Color (116,  68,  56),
		new Gdk.Color ( 44,  40,  40),
		new Gdk.Color ( 96,  60,  48),
		new Gdk.Color (168, 116,  72),
		new Gdk.Color (124,  60,  20),
		new Gdk.Color (100,  40,  56),
		new Gdk.Color (112,  76,  60),
		new Gdk.Color ( 24,   8,  40),
		new Gdk.Color ( 72,  56,  44),
		new Gdk.Color (192, 168, 132),
		new Gdk.Color (152, 160, 172),
		new Gdk.Color (156,  56,   8),
		new Gdk.Color (160, 100,  84),
		new Gdk.Color (140, 116,  84),
		new Gdk.Color (  8,   8,  48),
		new Gdk.Color (136,  68,  44),
		new Gdk.Color (104,  12,  48),
		new Gdk.Color (120,  76,  68),
		new Gdk.Color (176, 172, 156),
		new Gdk.Color (188, 140,  96),
		new Gdk.Color (172,  80,  40),
		new Gdk.Color ( 44,  28,  40),
		new Gdk.Color (124,  36,  60),
		new Gdk.Color (140,  28,  76),
		new Gdk.Color ( 88,  72,  80),
		new Gdk.Color ( 44,   8,  40),
		new Gdk.Color (184, 104,  60),
		new Gdk.Color (132,  88,  64),
		new Gdk.Color (160, 104,  72),
		new Gdk.Color (124,  76,  64),
		new Gdk.Color ( 92,  12,  48),
		new Gdk.Color (184, 180, 180),
		new Gdk.Color (104,  72,  48),
		new Gdk.Color ( 44,  40,  28),
		new Gdk.Color ( 56,  44,  56),
		new Gdk.Color ( 36,  12,  48),
		new Gdk.Color ( 96,  44,  36),
		new Gdk.Color (192, 192, 192),
		new Gdk.Color (164, 176, 192),
		new Gdk.Color (188, 128,  96),
		new Gdk.Color (160,  84,  72),
		new Gdk.Color (108,  56,  52),
		new Gdk.Color (172, 156, 128),
		new Gdk.Color (148,  72,  24),
		new Gdk.Color (164, 160, 164),
		new Gdk.Color ( 80,  36,  56),
		new Gdk.Color (156,  84,   8),
		new Gdk.Color (152, 128,  92),
		new Gdk.Color (144,  16,  48),
		new Gdk.Color (152,   8,   8),
		new Gdk.Color ( 76,  52,  28),
		new Gdk.Color (148,  40,  12),
		new Gdk.Color (104,  56,  60),
		new Gdk.Color (128,  64,  64),
		new Gdk.Color ( 68,  28,  56),
		new Gdk.Color (136, 136, 140),
	};

	public static int
	AllocColor ()
	{
		int ret = position;
		position++;
		return ret;
	}
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
			for (int i = 0; i < tokens.Length - 2; i++)
				values[i] = tokens[i + 1];
			data.Add (name, values);
		} else {
			string[] values = new string[tokens.Length - 1];
			for (int i = 0; i < tokens.Length - 1; i++)
				values[i] = tokens[i + 1];
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

		// FIXME - write out frames

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
	Pango.Layout		biglayout;
	Pango.Layout		smalllayout;
	int			redraw_timeout;
	ArrayList		bones;

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
			enabled_bones = new ArrayList ();
			bones = new ArrayList ();
			CreateBackBuffer ();
			Draw ();

			hadj.Upper = nframes * 40;
			hadj.StepIncrement = 120;

			QueueRedraw ();
		}
	}

	public
	CurveEditor ()
	{
		enabled_bones = new ArrayList ();
		bones = new ArrayList ();
		redraw_timeout = 0;
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

		biglayout = CreatePangoLayout (null);
		biglayout.FontDescription = Pango.FontDescription.FromString ("Bitstream Vera Sans 8");
		smalllayout = CreatePangoLayout (null);
		smalllayout.FontDescription = Pango.FontDescription.FromString ("Bitstream Vera Sans 6");
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

		// Draw frame lines and numbers
		for (int i = 0; i < amc.frames.Count; i++) {
			int pos = i * 40 + 20;
			if (IsVisible (pos)) {
				back_buffer.DrawLine (black_gc,
					(int) (pos - hadj.Value), 0,
					(int) (pos - hadj.Value), Allocation.Height - 30);

				biglayout.SetText (i.ToString ());
				int lw, lh;
				biglayout.GetPixelSize (out lw, out lh);

				back_buffer.DrawLayout (black_gc, (int) (pos - (lw / 2) - hadj.Value), Allocation.Height - 28, biglayout);

				float seconds = i / 120.0f;

				smalllayout.SetText (System.String.Format ("{0:00.000}", seconds));
				smalllayout.GetPixelSize (out lw, out lh);
				back_buffer.DrawLayout (black_gc, (int) (pos - (lw / 2) - hadj.Value), Allocation.Height - 10, smalllayout);
			}

			if (IsVisible (pos) || IsVisible (pos + 40)) {
				// Iterate through all the bones, draw those that are active.
				foreach (object[] data in bones) {
					int angle = (int) data[0];
					string name = (string) data[1];
					Gdk.GC gc = (Gdk.GC) data[2];

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

						back_buffer.DrawLine (gc,
							(int) (pos - hadj.Value),      height1,
							(int) (pos + 40 - hadj.Value), height2);

						Gdk.Rectangle r = new Gdk.Rectangle ();
						r.X = (int) (pos - hadj.Value - 2);
						r.Y = height1 - 2;
						r.Width = 5;
						r.Height = 5;
						back_buffer.DrawRectangle (black_gc, true, r);
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
		back_buffer.DrawLine (black_gc, 0, Allocation.Height - 30, Allocation.Width, Allocation.Height - 30);
	}

	bool
	IsVisible (int x)
	{
		return ((x > hadj.Value) && (x < (hadj.Value + Allocation.Width)));
	}

	int FindValueHeight (float f)
	{
		float pc = f / 360.0f;
		float height = (Allocation.Height - 31) * pc;
		return (int) ((Allocation.Height - 31) - height);
	}

	protected override bool
	OnConfigureEvent (Gdk.EventConfigure ev)
	{
		hadj.PageSize = ev.Width;
		hadj.PageIncrement = ev.Width;

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
		QueueRedraw ();
	}

	public void
	VisibilityChanged ()
	{
		bones = new ArrayList ();
		foreach (string p in enabled_bones) {
			Gtk.TreePath path = new Gtk.TreePath (p);
			Gtk.TreeIter iter;

			bone_store.GetIter (out iter, path);
			int num = (int) bone_store.GetValue (iter, 5);
			Gdk.Color color = (Gdk.Color) ColorMap.Colors[(int) bone_store.GetValue (iter, 4)];

			Gtk.TreeIter parent;
			bone_store.IterParent (out parent, iter);
			string s = (string) bone_store.GetValue (parent, 0);

			Gdk.GC gc = new Gdk.GC (back_buffer);
			back_buffer.Colormap.AllocColor (ref color, true, true);
			gc.Foreground = color;

			object[] data = {
				num, s, gc
			};
			bones.Add (data);
		}

		QueueRedraw ();
	}

	void
	QueueRedraw ()
	{
		if (redraw_timeout == 0)
			GLib.Timeout.Add (20, new GLib.TimeoutHandler (Redraw));
	}

	bool
	Redraw ()
	{
		Gdk.Rectangle area = Allocation;
		area.X = 0; area.Y = 0;
		Draw ();
		GdkWindow.InvalidateRect (area, true);

		redraw_timeout = 0;
		return false;
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
						int color = ColorMap.AllocColor ();
						Gtk.TreeIter citer = bone_store.AppendNode (iter);
						bone_store.SetValue (citer, 0, System.String.Format ("<b>{0}</b>", i.ToString ()));
						bone_store.SetValue (citer, 1, ColorMap.Colors[color]);
						bone_store.SetValue (citer, 3, true);
						bone_store.SetValue (citer, 4, color);
						bone_store.SetValue (citer, 5, i);
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
