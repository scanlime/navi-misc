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

class CurveEditor : Gtk.DrawingArea
{
	// Widget data
	Gtk.Adjustment		hadj;
	Gtk.Adjustment		vadj;

	// Picking data
	ArrayList		pad_positions;
	ArrayList		selected_pads;

	// Drawing data
	Gdk.GC			white_gc;
	Gdk.GC			grey_gc;
	Gdk.GC			black_gc;
	Gdk.Pixmap		back_buffer;
	Pango.Layout		biglayout;
	Pango.Layout		smalllayout;
	uint			redraw_timeout;
	ArrayList		bones;
	int[]			visible_range;

	// Information about the AMC data
	int 			nframes;
	public Gtk.TreeStore	bone_store;
	public ArrayList	enabled_bones;

	// Window information
	int			width;
	int			height;

	// The AMC file
	AMC.File		amc;
	public AMC.File		AMC
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
			hadj.Value = 0;
			hadj.StepIncrement = 120;
			RecomputeRange ();

			QueueRedraw ();
		}
	}

	public
	CurveEditor ()
	{
		enabled_bones = new ArrayList ();
		pad_positions = new ArrayList ();
		selected_pads = new ArrayList ();
		bones = new ArrayList ();
		redraw_timeout = 0;

		visible_range = new int[2];
		visible_range[0] = 0;
		visible_range[1] = 0;
	}

	void
	CreateGCs ()
	{
		Gdk.Color white = new Gdk.Color (0xff, 0xff, 0xff);
		Gdk.Color grey  = new Gdk.Color (0xde, 0xde, 0xde);
		Gdk.Color black = new Gdk.Color (0x00, 0x00, 0x00);

		white_gc = new Gdk.GC (GdkWindow);
		grey_gc  = new Gdk.GC (GdkWindow);
		black_gc = new Gdk.GC (GdkWindow);

		GdkWindow.Colormap.AllocColor (ref white, true, true);
		GdkWindow.Colormap.AllocColor (ref grey,  true, true);
		GdkWindow.Colormap.AllocColor (ref black, true, true);

		white_gc.Foreground = white;
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

		pad_positions.Clear ();

		// Draw frame lines and numbers
		for (int i = visible_range[0]; i < visible_range[1]; i++) {
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

			// Iterate through all the bones, draw those that are active.
			foreach (object[] data in bones) {
				int angle = (int) data[0];
				string name = (string) data[1];
				Gdk.GC gc = (Gdk.GC) data[2];

				if (i + 1 != amc.frames.Count) {
					AMC.Frame frame1 = (AMC.Frame) amc.frames[i];
					AMC.Frame frame2 = (AMC.Frame) amc.frames[i + 1];
					float val1 = ((float[]) (frame1.data[name]))[angle];
					float val2 = ((float[]) (frame2.data[name]))[angle];

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

					// Add the rect to the pick list
					object[] pick_data = {
						r, i, name, angle
					};
					pad_positions.Add (pick_data);
				} else {
					AMC.Frame frame = (AMC.Frame) amc.frames[i];
					float val = ((float[]) (frame.data[name]))[angle];

					if (val < 0) val = 360.0f - val;
					if (val > 360) val = val - 360.0f;

					int height = FindValueHeight (val);
					Gdk.Rectangle r = new Gdk.Rectangle ();
					r.X = (int) (pos - hadj.Value - 2);
					r.Y = height - 2;
					r.Width = 5;
					r.Height = 5;
					back_buffer.DrawRectangle (black_gc, true, r);

					// Add the rect to the pick list
					object[] pick_data = {
						r, i, name, angle
					};
					pad_positions.Add (pick_data);
				}
			}
		}

		foreach (object[] pick_data in selected_pads) {
			Gdk.Rectangle rect = (Gdk.Rectangle) pick_data[0];
			back_buffer.DrawRectangle (white_gc, true, rect);
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
		RecomputeRange ();
		QueueRedraw ();
	}

	void
	RecomputeRange ()
	{
		visible_range[0] = (int) (hadj.Value / 40) - 1;
		int i = visible_range[0] + 40;
		while (IsVisible (i * 40 + 20) && i < amc.frames.Count)
			i++;
		visible_range[1] = i;

		// clamp ranges
		if (visible_range[0] < 0)
			visible_range[0] = 0;
		if (visible_range[1] > amc.frames.Count)
			visible_range[1] = amc.frames.Count;
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
			redraw_timeout = GLib.Timeout.Add (20, new GLib.TimeoutHandler (Redraw));
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

	public void
	ButtonPress (Gdk.EventButton ev)
	{
		Gdk.Point p = new Gdk.Point ();
		p.X = (int) ev.X;
		p.Y = (int) ev.Y;
		bool changed = false;
		bool foundold = false;
		ArrayList new_selected = new ArrayList ();

		GrabFocus ();

		for (int i = 0; i < pad_positions.Count; i++) {
			object[] pick_data = (object[]) pad_positions[i];
			Gdk.Rectangle rect = (Gdk.Rectangle) pick_data[0];
			if (rect.Contains (p)) {
				int frame   = (int)    pick_data[1];
				string name = (string) pick_data[2];
				int angle   = (int)    pick_data[3];
				System.Console.WriteLine ("picked {0}:{1} at frame {2}", name, angle, frame);
				if (selected_pads.Contains (pick_data)) {
					foundold = true;
				} else {
					new_selected.Add (pick_data);
					changed = true;
				}
				break;
			}
		}

		if (!foundold) {
			if ((ev.State & Gdk.ModifierType.ShiftMask) == 0) {
				selected_pads = new_selected;
				changed = true;
			} else {
				selected_pads.AddRange (new_selected);
				changed = true;
			}
		}

		if (changed)
			QueueRedraw ();
	}

	public void
	ButtonRelease (Gdk.EventButton ev)
	{
	}

	public void
	MotionNotify (Gdk.EventMotion ev)
	{
	}

	public void
	KeyPress (Gdk.EventKey ev)
	{
		System.Console.WriteLine ("key press!");
	}
}

class AMCEditor
{
	[Glade.Widget] Gtk.Window		toplevel;
	[Glade.Widget] Gtk.TreeView		bone_list;
	[Glade.Widget] CurveEditor		curve_editor;
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
			toplevel.Title = "AMC Editor - None";
		} else {
			if (modified)
				toplevel.Title = "AMC Editor - " + System.IO.Path.GetFileName (Filename) + "*";
			else
				toplevel.Title = "AMC Editor - " + System.IO.Path.GetFileName (Filename);
		}
	}

	void
	UpdateToolbarSensitivity ()
	{
		menu_save.Sensitive = modified;
		menu_saveas.Sensitive = (Filename != null);
	}

	void
	CurveButtonPressEvent (object o, Gtk.ButtonPressEventArgs args)
	{
		curve_editor.ButtonPress (args.Event);
	}

	void
	CurveButtonReleaseEvent (object o, Gtk.ButtonReleaseEventArgs args)
	{
		curve_editor.ButtonRelease (args.Event);
	}

	void
	CurveMotionNotifyEvent (object o, Gtk.MotionNotifyEventArgs args)
	{
		curve_editor.MotionNotify (args.Event);
	}

	void
	CurveKeyPressEvent (object o, Gtk.KeyPressEventArgs args)
	{
		curve_editor.KeyPress (args.Event);
	}
}
