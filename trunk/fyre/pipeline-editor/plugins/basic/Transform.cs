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

using Gdk;

class TwoDTransform : Element
{
	private static Gdk.Pixbuf icon;

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
			icon = new Gdk.Pixbuf ("/usr/share/fyre/2.0/TwoDTransform.png");
		return icon;
	}

	public override string Description ()
	{
		return "Creates an 3x3 matrix out of a set of 2D transformations";
	}

	public override string InputDesc ()
	{
		return "<i>rotation</i>\n" + "<i>aspect</i>\n" + "<i>zoom</i>\n" + "<i>offset</i>";
	}

	public override string OutputDesc ()
	{
		return "<b>M</b>:\tmatrix";
	}
}
