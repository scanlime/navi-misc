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

using System;
using System.Collections;
using Gtk;
using GLib;

class PipelineDrawing : Gtk.DrawingArea
{
	private int			size_x, size_y;
	private int			drawing_x, drawing_y;
	private Gtk.Adjustment		hadj;
	private Gtk.Adjustment		vadj;

	public PipelineDrawing ()
	{
		drawing_x = 0;
		drawing_y = 0;
	}

	protected override bool OnConfigureEvent (Gdk.EventConfigure ev)
	{
		size_x = ev.Width;
		size_y = ev.Height;

		if (drawing_x == 0)
			drawing_x = size_x;
		if (drawing_y == 0)
			drawing_y = size_y;

		if (hadj != null) {
			float w = (float) ev.Width;
			float h = (float) ev.Height;

			// temporarily set a big region. this will need to be replaced by
			// tracking the drawing on the page
			hadj.Upper = w * 8;
			vadj.Upper = h * 8;

			// set our page size to be wxh
			hadj.PageSize = w;
			vadj.PageSize = h;
			hadj.PageIncrement = w;
			vadj.PageIncrement = h;
		}

		return base.OnConfigureEvent (ev);
	}

	protected override void OnSetScrollAdjustments (Adjustment hadj, Adjustment vadj)
	{
		// set step size to 10px, everything else to 0
		hadj.Value         = 0.0f;
		hadj.Upper         = 0.0f;
		hadj.Lower         = 0.0f;
		hadj.StepIncrement = 10.0f;
		hadj.PageIncrement = 0.0f;
		hadj.PageSize      = 0.0f;

		vadj.Value         = 0.0f;
		vadj.Upper         = 0.0f;
		vadj.Lower         = 0.0f;
		vadj.StepIncrement = 10.0f;
		vadj.PageIncrement = 0.0f;
		vadj.PageSize      = 0.0f;

		this.hadj = hadj;
		this.vadj = vadj;
	}
};
