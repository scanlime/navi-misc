/*
 * Util.cs - Miscellaneous utility functions
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

namespace Fyre.Editor
{
	static class Util
	{
		public static Gdk.Rectangle
		SDRectToGdk (System.Drawing.Rectangle r)
		{
			Gdk.Rectangle ret = new Gdk.Rectangle ();
			ret.X      = r.X;
			ret.Y      = r.Y;
			ret.Width  = r.Width;
			ret.Height = r.Height;
			return ret;
		}

		public static System.Drawing.Rectangle
		GdkRectToSD (Gdk.Rectangle r)
		{
			System.Drawing.Rectangle ret = new System.Drawing.Rectangle ();
			ret.X      = r.X;
			ret.Y      = r.Y;
			ret.Width  = r.Width;
			ret.Height = r.Height;
			return ret;
		}
	}
}
