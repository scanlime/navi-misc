/*
 * HistogramImager.cs - An Element which implements the histogram
 *	imager algorithm.
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

using Gdk;

class HistogramImager : Element
{
	static Gdk.Pixbuf icon;
	static string[,] inputs;
	static string[,] outputs;

	public override string Name ()
	{
		return "Histogram Imager";
	}

	public override string Category()
	{
		return "Renderers";
	}

	public override Gdk.Pixbuf Icon ()
	{
		if (icon == null)
			icon = new Gdk.Pixbuf (null, "HistogramImager.png");
		return icon;
	}

	public override string Description ()
	{
		return "Maintains a histogram of the\npoints given to it, suitable\nfor tone-mapping.";
	}

	public override string[,] InputDesc ()
	{
		if (inputs == null) {
			inputs = new string[3,2];
			inputs[0,0] = "<i>v</i>";	inputs[0,1] = "point";
			inputs[1,0] = "<i>w</i>";	inputs[1,1] = "width";
			inputs[2,0] = "<i>h</i>";	inputs[2,1] = "height";
		}
		return inputs;
	}

	public override string[,] OutputDesc ()
	{
		if (outputs == null) {
			outputs = new string[1,2];
			outputs[0,0] = "<b>M</b>";	outputs[0,1] = "image";
		}
		return outputs;
	}
}
