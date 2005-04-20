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

		// Maintain information about the location of all the pads.
		Rectangle []			pads;
		Element				element;
		static Gdk.Pixmap		pm = new Gdk.Pixmap (null,1,1,8);

		// We use these fonts so frequently that we may as well store them permanently.
		static Font			plain = new Font (new FontFamily ("Bitstream Vera Sans"), 10, FontStyle.Regular);
		static Font			bold  = new Font (new FontFamily ("Bitstream Vera Sans"), 10, FontStyle.Bold);

		// Colors
		static 				System.Drawing.Color bg_color;
		static 				System.Drawing.Color fg_color;
		static 				System.Drawing.Color bg_color_prelight;
		static 				System.Drawing.Color fg_color_prelight;
		static				System.Drawing.Color element_bg_color;
		static				System.Drawing.Color element_fg_color;

		SizeF				name_sz;

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

			// Size of the element name in bold.
			int 		numpads;

			name_sz = graphics.MeasureString (e.Name (), bold);

			// Determine if there are more inputs or outputs in the element and allocate
			// RectangleFs for all pads.
			if (e.inputs == null && e.outputs == null) {
				numpads = 0;
			} else if (e.inputs == null) {
				numpads = e.outputs.Length;
				pads = new Rectangle[e.outputs.Length];
			} else if (e.outputs == null) {
				numpads = e.inputs.Length;
				pads = new Rectangle[e.inputs.Length];
			} else {
				if (e.inputs.Length > e.outputs.Length)
					numpads = e.inputs.Length;
				else
					numpads = e.outputs.Length;
				pads = new Rectangle[e.inputs.Length + e.outputs.Length];
			}

			// Calculate height.
			position.Height = 14 + (int) System.Math.Ceiling (name_sz.Height);

			if (numpads > 0)
				position.Height += (numpads - 1)*10 + numpads*20;

			// Calculate width.

			SizeF in_name_sz;
			SizeF out_name_sz;

			if (e.inputs != null)
				in_name_sz = graphics.MeasureString (e.LongestInputPadName (), plain);
			else
				in_name_sz = new SizeF (0,0);

			if (e.outputs != null)
				out_name_sz = graphics.MeasureString (e.LongestOutputPadName (), plain);
			else
				out_name_sz = new SizeF (0,0);

			position.Width = 64 + 20 + (int) System.Math.Ceiling (in_name_sz.Width) +
				(int) System.Math.Ceiling (out_name_sz.Width);

			// If there isn't enough space horizontally for the element name, we'll set the
			// width based on the width of the element name.
			if (position.Width < 28 + (int) System.Math.Ceiling (name_sz.Width))
				position.Width = 28 + (int) System.Math.Ceiling (name_sz.Width);

			// Create a RectangleF for each pad representing its position.
			int x;
			int y;
			int i = 0;

			// Inputs.
			if (e.inputs != null) {
				x = 0;
				y = 8 + (int) name_sz.Height;
				for (i = 0; i < e.inputs.Length; i++) {
					pads[i] = new Rectangle (x, y, 20, 20);
					y += 30;
				}
			}

			// Outputs.
			if (e.outputs != null) {
				x = position.Width - 21;
				y = 8 + (int) name_sz.Height;
				for (int j = 0; j < e.outputs.Length; j++) {
					pads[i] = new Rectangle (x, y, 20, 20);
					y += 30;
					i++;
				}
			}

			// Store a reference to the element we're drawing.
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
			Pen	border = new System.Drawing.Pen (fg_color);
			Brush 	background = new System.Drawing.SolidBrush (element_bg_color);
			Brush	white = new System.Drawing.SolidBrush (Color.White);

			// For the time being draw the element against a white background.
			context.FillRectangle (white, 0, 0, position.Width, position.Height);
			context.DrawRectangle (border, 10, 0, position.Width - 21, position.Height - 1);
			context.FillRectangle (background, 11, 1, position.Width - 22, position.Height - 2);

			// Element name.
			PointF		name_pos = new PointF (10+(position.Width-21-name_sz.Width)/2, 2);
			RectangleF	name_box = new RectangleF (name_pos, name_sz);
			Brush		text = new SolidBrush (fg_color);

			context.DrawString (element.Name (), bold, text, name_box);

			int	i = 0;

			// Draw input pads.
			if (element.inputs != null) {
				for (i = 0; i < element.inputs.Length; i++) {
					DrawInputPad (element.inputs[i], context, pads[i]);
				}
			}

			// Draw output pads.
			if (element.outputs != null) {
				for (int j = 0; j < element.outputs.Length; j++) {
					DrawOutputPad (element.outputs[j], context, pads[i]);
					i++;
				}
			}
		}

		public virtual void
		DrawPad (System.Drawing.Graphics context, RectangleF box)
		{
			PointF []	triangle = new PointF[3];
			Pen		pen = new Pen (fg_color);
			Brush		brush = new SolidBrush (element_bg_color);
			Brush		trifill = new SolidBrush (element_fg_color);

			// FIXME: This hard coded nonsense bothers me. Something more dynamic would
			// probably help when we want to draw these things backwards

			// The corners of the triangle.
			triangle[0] = new System.Drawing.PointF (box.Left+8,box.Top+5);
			triangle[1] = new System.Drawing.PointF (box.Left+8,box.Top+15);
			triangle[2] = new System.Drawing.PointF (box.Left+13, box.Top+10);

			// Draw a white circle with a black border
			context.FillEllipse (brush, box);
			context.DrawEllipse (pen, box);

			// Draw the triangle.
			context.FillPolygon (trifill, triangle);
		}

		public virtual void
		DrawInputPad (Pad pad, System.Drawing.Graphics context, RectangleF box)
		{
			Brush		text = new SolidBrush (fg_color);
			SizeF		name_len = context.MeasureString (pad.Name, plain);
			PointF		name_pos = new PointF (box.Left+27, box.Top);
			RectangleF	name_box = new RectangleF (name_pos, name_len);

			DrawPad (context, box);

			context.DrawString (pad.Name, plain, text, name_box);
		}

		public virtual void
		DrawOutputPad (Pad pad, System.Drawing.Graphics context, RectangleF box)
		{
			Brush		text = new SolidBrush (fg_color);
			SizeF		name_len = context.MeasureString (pad.Name, plain);
			PointF		name_pos = new PointF (box.Left - name_len.Width - 7, box.Top);
			RectangleF	name_box = new RectangleF (name_pos, name_len);

			DrawPad (context, box);

			context.DrawString (pad.Name, plain, text, name_box);
		}

		public virtual void
		DrawMask (Gdk.Pixmap mask)
		{
			// Fore- and background colours.
			Gdk.GC		fg = new Gdk.GC (mask);
			Gdk.GC		bg = new Gdk.GC (mask);
			Gdk.Color	white = new Gdk.Color (1,1,1);
			Gdk.Color	black = new Gdk.Color (0,0,0);

			Gdk.Colormap.System.AllocColor (ref white, true, true);
			Gdk.Colormap.System.AllocColor (ref black, true, true);

			fg.Foreground = white;
			bg.Foreground = black;

			// Main body.
			mask.DrawRectangle (bg, true, 0, 0, position.Width, position.Height);
			mask.DrawRectangle (fg, true, 10, 0, position.Width-20, position.Height);

			// Pads.
			if (pads != null) {
				foreach (Rectangle pad in pads) {
					mask.DrawArc (fg, true, pad.Left-1, pad.Top-1, 21, 21, 0, 360*64);
				}
			}
		}

		public virtual void
		Serialize (XmlTextWriter writer)
		{
			// TODO: Implement
		}

		public static void
		SetColors (Gtk.Style style)
		{
			Gdk.Color c;

			c = style.Background (Gtk.StateType.Normal);
			bg_color = System.Drawing.Color.FromArgb (c.Red / 255, c.Green / 255, c.Blue / 255);

			c = style.Foreground (Gtk.StateType.Normal);
			fg_color = System.Drawing.Color.FromArgb (c.Red / 256, c.Green / 256, c.Blue / 256);

			c = style.Background (Gtk.StateType.Prelight);
			bg_color_prelight = System.Drawing.Color.FromArgb (c.Red / 256, c.Green / 256, c.Blue / 256);

			c = style.Foreground (Gtk.StateType.Prelight);
			fg_color_prelight = System.Drawing.Color.FromArgb (c.Red / 256, c.Green / 256, c.Blue / 256);

			c = style.Background (Gtk.StateType.Insensitive);
			element_bg_color = System.Drawing.Color.FromArgb (c.Red / 256, c.Green / 256, c.Blue / 256);

			c = style.Dark (Gtk.StateType.Selected);
			element_fg_color = System.Drawing.Color.FromArgb (c.Red / 256, c.Green / 256, c.Blue / 256);
		}
	}

}
