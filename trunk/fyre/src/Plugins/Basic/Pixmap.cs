/*
 * Pixmap.cs - An Element which implements the histogram imager algorithm.
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

class Pixmap : Fyre.Element
{
	static Gdk.Pixbuf icon;

	public
	Pixmap ()
	{
		inputs = new Fyre.InputPad[] {
			new Fyre.InputPad ("w", "width"),
			new Fyre.InputPad ("h", "height"),
			new Fyre.InputPad ("(x,y)", "point"),
			new Fyre.InputPad ("c", "color"),
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
		switch (pad) {
		case 0:
		case 1:
			return (d.type is Fyre.Int);
		case 2:
			// FIXME - check for Matrix(Int, 1, 2)
			return true;
		case 3:
			// FIXME - check for various color representations
			return true;
		}
		return true;
	}
}
