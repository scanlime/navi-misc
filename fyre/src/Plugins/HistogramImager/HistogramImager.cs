/*
 * HistogramImager.cs - An Element which implements the histogram
 *	imager algorithm.
 *
 * Fyre - a generic framework for computational art
 * Copyright (C) 2004-2005 Fyre Team (see AUTHORS)
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

using System.Xml;

class HistogramImager : Fyre.Element
{
	static Gdk.Pixbuf icon;

	public
	HistogramImager () : base ()
	{
		inputs = new Fyre.InputPad[] {
			new Fyre.InputPad ("v", "point"),
			new Fyre.InputPad ("w", "width"),
			new Fyre.InputPad ("h", "height"),
		};

		outputs = new Fyre.OutputPad[] {
			new Fyre.OutputPad ("M", "image"),
		};

		SetPadNumbers ();
		NewCanvasElement ();
		NewID ();
	}

	public override string
	Name ()
	{
		return "Histogram Imager";
	}

	public override string
	Category()
	{
		return "Renderers";
	}

	public override Gdk.Pixbuf
	Icon ()
	{
		if (icon == null)
			icon = new Gdk.Pixbuf (null, "HistogramImager.png");
		return icon;
	}

	public override string
	Description ()
	{
		return "Maintains a histogram of the\npoints given to it, suitable\nfor tone-mapping.";
	}

	public override bool
	Check (Fyre.Type[] t, out Fyre.Type[] to)
	{
		to = null;
		int[] dim = new int[2];

		// Check pad 1
		if (!(t[0] is Fyre.Matrix))
			return false;
		Fyre.Matrix m = (Fyre.Matrix) t[0];
		if (!(m.Rank == 1 && m.Size[0] == 2))
			return false;

		// Check pad 2
		if (!(t[1] is Fyre.Int))
			return false;
		Fyre.Int i = (Fyre.Int) t[1];
		dim[0] = i.Value;

		// Check pad 3
		if (!(t[2] is Fyre.Int))
			return false;
		i = (Fyre.Int) t[2];
		dim[1] = i.Value;


		to = new Fyre.Type[] {
			new Fyre.Matrix (new Fyre.Float (), 2, dim),
		};
		return true;
	}
}
