/*
 * PipelineDrawing.cs - Gtk.DrawingArea subclass that handles drawing of
 *	a Fyre pipeline
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

	class PipelineDrawing : Gtk.DrawingArea
	{
		Gtk.Adjustment		hadj;
		Gtk.Adjustment		vadj;

		// The drawing extents are the size of our current drawing area. This
		// can be modified via the arrows on the scrollbars. It is always a
		// superset of the layout extents, which are the size of the pipeline
		// itself. When scrolling around, the drawing extents shrink to the
		// layout extents when the area isn't visible.
		Gdk.Rectangle		drawing_extents;
		Gdk.Rectangle		layout_extents;

		public PipelineDrawing () : base ()
		{
			drawing_extents = new Gdk.Rectangle ();
			layout_extents = new Gdk.Rectangle ();

			// For now, initialize the layout extents to 300x300 at the origin
			layout_extents.X = 0;
			layout_extents.Y = 0;
			layout_extents.Width  = 300;
			layout_extents.Height = 300;

			drawing_extents = layout_extents;

			Show ();
		}

		protected override bool OnExposeEvent (Gdk.EventExpose ev)
		{
			Gdk.Drawable d = GdkWindow;
			Gdk.GC gc = Style.BackgroundGC (Gtk.StateType.Prelight);

			d.DrawRectangle (gc, true, ev.Area);
			return true;
		}

		protected override bool OnConfigureEvent (Gdk.EventConfigure ev)
		{
			if (hadj != null) {
				hadj.PageSize = ev.Width;
				vadj.PageSize = ev.Height;

				hadj.PageIncrement = ev.Width;
				vadj.PageIncrement = ev.Height;

				// Temporarily set the range here. This will need to query
				// the main canvas for extents once that exists
				hadj.Upper = ev.Width * 8;
				vadj.Upper = ev.Height * 8;
			}

			return base.OnConfigureEvent (ev);
		}

		protected override void OnSetScrollAdjustments (Gtk.Adjustment hadj, Gtk.Adjustment vadj)
		{
			hadj.Lower         = 0.0;
			hadj.Upper         = 0.0;
			hadj.Value         = 0.0;
			hadj.PageIncrement = 0.0;
			hadj.PageSize      = 0.0;
			hadj.StepIncrement = 0.0;

			vadj.Lower         = 0.0;
			vadj.Upper         = 0.0;
			vadj.Value         = 0.0;
			vadj.PageIncrement = 0.0;
			vadj.PageSize      = 0.0;
			vadj.StepIncrement = 0.0;

			this.hadj = hadj;
			this.vadj = vadj;
		}
	}

}
