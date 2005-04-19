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
				// FIXME - We should probably cache this. This will only change
				// when elements are added/removed/moved.

				// Compute the layout extents. We do this by taking a union of all
				// the rectangles of each element.
				IDictionaryEnumerator e = elements.GetEnumerator ();
				e.Reset ();
				Gdk.Rectangle extents;

				// If we don't have anything on that layout, just return 0x0+0+0
				extents.Width = extents.Height = 0;
				extents.X = extents.Y = 0;
				if (e.MoveNext ()) {
					CanvasElement ce = (CanvasElement) e.Value;
					extents = ConvertRect (ce.position);
				} else {
					return extents;
				}

				while (e.MoveNext ()) {
					CanvasElement ce = (CanvasElement) e.Value;
					extents = extents.Union (ConvertRect (ce.position));
				}

				return extents;
			}
		}

		Gdk.Rectangle
		ConvertRect (System.Drawing.Rectangle r)
		{
			Gdk.Rectangle ret;
			ret.X      = r.X;
			ret.Y      = r.Y;
			ret.Width  = r.Width;
			ret.Height = r.Height;
			return ret;
		}

		public
		Layout ()
		{
			elements = new Hashtable ();
		}

		public void
		Add (Element e, CanvasElement ce)
		{
			elements.Add (e.id.ToString ("d"), ce);
		}

		public void
		Draw (System.Drawing.Graphics context)
		{
			IDictionaryEnumerator e = elements.GetEnumerator ();
			e.Reset ();
			while (e.MoveNext ()) {
				CanvasElement ce = (CanvasElement) e.Value;
				ce.Draw (context);
			}
		}
	}
}
