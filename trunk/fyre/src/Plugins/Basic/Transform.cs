/*
 * Transform.cs - An Element which takes translation, rotation,
 *	scale, and aspect ratio and produces a matrix representing
 *	those transformations.
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

using System.Xml;

class TwoDTransform : Fyre.Element
{
	static Gdk.Pixbuf icon;
	static string[,] inputs;
	static string[,] outputs;

	public override string Name ()
	{
		return "2D Transform";
	}

	public override string Category()
	{
		return "Arithmetic";
	}

	public override Gdk.Pixbuf Icon ()
	{
		if (icon == null)
			icon = new Gdk.Pixbuf (null, "TwoDTransform.png");
		return icon;
	}

	public override string Description ()
	{
		return "Creates an 3x3 matrix out\nof a set of 2D\ntransformations";
	}

	public override string[,] InputDesc ()
	{
		if (inputs == null) {
			inputs = new string[4,2];
			inputs[0,0] = "<i>r</i>";	inputs[0,1] = "rotation";
			inputs[1,0] = "<i>a</i>";	inputs[1,1] = "aspect ratio";
			inputs[2,0] = "<i>z</i>";	inputs[2,1] = "zoom";
			inputs[3,0] = "<i>t</i>";	inputs[3,1] = "translation";
		}
		return inputs;
	}

	public override string[,] OutputDesc ()
	{
		if (outputs == null) {
			outputs = new string[1,2];
			outputs[0,0] = "<b>M</b>";	outputs[0,1] = "matrix";
		}
		return outputs;
	}

	public override void Serialize (XmlWriter writer)
	{
		base.Serialize (writer);
	}

	public override void DeSerialize (XmlReader reader)
	{
		base.DeSerialize (reader);
	}
}
