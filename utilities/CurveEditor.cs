/*
 * CurveEditor.cs - a widget which provides blender-IPO-like
 * editing facilities for AMC data
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

class PickData : System.ICloneable
{
	public Gdk.Rectangle	rect;
	public int		frame;
	public string		bone;
	public int		angle;

	public
	PickData (Gdk.Rectangle rect, int frame, string bone, int angle)
	{
		this.rect  = rect;
		this.frame = frame;
		this.bone  = bone;
		this.angle = angle;
	}

	public object
	Clone ()
	{
		return new PickData (rect, frame, bone, angle);
	}
}

class CurveEditor : Gtk.DrawingArea
{
	// Widget data
	Gtk.Adjustment		hadj;
	Gtk.Adjustment		vadj;

	// Picking data
	ArrayList		pad_positions;
	ArrayList		selected_pads;
	ArrayList		saved_pads;
	bool			grabbed;
	bool			lasso;
	int			lasso_x, lasso_y;
	int			mouse_x, mouse_y;

	// Drawing data
	Gdk.GC			white_gc;
	Gdk.GC			grey_gc;
	Gdk.GC			black_gc;
	Gdk.GC			lasso_gc;
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

		grabbed = false;
		lasso = false;
	}

	void
	CreateGCs ()
	{
		Gdk.Color white = new Gdk.Color (0xff, 0xff, 0xff);
		Gdk.Color grey  = new Gdk.Color (0xde, 0xde, 0xde);
		Gdk.Color black = new Gdk.Color (0x00, 0x00, 0x00);
		Gdk.Color lasso = new Gdk.Color (0xa0, 0xa0, 0xa0);

		white_gc = new Gdk.GC (GdkWindow);
		grey_gc  = new Gdk.GC (GdkWindow);
		black_gc = new Gdk.GC (GdkWindow);
		lasso_gc = new Gdk.GC (GdkWindow);

		GdkWindow.Colormap.AllocColor (ref white, true, true);
		GdkWindow.Colormap.AllocColor (ref grey,  true, true);
		GdkWindow.Colormap.AllocColor (ref black, true, true);
		GdkWindow.Colormap.AllocColor (ref lasso, true, true);

		white_gc.Foreground = white;
		grey_gc.Foreground  = grey;
		black_gc.Foreground = black;
		lasso_gc.Foreground = lasso;

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
			DrawFrameLine (i, pos);

			// Iterate through all the bones, draw those that are active.
			foreach (object[] data in bones)
				DrawBone (i, pos, data);
		}

		foreach (PickData pick_data in selected_pads) {
			Gdk.Rectangle rect = pick_data.rect;
			if (IsVisible (rect.X)) {
				rect.X -= (int) hadj.Value;
				back_buffer.DrawRectangle (white_gc, true, rect);
			}
		}

		// Draw border between frame # and edit region
		back_buffer.DrawLine (black_gc, 0, Allocation.Height - 30, Allocation.Width, Allocation.Height - 30);

		if (lasso) {
			// Draw selection box
			DrawLasso ();
		}
	}

	void
	DrawFrameLine (int frame, int position) {
		if (IsVisible (position)) {
			back_buffer.DrawLine (black_gc,
				(int) (position - hadj.Value), 0,
				(int) (position - hadj.Value), Allocation.Height - 30);

			biglayout.SetText (frame.ToString ());
			int lw, lh;
			biglayout.GetPixelSize (out lw, out lh);

			back_buffer.DrawLayout (black_gc,
				(int) (position - (lw / 2) - hadj.Value),
				Allocation.Height - 28,
				biglayout);

			// FIXME - this really shouldn't be hardcoded, but it's not in
			// any of the data files
			float seconds = frame / 120.0f;

			smalllayout.SetText (System.String.Format ("{0:00.000}", seconds));
			smalllayout.GetPixelSize (out lw, out lh);
			back_buffer.DrawLayout (black_gc,
				(int) (position - (lw / 2) - hadj.Value),
				Allocation.Height - 10,
				smalllayout);
		}
	}

	void
	DrawBone (int frame, int pos, object[] bone) {
		int angle   = (int)    bone[0];
		string name = (string) bone[1];
		Gdk.GC gc   = (Gdk.GC) bone[2];

		if (frame + 1 != amc.frames.Count) {
			// If we're not on the last frame, draw a pad for the value
			// and a line going to the next frame's value
			AMC.Frame frame1 = (AMC.Frame) amc.frames[frame];
			AMC.Frame frame2 = (AMC.Frame) amc.frames[frame + 1];

			float val1 = ((float[]) (frame1.data[name]))[angle];
			float val2 = ((float[]) (frame2.data[name]))[angle];

			// FIXME - this completely screws up root position and negative
			// angles, which might come back later to bite us if we're looking
			// for gaps or jumps in the data
			if (val1 < 0) val1 = 360.0f - val1;
			if (val2 < 0) val2 = 360.0f - val2;
			if (val1 > 360) val1 = val1 - 360.0f;
			if (val2 > 360) val2 = val2 - 360.0f;

			int height1 = FindValueHeight (val1);
			int height2 = FindValueHeight (val2);

			// Draw the curve line
			back_buffer.DrawLine (gc,
				(int) (pos - hadj.Value),      height1,
				(int) (pos - hadj.Value + 40), height2);

			// Draw and store the pad
			Gdk.Rectangle r = new Gdk.Rectangle ();
			r.X = (int) (pos - hadj.Value - 2);
			r.Y = height1 - 2;
			r.Width = 5;
			r.Height = 5;
			back_buffer.DrawRectangle (black_gc, true, r);

			pad_positions.Add (new PickData (r, frame, name, angle));
		} else {
			AMC.Frame frame1 = (AMC.Frame) amc.frames[frame];
			float val = ((float[]) (frame1.data[name]))[angle];

			// FIXME - this completely screws up root position and negative
			// angles, which might come back later to bite us if we're looking
			// for gaps or jumps in the data
			if (val < 0) val = 360.0f - val;
			if (val > 360) val = val - 360.0f;

			// Draw and store the pad
			int height = FindValueHeight (val);
			Gdk.Rectangle r = new Gdk.Rectangle ();
			r.X = (int) (pos - hadj.Value - 2);
			r.Y = height - 2;
			r.Width  = 5;
			r.Height = 5;
			back_buffer.DrawRectangle (black_gc, true, r);

			pad_positions.Add (new PickData (r, frame, name, angle));
		}
	}

	bool
	IsVisible (int x)
	{
		return ((x > hadj.Value) && (x < (hadj.Value + Allocation.Width)));
	}

	void
	DrawLasso ()
	{
		Gdk.Rectangle lasso_rect = new Gdk.Rectangle ();

		int mx, my;
		GetPointer (out mx, out my);
		if (mx < lasso_x) {
			lasso_rect.X = mx;
			lasso_rect.Width = lasso_x - mx;
		} else {
			lasso_rect.X = lasso_x;
			lasso_rect.Width = mx - lasso_x;
		}

		if (my < lasso_y) {
			lasso_rect.Y = my;
			lasso_rect.Height = lasso_y - my;
		} else {
			lasso_rect.Y = lasso_y;
			lasso_rect.Height = my - lasso_y;
		}

		back_buffer.DrawRectangle (lasso_gc, false, lasso_rect);
	}

	int
	FindValueHeight (float f)
	{
		float pc = f / 360.0f;
		float height = (Allocation.Height - 30) * pc;
		return (int) ((Allocation.Height - 30) - height);
	}

	float
	FindHeightValue (int h)
	{
		float inv = (float) ((Allocation.Height - 30) - h);
		float pc = inv / (Allocation.Height - 30);
		return (pc * 360.0f);
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
		selected_pads.Clear ();
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
		if (grabbed) {
			grabbed = false;
		} else if (lasso) {
			Gdk.Rectangle r = new Gdk.Rectangle ();
			int mx, my;

			GetPointer (out mx, out my);
			if (mx < lasso_x) {
				r.X = mx;
				r.Width = lasso_x - mx;
			} else {
				r.X = lasso_x;
				r.Width = mx - lasso_x;
			}

			if (my < lasso_y) {
				r.Y = my;
				r.Height = lasso_y - my;
			} else {
				r.Y = lasso_y;
				r.Height = my - lasso_y;
			}

			ArrayList new_selected = new ArrayList ();
			foreach (PickData pick_data in pad_positions) {
				Gdk.Rectangle r2;
				if (r.Intersect (pick_data.rect, out r2)) {
					PickData newd = (PickData) pick_data.Clone ();
					newd.rect.X += (int) hadj.Value;
					new_selected.Add (newd);
				}
			}

			if ((ev.State & Gdk.ModifierType.ShiftMask) == 0) {
				selected_pads = new_selected;
			} else {
				selected_pads.AddRange (new_selected);
			}

			lasso = false;
			Redraw ();
		} else {
			Gdk.Point p = new Gdk.Point ();
			p.X = (int) ev.X;
			p.Y = (int) ev.Y;
			bool changed = false;
			ArrayList new_selected = new ArrayList ();

			for (int i = 0; i < pad_positions.Count; i++) {
				PickData pick_data = (PickData) pad_positions[i];
				if (pick_data.rect.Contains (p)) {
					PickData newd = (PickData) pick_data.Clone ();
					newd.rect.X += (int) hadj.Value;
					new_selected.Add (newd);
					changed = true;
					break;
				}
			}

			if ((ev.State & Gdk.ModifierType.ShiftMask) == 0) {
				selected_pads = new_selected;
				changed = true;
			} else {
				selected_pads.AddRange (new_selected);
				changed = true;
			}

			if (changed)
				QueueRedraw ();
		}
	}

	public void
	ButtonRelease (Gdk.EventButton ev)
	{
	}

	public void
	MotionNotify (Gdk.EventMotion ev)
	{
		if (grabbed) {
			int new_x, new_y;
			GetPointer (out new_x, out new_y);
			int offset_y = new_y - mouse_y;

			foreach (PickData pick_data in selected_pads) {
				pick_data.rect.Y += offset_y;

				AMC.Frame frame = (AMC.Frame) amc.frames[pick_data.frame];
				float val = FindHeightValue (pick_data.rect.Y + 2);
				((float[]) frame.data[pick_data.bone])[pick_data.angle] = val;
			}

			mouse_x = new_x;
			mouse_y = new_y;

			QueueRedraw ();
		} else if (lasso) {
			QueueRedraw ();
		}
	}

	public void
	KeyPress (Gdk.EventKey ev)
	{
		if (grabbed && (ev.Key == Gdk.Key.Escape)) {
			// Restore the state that we saved when we began the drag
			foreach (PickData pick_data in saved_pads) {
				AMC.Frame frame = (AMC.Frame) amc.frames[pick_data.frame];
				float val = FindHeightValue (pick_data.rect.Y + 3);
				((float[]) frame.data[pick_data.bone])[pick_data.angle] = val;
			}
			selected_pads = saved_pads;
			grabbed = false;
			QueueRedraw ();
			return;
		}

		if ((selected_pads.Count > 0) && (ev.Key == Gdk.Key.g)) {
			// Copy selected_pads so we can restore that state later
			saved_pads = new ArrayList ();
			foreach (PickData pick_data in selected_pads) {
				saved_pads.Add (pick_data.Clone ());
			}
			GetPointer (out mouse_x, out mouse_y);
			grabbed = true;
			return;
		}

		if (ev.Key == Gdk.Key.b) {
			GetPointer (out lasso_x, out lasso_y);
			lasso = true;
			return;
		}
	}
}
