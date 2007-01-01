/*
 * Iterative.cs - Control flow block for feeding the output of a pipeline
 *	back into itself.
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

class Iterative : Fyre.Element
{
	static Gdk.Pixbuf icon;

	public
	Iterative () : base ()
	{
		inputs = new Fyre.InputPad[] {
			new Fyre.InputPad ("d0", "data"),
			new Fyre.InputPad ("e", "enable"),
		};

		outputs = new Fyre.OutputPad[] {
			new Fyre.OutputPad ("d1", "data"),
		};

		SetPadNumbers ();
		NewID ();
	}

	public override string
	Name ()
	{
		return "Iterative";
	}

	public override string
	Category()
	{
		return "Control Flow";
	}

	public override Gdk.Pixbuf
	Icon ()
	{
		if (icon == null)
			icon = new Gdk.Pixbuf (null, "Iterative.png");
		return icon;
	}

	public override string
	Description ()
	{
		return "Creates a feedback loop\nwhile enable is true";
	}

	public override Fyre.Type[]
	Check (Fyre.Type[] t)
	{
		if (!Fyre.Type.IsBool (t[1]))
			throw new Fyre.PadError (0, System.String.Format ("Pad type must be Bool: got {0}", t[0]));

		// return whatever we got on our input
		return new Fyre.Type[] {
			t[0],
		};
	}
}
