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
	public PipelineDrawing ()
	{
	}

	protected override bool OnConfigureEvent (Gdk.EventConfigure ev)
	{
		System.Console.WriteLine("configure: {0}x{1}\n", ev.Width, ev.Height);
		return base.OnConfigureEvent (ev);
	}

	protected override void OnSizeAllocated (Gdk.Rectangle allocation)
	{
		System.Console.WriteLine("allocated: {0}x{1}\n", allocation.Width, allocation.Height);
		base.OnSizeAllocated (allocation);
	}

	protected override void OnSizeRequested (ref Gtk.Requisition requisition)
	{
		base.OnSizeRequested (ref requisition);
	}

	protected override void OnSetScrollAdjustments (Adjustment hadj, Adjustment vadj)
	{
	}
};
