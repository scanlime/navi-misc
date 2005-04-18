/*
 * Layout.cs -manages the graph layout
 *
 * Fyre - a generic framework for computational art
 * Copyright (C) 2004-2005 Fyre Team (see AUTHORS)
 *
 * Copyright (C) 2005 David Trowbridge
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

using System.Collections;

namespace Fyre
{

	class Layout
	{
		Hashtable		elements;

		public Gdk.Rectangle	Extents
		{
			get {
				// Compute the layout extents. We do this by taking a union of all
				// the rectangles of each element.
				IDictionaryEnumerator e = elements.GetEnumerator ();
				e.Reset ();
				Gdk.Rectangle extents;

				// If we don't have anything on that layout, just return 0x0+0+0
				extents.Width = extents.Height = 0;
				extents.X = extents.Y = 0;
				if (e.MoveNext ())
					extents = (Gdk.Rectangle) e.Current;
				else
					return extents;

				while (e.MoveNext ()) {
					Gdk.Rectangle r = (Gdk.Rectangle) e.Current;
					extents = extents.Union (r);
				}

				return extents;
			}
		}

		public
		Layout ()
		{
			elements = new Hashtable ();
		}

		public void
		Draw (System.Drawing.Graphics context)
		{
		}
	}
}
