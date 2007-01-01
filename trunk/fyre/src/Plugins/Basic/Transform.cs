/*
 * Transform.cs - An Element which takes translation, rotation,
 *	scale, and aspect ratio and produces a matrix representing
 *	those transformations.
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

class TwoDTransform : Fyre.Element
{
	static Gdk.Pixbuf icon;

	public
	TwoDTransform () : base ()
	{
		inputs = new Fyre.InputPad[] {
			new Fyre.InputPad ("r", "rotation"),
			new Fyre.InputPad ("a", "aspect ratio"),
			new Fyre.InputPad ("z", "zoom"),
			new Fyre.InputPad ("t", "translation"),
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
		return "2D Transform";
	}

	public override string
	Category()
	{
		return "Geometry";
	}

	public override Gdk.Pixbuf
	Icon ()
	{
		if (icon == null)
			icon = new Gdk.Pixbuf (null, "TwoDTransform.png");
		return icon;
	}

	public override string
	Description ()
	{
		return "Creates an 3x3 matrix out\nof a set of 2D\ntransformations";
	}

	public override Fyre.Type[]
	Check (Fyre.Type[] t)
	{
		// All our pads are floats
		for (int i = 0; i < 4; i++)
			if (!Fyre.Type.IsFloat (t[i]))
				throw new Fyre.PadError (i, System.String.Format ("Pad type must be Float: got {0}", t[i]));

		int[] mdim = {3, 3};
		return new Fyre.Type[] {
			new Fyre.Matrix (new Fyre.Float (), 2, mdim),
		};
	}
}
