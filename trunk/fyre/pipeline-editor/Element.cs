/*
 * Element.cs - abstract data type defining an Element
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

public class Element
{
	public string		Name;
	public string		Category;
	public string		Description;
	public Gdk.Pixbuf	Icon;

	/* These are expected to be in pango markup, for any
	 * necessary subscripts, UTF-8 trickery, etc
	 */
	public string		InputDesc;
	public string		OutputDesc;
}
