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
using Gnome;
using Gtk;
using GLib;

class PipelineDrawing : Gnome.Canvas
{
	private Gtk.Adjustment		hadj;
	private Gtk.Adjustment		vadj;
	private Gdk.Color		white;

	public PipelineDrawing ()
	{
		white = new Gdk.Color (0xff, 0xff, 0xff);
	}

	protected override void OnDrawBackground (Gdk.Drawable drawable, int x, int y, int w, int h)
	{
		Gdk.Colormap.System.AllocColor (ref white, true, true);

		Gdk.GC gc = new Gdk.GC (drawable);
		gc.Foreground = white;

		drawable.DrawRectangle (gc, true, x, y, w, h);
	}

	protected override void OnRenderBackground (ref CanvasBuf buffer)
	{
		// oh, how i wish upon a star, why the fuck can't i use this? :(
	}
};
