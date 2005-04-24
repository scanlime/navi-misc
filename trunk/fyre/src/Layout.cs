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

	public enum
	LayoutHover
	{
		None,
		Element,
		InputPad,
		OutputPad,
	};

	class Layout
	{
		Hashtable		elements;
		string			hover_element;
		string			selected_element;

		public Gdk.Rectangle	Extents
		{
			get {
				// FIXME - We should probably cache this. This will only change
				// when elements are added/removed/moved.

				// Compute the layout extents. We do this by taking a union of all
				// the rectangles of each element.
				IDictionaryEnumerator e = elements.GetEnumerator ();
				e.Reset ();
				Gdk.Rectangle extents = new Gdk.Rectangle ();

				extents.Width = 0;
				extents.Height = 0;

				// If we don't have anything on that layout, just return 0x0+0+0
				if (e.MoveNext ()) {
					CanvasElement ce = (CanvasElement) e.Value;
					extents = ConvertRect (ce.Position);
				} else {
					return extents;
				}

				while (e.MoveNext ()) {
					CanvasElement ce = (CanvasElement) e.Value;
					extents = extents.Union (ConvertRect (ce.Position));
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

		public event System.EventHandler Selected;
		protected void
		OnSelected (System.EventArgs e)
		{
			if (Selected != null)
				Selected (this, e);
		}

		public void
		Add (Element e, CanvasElement ce)
		{
			elements.Add (e.id.ToString ("d"), ce);
		}

		public void
		Remove (Element e)
		{
			elements.Remove (e.id.ToString ("d"));
		}

		public CanvasElement
		Get (Element e)
		{
			return (CanvasElement) elements[e.id.ToString ("d")];
		}

		public CanvasElement
		Get (System.Guid id)
		{
			return (CanvasElement) elements[id.ToString ("d")];
		}

		public void
		Draw (System.Drawing.Graphics context, System.Drawing.Rectangle area)
		{
			IDictionaryEnumerator e = elements.GetEnumerator ();
			e.Reset ();
			while (e.MoveNext ()) {
				CanvasElement ce = (CanvasElement) e.Value;

				if (area.Width == 0 || ce.Position.IntersectsWith (area)) {
					System.Drawing.Drawing2D.GraphicsState state = context.Save ();
					context.TranslateTransform (ce.Position.X, ce.Position.Y);
					ce.Draw (context);
					context.Restore (state);
				}
			}
		}

		public LayoutHover
		GetHoverType (int x, int y)
		{
			IDictionaryEnumerator e = elements.GetEnumerator ();
			e.Reset ();
			while (e.MoveNext ()) {
				CanvasElement ce = (CanvasElement) e.Value;

				System.Drawing.Point p = new System.Drawing.Point (x, y);

				if (ce.Position.Contains (p)) {
					int local_x = x - ce.X;
					int local_y = y - ce.Y;
					hover_element = (string) e.Key;
					ElementHover eh = ce.GetHover (local_x, local_y);
					if (eh == ElementHover.Body)      return LayoutHover.Element;
					if (eh == ElementHover.InputPad)  return LayoutHover.InputPad;
					if (eh == ElementHover.OutputPad) return LayoutHover.OutputPad;
				}
			}
			return LayoutHover.None;
		}

		public System.Guid
		GetHoverElement ()
		{
			return new System.Guid (hover_element);
		}

		public void
		MoveHoverElement (int x_offset, int y_offset)
		{
			CanvasElement ce = (CanvasElement) elements[hover_element];
			ce.Position.X += x_offset;
			ce.Position.Y += y_offset;
		}

		public void
		SetElementPosition (Element e, int x, int y)
		{
			CanvasElement ce = (CanvasElement) elements[e.id.ToString ("d")];
			ce.Position.X = x;
			ce.Position.Y = y;
		}

		public void
		SelectHoverElement ()
		{
			CanvasElement ce = (CanvasElement) elements[hover_element];
			ce.Selected = true;
			selected_element = hover_element;
			OnSelected (new System.EventArgs ());
		}

		public bool
		HasSelection ()
		{
			return (selected_element != null);
		}

		public System.Guid
		GetSelectedElement ()
		{
			return new System.Guid (selected_element);
		}

		public void
		DeselectAll ()
		{
			IDictionaryEnumerator e = elements.GetEnumerator ();
			e.Reset ();
			while (e.MoveNext ()) {
				CanvasElement ce = (CanvasElement) e.Value;
				ce.Selected = false;
			}
			selected_element = null;
			OnSelected (new System.EventArgs ());
		}
	}
}
