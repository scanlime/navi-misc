/*
 * MatrixMultply.cs - An Element which takes a vector and a matrix
 *	and multiplies the two.
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

class MatrixMultiply : Fyre.Element
{
	static Gdk.Pixbuf icon;

	public
	MatrixMultiply ()
	{
		inputs = new Fyre.InputPad[] {
			new Fyre.InputPad ("v<sub>0</sub>", "point"),
			new Fyre.InputPad ("M", "matrix"),
		};

		outputs = new Fyre.OutputPad[] {
			new Fyre.OutputPad ("v<sub>1</sub>", "new point"),
		};

		SetPadNumbers ();
		NewCanvasElement ();
		NewID ();
	}

	public override string
	Name ()
	{
		return "Matrix Multiply";
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
			icon = new Gdk.Pixbuf (null, "MatrixMultiply.png");
		return icon;
	}

	public override string
	Description ()
	{
		return "Multiplies a vector\nand a matrix";
	}

	public override void
	Serialize (XmlTextWriter writer)
	{
		base.Serialize (writer);
	}

	public override void
	DeSerialize (XmlTextReader reader)
	{
		base.DeSerialize (reader);
	}

	public override bool
	Check (int pad, Fyre.Data d)
	{
		// FIXME - hmm, not sure how to handle this. we need to check that
		// 0 has rank 1, but we also need to enforce that the size of v is
		// the same as the height of M.
		switch (pad) {
		case 0:
			return true;
		case 1:
			return true;
		}
		return true;
	}
}
