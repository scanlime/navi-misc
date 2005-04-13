/*
 * StochasticBlur.cs - An Element which takes a blur radius and ratio
 *	and perturbs it's input point randomly according to the
 *	probability given in the ratio.
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

class StochasticBlur : Fyre.Element
{
	static Gdk.Pixbuf icon;

	public
	StochasticBlur () : base ()
	{
		inputs = new Fyre.InputPad[] {
			new Fyre.InputPad ("v<sub>0</sub>", "point"),
			new Fyre.InputPad ("p", "ratio"),
			new Fyre.InputPad ("r", "radius"),
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
		return "Stochastic Blur";
	}

	public override string
	Category()
	{
		return "Miscellaneous";
	}

	public override Gdk.Pixbuf
	Icon ()
	{
		if (icon == null)
			icon = new Gdk.Pixbuf (null, "StochasticBlur.png");
		return icon;
	}

	public override string
	Description ()
	{
		return "Creates a blur by\nstochastically perturbing\na point within the\ngiven radius";
	}

	public override bool
	Check (Fyre.Type[] t, out Fyre.Type[] to)
	{
		to = null;
		// FIXME - implement
		return true;
	}
}
