/*
 * CanvasElement - flyweight which handles drawing of an Element
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

using System.Drawing;
using System.Xml;

namespace Fyre
{
	/*
	 * The basic structure of an element on the canvas has 3 pieces:
	 * 	name
	 * 	pads
	 * 	element-specific data
	 *
	 * Name and pads are provided by this class. If an element requires
	 * custom drawings (such as equations, etc), it will subclass
	 * CanvasElement.
	 *
	 * y-dimensions:
	 * 	name:			20px
	 * 	pad:			20px
	 * 	inter-pad distance:	10px
	 * 	top-to-pad:		8px
	 * 	pad-to-bottom:		6px
	 *
	 * x-dimensions:
	 * 	minimum size is either name, extra-data or pads
	 * 	pad-to-name:		7px
	 * 	between-names:		50px
	 */

	public class CanvasElement
	{
		// Width and height are maintained by the internal layout system.
		// X & Y are maintained by the global layout system. We'll probably
		// want get/set operators on this to trigger redraws, etc.
		public Rectangle		position;
		private Element			element;
		static private Gdk.Pixmap	pm = new Gdk.Pixmap (null,1,1,8);

		public int
		Width
		{
			get { return position.Width; }
		}

		public int
		Height
		{
			get { return position.Height; }
		}

		public
		CanvasElement (Element e, Gdk.Drawable drawable)
		{
			Graphics	graphics = Gtk.DotNet.Graphics.FromDrawable (drawable);
			Font		font = new Font (new FontFamily ("Bitstream Vera Sans"), 10, FontStyle.Regular);
			int 		numpads;

			if (e.inputs == null && e.outputs == null) {
				numpads = 0;
			} else if (e.inputs == null) {
				numpads = e.outputs.Length;
			} else if (e.outputs == null) {
				numpads = e.inputs.Length;
			} else {
				if (e.inputs.Length > e.outputs.Length)
					numpads = e.inputs.Length;
				else
					numpads = e.outputs.Length;
			}

			// FIXME: These height and width calculations may be overestimating the necessary size of an element.

			// Height = top-to-pad + pad-to-bottom + inter-pad-distnace*(numpads-1) + pad*(numpads)

			// FIXME - font.Height is the line spacing of the font, but we probably
			// just want the vertical extents. Guess we'll just have to see how it
			// works out, for now.
			position.Height = 14 + (numpads - 1)*10 + numpads*20;

			// Width = 2*pad-to-name + between-names + width-of-longest-input-name + width-of-longest-output-name
			position.Width = 64;
			if (e.inputs != null)
				position.Width += (int) System.Math.Ceiling (graphics.MeasureString (e.LongestInputPadName (), font).Width);
			if (e.outputs != null)
				position.Width += (int) System.Math.Ceiling (graphics.MeasureString (e.LongestOutputPadName (), font).Width);

			element = e;
		}

		// When we're drawing on the main canvas, we render at full size.
		// However, we also want to be able to render smaller for the
		// navigation box, so there are two versions.
		public virtual void
		Draw (System.Drawing.Graphics context, float zoom)
		{
			// FIXME
		}

		public virtual void
		Draw (System.Drawing.Graphics context)
		{
			// FIXME
			System.Drawing.Pen	border = new System.Drawing.Pen (Color.Chocolate);
			System.Drawing.Brush 	background = new System.Drawing.SolidBrush (Color.BlanchedAlmond);
			System.Drawing.Brush	white = new System.Drawing.SolidBrush (Color.White);

			context.FillRectangle (white, 0, 0, position.Width, position.Height);
			context.DrawRectangle (border, 10, 0, position.Width - 21, position.Height - 1);
			context.FillRectangle (background, 11, 1, position.Width - 22, position.Height - 2);

			int x = 0;
			int y = 8;
			for (int i = 0; i < element.inputs.Length; i++) {
				DrawPad (border, context, x, y);
				y += 30;
			}

			x = position.Width - 21;
			y = 8;
			for (int i = 0; i < element.outputs.Length; i++) {
				DrawPad (border, context, x, y);
				y += 30;
			}
		}

		public virtual void
		DrawPad (System.Drawing.Pen pen, System.Drawing.Graphics context, int x, int y)
		{
			System.Drawing.Point []	triangle = new System.Drawing.Point[3];
			System.Drawing.Brush	brush = new System.Drawing.SolidBrush (Color.White);

			triangle[0] = new System.Drawing.Point (x+8,y+5);
			triangle[1] = new System.Drawing.Point (x+8,y+15);
			triangle[2] = new System.Drawing.Point (x+13, y+10);

			context.DrawEllipse (pen, x, y, 20, 20);
			context.FillEllipse (brush, x+1, y+1, 19, 19);
			context.DrawPolygon (pen, triangle);
		}

		public virtual void
		Serialize (XmlTextWriter writer)
		{
			// TODO: Implement
		}
	}

}
