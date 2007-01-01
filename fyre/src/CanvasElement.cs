/*
 * CanvasElement - flyweight which handles drawing of an Element
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

using System.Drawing;
using System.Xml;

namespace Fyre.Editor
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
	public enum
	ElementHover
	{
		None,
		Body,
		InputPad,
		OutputPad,
	};

	public class CanvasElement
	{
		// Width and height are maintained by the internal layout system.
		// X & Y are maintained by the global layout system. We'll probably
		// want get/set operators on this to trigger redraws, etc.
		public Rectangle		Position;

		// Maintain information about the location of all the pads.
		Rectangle []			input_pads;
		Rectangle []			output_pads;
		public Element			element;
		static public Gdk.Drawable	Drawable;
		public int			HoverPad;

		// We use these fonts so frequently that we may as well store them permanently.
		static Font			plain = new Font (new FontFamily ("Bitstream Vera Sans"), 10, FontStyle.Regular);
		static Font			bold  = new Font (new FontFamily ("Bitstream Vera Sans"), 10, FontStyle.Bold);

		// Colors
		static System.Drawing.Color	bg_color;
		static System.Drawing.Color 	fg_color;
		static System.Drawing.Color 	bg_color_prelight;
		static System.Drawing.Color 	fg_color_prelight;
		static System.Drawing.Color 	element_bg_color;
		static System.Drawing.Color 	element_fg_color;

		// Size of the name of this element
		SizeF				name_sz;

		// Selection state
		public bool			Selected;

		public int
		X
		{
			get { return Position.X; }
		}

		public int
		Y
		{
			get { return Position.Y; }
		}

		public int
		Width
		{
			get { return Position.Width; }
		}

		public int
		Height
		{
			get { return Position.Height; }
		}

		public
		CanvasElement (Element e)
		{
			Graphics	graphics = Gtk.DotNet.Graphics.FromDrawable (Drawable);

			// Size of the element name in bold.
			int 		numpads;

			name_sz = graphics.MeasureString (e.Name (), bold);

			// Determine if there are more inputs or outputs in the element and allocate
			// RectangleFs for all pads.
			if (e.inputs == null && e.outputs == null) {
				numpads = 0;
			} else if (e.inputs == null) {
				numpads = e.outputs.Length;
				output_pads = new Rectangle[e.outputs.Length];
			} else if (e.outputs == null) {
				numpads = e.inputs.Length;
				input_pads = new Rectangle[e.inputs.Length];
			} else {
				if (e.inputs.Length > e.outputs.Length)
					numpads = e.inputs.Length;
				else
					numpads = e.outputs.Length;
				input_pads  = new Rectangle[e.inputs.Length];
				output_pads = new Rectangle[e.outputs.Length];
			}

			// Calculate height.
			Position.Height = 14 + (int) System.Math.Ceiling (name_sz.Height);

			if (numpads > 0)
				Position.Height += (numpads - 1)*10 + numpads*20;

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

			Position.Width = 64 + 20 + (int) System.Math.Ceiling (in_name_sz.Width) +
				(int) System.Math.Ceiling (out_name_sz.Width);

			// If there isn't enough space horizontally for the element name, we'll set the
			// width based on the width of the element name.
			if (Position.Width < 28 + (int) System.Math.Ceiling (name_sz.Width))
				Position.Width = 28 + (int) System.Math.Ceiling (name_sz.Width);

			// Create a RectangleF for each pad representing its position.
			int x;
			int y;
			int i = 0;

			// Inputs.
			if (e.inputs != null) {
				x = 0;
				y = 8 + (int) name_sz.Height;
				for (i = 0; i < e.inputs.Length; i++) {
					input_pads[i] = new Rectangle (x, y, 20, 20);
					y += 30;
				}
			}

			// Outputs.
			if (e.outputs != null) {
				x = Position.Width - 21;
				y = 8 + (int) name_sz.Height;
				for (int j = 0; j < e.outputs.Length; j++) {
					output_pads[j] = new Rectangle (x, y, 20, 20);
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
			Pen	sborder = new System.Drawing.Pen (element_fg_color);
			Brush 	background = new System.Drawing.SolidBrush (element_bg_color);

			context.FillRectangle (background, 11, 1, Position.Width - 22, Position.Height - 2);
			if (Selected) {
				context.DrawRectangle (sborder, 10, 0, Position.Width - 21, Position.Height - 1);
				context.DrawRectangle (sborder, 11, 1, Position.Width - 23, Position.Height - 3);
			} else {
				context.DrawRectangle (border, 10, 0, Position.Width - 21, Position.Height - 1);
			}

			// Element name.
			PointF		name_pos = new PointF (10+(Position.Width-21-name_sz.Width)/2, 2);
			RectangleF	name_box = new RectangleF (name_pos, name_sz);
			Brush		text = new SolidBrush (fg_color);
			Brush		stext = new SolidBrush (element_fg_color);

			if (Selected)
				context.DrawString (element.Name (), bold, stext, name_box);
			else
				context.DrawString (element.Name (), bold, text, name_box);

			// Draw input pads.
			if (element.inputs != null) {
				for (int i = 0; i < element.inputs.Length; i++) {
					DrawInputPad (element.inputs[i], context, input_pads[i]);
				}
			}

			// Draw output pads.
			if (element.outputs != null) {
				for (int i = 0; i < element.outputs.Length; i++) {
					DrawOutputPad (element.outputs[i], context, output_pads[i]);
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

			// Assign pixel values for the colors statically-
			// we're drawing on a 1bpp mask image, so this is easy.
			// In fact, allocating these from the colormap is incorrect
			// because this mask image isn't at the same depth as our
			// window. Allocating these from the colormap happens to work
			// at some depths, but that isn't reliable. In particular, it
			// would break at 16bpp.
			white.Pixel = 1;
			black.Pixel = 0;

			fg.Foreground = white;
			bg.Foreground = black;

			// Main body.
			mask.DrawRectangle (bg, true, 0, 0, Position.Width, Position.Height);
			mask.DrawRectangle (fg, true, 10, 0, Position.Width-20, Position.Height);

			// Pads.
			if (input_pads != null) {
				foreach (Rectangle pad in input_pads) {
					mask.DrawArc (fg, true, pad.Left-1, pad.Top-1, 21, 21, 0, 360*64);
				}
			}
			if (output_pads != null) {
				foreach (Rectangle pad in output_pads) {
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

		public ElementHover
		GetHover (int x, int y)
		{
			if (input_pads != null) {
				for (int i = 0; i < input_pads.Length; i++) {
					Rectangle pad = input_pads[i];
					if ((x >= pad.X) &&
					    (x <= pad.X + pad.Width) &&
				    	    (y >= pad.Y) &&
				    	    (y <= pad.Y + pad.Height)) {
						HoverPad = i;
						return ElementHover.InputPad;
					}
				}
			}
			if (output_pads != null) {
				for (int i = 0; i < output_pads.Length; i++) {
					Rectangle pad = output_pads[i];
					if ((x >= pad.X) &&
				    	    (x <= pad.X + pad.Width) &&
				    	    (y >= pad.Y) &&
				    	    (y <= pad.Y + pad.Height)) {
						HoverPad = i;
						return ElementHover.OutputPad;
					}
				}
			}
			if (x >= 10 && x <= Position.Width - 10)
				return ElementHover.Body;
			return ElementHover.None;
		}

		public void
		Write (XmlTextWriter writer)
		{
			writer.WriteStartElement (null, "element", null);

			// element Guid
			writer.WriteStartAttribute (null, "id", null);
			writer.WriteString (element.id.ToString ());
			writer.WriteEndAttribute ();

			// X position
			writer.WriteStartAttribute (null, "x", null);
			writer.WriteString (X.ToString ());
			writer.WriteEndAttribute ();

			// Y position
			writer.WriteStartAttribute (null, "y", null);
			writer.WriteString (Y.ToString ());
			writer.WriteEndAttribute ();

			writer.WriteEndElement ();
		}

		public static CanvasElement
		CreateFromXml (XmlTextReader reader, Pipeline pipeline)
		{
			string id;
			int x, y;

			reader.MoveToAttribute ("id");
			id = reader.Value;

			reader.MoveToAttribute ("x");
			x = System.Int32.Parse (reader.Value);

			reader.MoveToAttribute ("y");
			y = System.Int32.Parse (reader.Value);

			Element e = (Element) pipeline.element_store[id];

			CanvasElement ce = new CanvasElement (e);
			ce.Position.X = x;
			ce.Position.Y = y;

			return ce;
		}

		// These functions are used by the Layout to draw PadConnections.
		// They return the position of the pad in global space
		public void
		GetInputPosition (int pad, out int x, out int y)
		{
			Rectangle r = input_pads[pad];
			x = Position.X + r.X + (r.Width  / 2);
			y = Position.Y + r.Y + (r.Height / 2);
		}

		public void
		GetOutputPosition (int pad, out int x, out int y)
		{
			Rectangle r = output_pads[pad];
			x = Position.X + r.X + (r.Width  / 2);
			y = Position.Y + r.Y + (r.Height / 2);
		}
	}

}
