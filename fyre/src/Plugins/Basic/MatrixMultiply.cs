/*
 * MatrixMultply.cs - An Element which takes a vector and a matrix
 *	and multiplies the two.
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

class MatrixMultiply : Fyre.Element
{
	static Gdk.Pixbuf icon;
	static string[,] inputs;
	static string[,] outputs;

	public override string Name ()
	{
		return "Matrix Multiply";
	}

	public override string Category()
	{
		return "Arithmetic";
	}

	public override Gdk.Pixbuf Icon ()
	{
		if (icon == null)
			icon = new Gdk.Pixbuf (null, "MatrixMultiply.png");
		return icon;
	}

	public override string Description ()
	{
		return "Multiplies a vector\nand a matrix";
	}

	public override string[,] InputDesc ()
	{
		if (inputs == null) {
			inputs = new string[2,2];
			inputs[0,0] = "<i>v<sub>0</sub></i>";	inputs[0,1] = "point";
			inputs[1,0] = "<b>M</b>";		inputs[1,1] = "matrix";
		}
		return inputs;
	}

	public override string[,] OutputDesc ()
	{
		if (outputs == null) {
			outputs = new string[1,2];
			outputs[0,0] = "<i>v<sub>1</sub></i>";	outputs[0,1] = "new point";
		}
		return outputs;
	}

	public override void Serialize (XmlWriter writer)
	{
	}

	public override void DeSerialize (XmlReader reader)
	{
	}
}
