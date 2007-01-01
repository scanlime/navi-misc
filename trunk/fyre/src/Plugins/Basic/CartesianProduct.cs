/*
 * CartesianProduct.cs - An Element which takes two ranges of integers
 *	and outputs a matrix of the cartesian product.
 *	FIXME - we may want to have this take other "generators" as inputs,
 *	rather than just restricting it to integers
 *
 * Fyre - a generic framework for computational art
 * Copyright (C) 2004-2007 Fyre Team (see AUTHORS)
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

class CartesianProduct : Fyre.Element
{
	static Gdk.Pixbuf icon;

	public
	CartesianProduct () : base ()
	{
		inputs = new Fyre.InputPad[] {
			new Fyre.InputPad ("(x0,x1)", "X range"),
			new Fyre.InputPad ("(y0,y1)", "Y range"),
		};

		outputs = new Fyre.OutputPad[] {
			new Fyre.OutputPad ("M", "matrix"),
		};

		SetPadNumbers ();
		NewID ();
	}

	public override string
	Name ()
	{
		return "Cartesian Product";
	}

	public override string
	Category()
	{
		return "Arithmetic";
	}

	public override Gdk.Pixbuf
	Icon ()
	{
		if (icon == null)
			icon = new Gdk.Pixbuf (null, "CartesianProduct.png");
		return icon;
	}

	public override string
	Description ()
	{
		return "Computes the cartesian product\nof two integer ranges";
	}

	public override Fyre.Type[]
	Check (Fyre.Type[] t)
	{
		int x, y;

		// Check first input - 1x2 matrix of ints
		if (!((Fyre.Type.IsMatrix (t[0])) &&
		      (Fyre.Type.IsInt (Fyre.Type.GetMatrixType (t[0]))) &&
		      (Fyre.Type.GetMatrixRank (t[0]) == 1) &&
		      (Fyre.Type.GetMatrixSize (t[0])[0] == 2)))
			throw new Fyre.PadError (0, System.String.Format ("Pad type must be Matrix(Int, 1, [2]): got {0}", t[0]));

		// Check second input - 1x2 matrix of ints
		if (!((Fyre.Type.IsMatrix (t[1])) &&
		      (Fyre.Type.IsInt (Fyre.Type.GetMatrixType (t[1]))) &&
		      (Fyre.Type.GetMatrixRank (t[1]) == 1) &&
		      (Fyre.Type.GetMatrixSize (t[1])[0] == 2)))
			throw new Fyre.PadError (1, System.String.Format ("Pad type must be Matrix(Int, 1, [2]): got {0}", t[1]));

		// FIXME - we should set the output size of our matrix based on the data
		// contained in the pairs we get, but we don't actually have that capability
		// inside the Matrix class yet, since it's not really implemented at all.
		// Just hardcode some sizes for now.
		x = 20;
		y = 20;

		// Create output types
		int[] pdim = {2};
		int[] mdim = new int[] {x, y};
		return new Fyre.Type[] {
			new Fyre.Matrix (
				new Fyre.Matrix (new Fyre.Int (), 1, pdim),
				2, mdim
			),
		};
	}
}
