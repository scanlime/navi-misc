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

			// Height = top-to-pad + pad-to-bottom + inter-pad-distnace*(numpads-1) + pad*(numpads)
			position.Height = 14;

			if (numpads > 0)
				position.Height += (numpads - 1)*10 + numpads*20;
			else
				position.Height += 10;


			// Width = 2*pad-to-name + between-names + width-of-longest-input-name + width-of-longest-output-name
			// The pads stick out by 10 px on either side, so we'll add 20 to account for this.
			position.Width = 64 + 20;
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
			Pen	border = new System.Drawing.Pen (Color.Chocolate);
			Brush 	background = new System.Drawing.SolidBrush (Color.BlanchedAlmond);
			Brush	white = new System.Drawing.SolidBrush (Color.White);

			context.FillRectangle (white, 0, 0, position.Width, position.Height);
			context.DrawRectangle (border, 10, 0, position.Width - 21, position.Height - 1);
			context.FillRectangle (background, 11, 1, position.Width - 22, position.Height - 2);

			float x;
			float y;

			if (element.inputs != null) {
				x = 0;
				y = 8;
				foreach (InputPad pad in element.inputs) {
					DrawInputPad (pad, context, x, y);
					y += 30;
				}
			}

			if (element.outputs != null) {
				x = position.Width - 21;
				y = 8;
				foreach (OutputPad pad in element.outputs) {
					DrawOutputPad (pad, context, x, y);
					y += 30;
				}
			}
		}

		public virtual void
		DrawPad (System.Drawing.Graphics context, float x, float y)
		{
			PointF []	triangle = new PointF[3];
			Pen		pen = new Pen (Color.Chocolate);
			Brush		brush = new SolidBrush (Color.White);
			Brush		fill = new SolidBrush (Color.Chocolate);

			// FIXME: This hard coded nonsense bothers me.
			// The corners of the triangle.
			triangle[0] = new System.Drawing.PointF (x+8,y+5);
			triangle[1] = new System.Drawing.PointF (x+8,y+15);
			triangle[2] = new System.Drawing.PointF (x+13, y+10);

			// Draw a white circle with a "Chocolate" border
			context.FillEllipse (brush, x, y, 20, 20);
			context.DrawEllipse (pen, x, y, 20, 20);

			// Draw the triangle.
			context.FillPolygon (fill, triangle);

		}

		public virtual void
		DrawInputPad (Pad pad, System.Drawing.Graphics context, float x, float y)
		{
			Font		font = new Font (new FontFamily ("Bitstream Vera Sans"), 10, FontStyle.Regular);
			Brush		text = new SolidBrush (Color.Black);
			SizeF		name_len = context.MeasureString (pad.Name, font);
			PointF		name_pos = new PointF (x+27, y);
			RectangleF	name_box = new RectangleF (name_pos, name_len);

			DrawPad (context, x, y);

			context.DrawString (pad.Name, font, text, name_box);
		}

		public virtual void
		DrawOutputPad (Pad pad, System.Drawing.Graphics context, float x, float y)
		{
			Font		font = new Font (new FontFamily ("Bitstream Vera Sans"), 10, FontStyle.Regular);
			Brush		text = new SolidBrush (Color.Black);
			SizeF		name_len = context.MeasureString (pad.Name, font);
			PointF		name_pos = new PointF (x - name_len.Width - 7, y);
			RectangleF	name_box = new RectangleF (name_pos, name_len);

			DrawPad (context, x, y);

			context.DrawString (pad.Name, font, text, name_box);
		}

		public virtual void
		Serialize (XmlTextWriter writer)
		{
			// TODO: Implement
		}
	}

}
