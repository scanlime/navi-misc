/*
 * Layout.cs -manages the graph layout
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

using System.Collections;
using System.Xml;

namespace Fyre.Editor
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
		public Pipeline		pipeline;

		// "open" connection
		string			source_element;
		int			source_pad;
		int			conn_x;
		int			conn_y;

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

				// If we don't have anything on that layout, just return 0x0+0+0.  We
				// recognize that width=0 means there's no document.
				if (e.MoveNext ()) {
					CanvasElement ce = (CanvasElement) e.Value;
					extents = Util.SDRectToGdk (ce.Position);
				} else {
					return extents;
				}

				while (e.MoveNext ()) {
					CanvasElement ce = (CanvasElement) e.Value;
					extents = extents.Union (Util.SDRectToGdk (ce.Position));
				}

				return extents;
			}
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

		public event System.EventHandler Changed;
		public void
		OnChanged (System.EventArgs e)
		{
			if (Changed != null)
				Changed (this, e);
		}

		public void
		Add (Element e, CanvasElement ce)
		{
			elements.Add (e.id.ToString ("d"), ce);

			OnChanged (new System.EventArgs ());
		}

		public void
		Remove (Element e)
		{
			if (e.id.ToString ("d") == selected_element)
			{
				CanvasElement ce = Get (e);
				ce.Selected = false;
				selected_element = null;
				OnSelected (new System.EventArgs ());
			}

			elements.Remove (e.id.ToString ("d"));

			OnChanged (new System.EventArgs ());
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
			System.Drawing.Pen pen = new System.Drawing.Pen (System.Drawing.Color.Black);

			// Draw connections before elements, so the lines appear under elements
			foreach (PadConnection connection in pipeline.connections) {
				CanvasElement source = (CanvasElement) elements[connection.source_element.ToString ("d")];
				CanvasElement sink   = (CanvasElement) elements[connection.sink_element.ToString ("d")];

				System.Drawing.Rectangle conn_rect = new System.Drawing.Rectangle ();

				int x1, x2, y1, y2;

				source.GetOutputPosition (connection.source_pad, out x1, out y1);
				sink.GetInputPosition    (connection.sink_pad,   out x2, out y2);

				if (x1 < x2) {
					conn_rect.X      = x1;
					conn_rect.Width  = x2 - x1;
				} else {
					conn_rect.X      = x2;
					conn_rect.Width  = x1 - x2;
				}
				if (y1 < y2) {
					conn_rect.Y      = y1;
					conn_rect.Height = y2 - y1;
				} else {
					conn_rect.Y      = y2;
					conn_rect.Height = y1 - y2;
				}

				if (area.IntersectsWith (conn_rect))
					context.DrawLine (pen, x1, y1, x2, y2);
			}

			// In-progress connection
			if (source_element != null) {
				CanvasElement source = (CanvasElement) elements[source_element];

				int x1, y1;
				source.GetOutputPosition (source_pad, out x1, out y1);

				context.DrawLine (pen, x1, y1, conn_x, conn_y);
			}

			// Finally, draw elements
			foreach (DictionaryEntry entry in elements) {
				CanvasElement ce = (CanvasElement) entry.Value;

				// Perform visibility culling.  This significantly speeds up drawing.
				// Not sure why System.Drawing doesn't do this internally, but oh well.
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
			foreach (DictionaryEntry entry in elements) {
				CanvasElement ce = (CanvasElement) entry.Value;

				System.Drawing.Point p = new System.Drawing.Point (x, y);

				if (ce.Position.Contains (p)) {
					int local_x = x - ce.X;
					int local_y = y - ce.Y;
					hover_element = (string) entry.Key;
					ElementHover eh = ce.GetHover (local_x, local_y);

					// Translate element hovers into layout hovers.
					// FIXME - This seems a little icky -- might want to just have a
					// single hover enum.
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

		public int
		GetHoverPad ()
		{
			CanvasElement ce = (CanvasElement) elements[hover_element];
			return ce.HoverPad;
		}

		public void
		MoveHoverElement (int x_offset, int y_offset)
		{
			CanvasElement ce = (CanvasElement) elements[hover_element];
			ce.Position.X += x_offset;
			ce.Position.Y += y_offset;

			OnChanged (new System.EventArgs ());
		}

		public void
		SetElementPosition (Element e, int x, int y)
		{
			CanvasElement ce = (CanvasElement) elements[e.id.ToString ("d")];
			ce.Position.X = x;
			ce.Position.Y = y;

			OnChanged (new System.EventArgs ());
		}

		public void
		SelectHoverElement ()
		{
			CanvasElement ce = (CanvasElement) elements[hover_element];
			ce.Selected = true;
			selected_element = hover_element;

			OnSelected (new System.EventArgs ());
			OnChanged (new System.EventArgs ());
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
			foreach (DictionaryEntry entry in elements) {
				CanvasElement ce = (CanvasElement) entry.Value;
				ce.Selected = false;
			}
			selected_element = null;

			OnSelected (new System.EventArgs ());
			OnChanged (new System.EventArgs ());
		}

		public void
		Serialize (XmlTextWriter writer)
		{
			// run through all the CanvasElements, serializing them
			foreach (DictionaryEntry entry in elements) {
				CanvasElement e = (CanvasElement) entry.Value;
				e.Write (writer);
			}
		}

		public void
		DeSerialize (XmlTextReader reader)
		{
			while (reader.Read () && reader.NodeType == XmlNodeType.Element && reader.Depth == 2) {
				CanvasElement ce = CanvasElement.CreateFromXml (reader, pipeline);

				elements.Add (ce.element.id.ToString ("d"), ce);
			}
			OnChanged (new System.EventArgs ());
		}

		public void
		BeginConnection ()
		{
			source_element = hover_element;
			CanvasElement ce = (CanvasElement) elements[hover_element];
			source_pad = ce.HoverPad;

			ce.GetOutputPosition (source_pad, out conn_x, out conn_y);
		}

		public void
		MoveConnection (int x, int y)
		{
			conn_x = x;
			conn_y = y;
			OnChanged (new System.EventArgs ());
		}

		public PadConnection
		GetConnection ()
		{
			PadConnection pc = new PadConnection ();
			pc.source_element = new System.Guid (source_element);
			pc.source_pad = source_pad;

			pc.sink_element = new System.Guid (hover_element);
			CanvasElement ce = (CanvasElement) elements[hover_element];
			pc.sink_pad = ce.HoverPad;

			return pc;
		}

		public void
		EndConnection ()
		{
			source_element = null;
			OnChanged (new System.EventArgs ());
		}
	}
}
