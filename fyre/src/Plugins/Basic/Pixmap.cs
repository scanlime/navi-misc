/*
 * Pixmap.cs - An Element which implements the histogram imager algorithm.
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

class Pixmap : Fyre.Element
{
	static Gdk.Pixbuf icon;

	public
	Pixmap () : base ()
	{
		inputs = new Fyre.InputPad[] {
			new Fyre.InputPad ("w", "width"),
			new Fyre.InputPad ("h", "height"),
			new Fyre.InputPad ("p", "point"),
			new Fyre.InputPad ("c", "color"),
		};

		flags = Fyre.ElementFlags.Output;

		SetPadNumbers ();
		NewID ();
	}

	public override string
	Name ()
	{
		return "Pixmap";
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
			icon = new Gdk.Pixbuf (null, "Pixmap.png");
		return icon;
	}

	public override string
	Description ()
	{
		return "Simple pixmap image";
	}

	public override Fyre.Type[]
	Check (Fyre.Type[] t)
	{
		// Check that w & h are int
		if (!(Fyre.Type.IsInt (t[0])))
			throw new Fyre.PadError (0, System.String.Format ("Pad type must be Int: got {0}", t[0]));
		if (!(Fyre.Type.IsInt (t[1])))
			throw new Fyre.PadError (1, System.String.Format ("Pad type must be Int: got {0}", t[1]));

		// Check that the point is an int pair
		if (!((Fyre.Type.IsMatrix (t[2])) &&
		      (Fyre.Type.GetMatrixRank (t[2]) == 1) &&
		      (Fyre.Type.GetMatrixSize (t[2])[0] == 2) &&
		      (Fyre.Type.IsInt (Fyre.Type.GetMatrixType (t[2])))))
			throw new Fyre.PadError (2, System.String.Format ("Pad type must be Matrix(Int, 1, [2]): got {0}", t[2]));

		// FIXME - check color

		return null;
	}
}
