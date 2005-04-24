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

namespace Fyre.Canvas
{
	public enum
	ElementHover
	{
		None,
		Body,
		InputPad,
		OutputPad,
	};

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
	public class Element
	{
		// Width and height are maintained by the internal layout system.
		// X & Y are maintained by the global layout system. We'll probably
		// want get/set operators on this to trigger redraws, etc.
		public Rectangle	Position;

		// Maintain information about the location of all the pads.
		Pad []			input_pads;
		Pad []			output_pads;
		Element			element;
		static Gdk.Pixmap	pm = new Gdk.Pixmap (null,1,1,8);

		// Size of the name of this element
		SizeF			name_sz;

		// Selection state
		public bool		Selected;


		public
		Element (Fyre.Element e, Gdk.Drawable drawable)
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
					input_pads[i] = new Rectangle (x, y, 20, 20);
					y += 30;
				}
			}

			// Outputs.
			if (e.outputs != null) {
				x = position.Width - 21;
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

			context.FillRectangle (background, 11, 1, position.Width - 22, position.Height - 2);
			if (Selected) {
				context.DrawRectangle (sborder, 10, 0, position.Width - 21, position.Height - 1);
				context.DrawRectangle (sborder, 11, 1, position.Width - 23, position.Height - 3);
			} else {
				context.DrawRectangle (border, 10, 0, position.Width - 21, position.Height - 1);
			}

			// Element name.
			PointF		name_pos = new PointF (10+(position.Width-21-name_sz.Width)/2, 2);
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

		public ElementHover
		GetHover (int x, int y)
		{
			if (input_pads != null) {
				foreach (Rectangle pad in input_pads) {
					if ((x >= pad.X) &&
					    (x <= pad.X + pad.Width) &&
				    	    (y >= pad.Y) &&
				    	    (y <= pad.Y + pad.Height))
						return ElementHover.InputPad;
				}
			}
			if (output_pads != null) {
				foreach (Rectangle pad in output_pads) {
					if ((x >= pad.X) &&
				    	    (x <= pad.X + pad.Width) &&
				    	    (y >= pad.Y) &&
				    	    (y <= pad.Y + pad.Height))
						return ElementHover.OutputPad;
				}
			}
			if (x >= 10 && x <= position.Width - 10)
				return ElementHover.Body;
			return ElementHover.None;
		}
	}

	/*** Element Drawing Primitives ***/

	// FIXME Should these two structs maybe be private? Do we need them outside
	// of drawing stuff on the canvas?

	// Our fonts.
	// FIXME Might be a good idea to have something to fall back on if they
	// don't have Bitstream Vera fonts.
	public sealed struct
	Font
	{
		static public Font plain = new Font (new FontFamily ("Bitstream Vera Sans"), 10, FontStyle.Regular);
		static public Font bold  = new Font (new FontFamily ("Bitstream Vera Sans"), 10, FontStyle.Bold);
	}

	// Colors for drawing.
	// FIXME Need to rename these members to something more sensible.
	public sealed struct 
	Color
	{
		static public System.Drawing.Color bg_color;
		static public System.Drawing.Color fg_color;
		static public System.Drawing.Color bg_color_prelight;
		static public System.Drawing.Color fg_color_prelight;
		static public System.Drawing.Color element_bg_color;
		static public System.Drawing.Color element_fg_color;

		// Set the colors from the Gtk theme.
		public static void
		Set (Gtk.Style style)
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


	// Abstract base class for everything drawn on a Fyre Canvas.
	internal abstract class Widget
	{
		Rectangle	position;

		bool		selected;
		bool		hover;

		/*** Properties ***/
		public int
		X
		{
			get { return position.X; }
			set { position.X = value; }
		}

		public int
		Y
		{
			get { return position.Y; }
			set { position.Y = value; }
		}

		// FIXME Not sure if these properties need to be r/w, might be ok
		// with just read access.
		public int
		Width
		{
			get { return position.Width; }
			set { position.Width = value; }
		}

		public int
		Height
		{
			get { return position.Height; }
			set { position.Height = value; }
		}

		/*** Methods ***/
		public
		Widget (int x, int y, int w, int h)
		{
			position = new Rectangle (x, y, w, h);
		}

		public
		Widget (Rectangle pos)
		{
			position = pos;
		}

		// All of these objects should provide a method for drawing themselves.
		public virtual void
		Draw (Graphics context)
		{
		}

		// All of these objects should provide a method for drawing themselves backwards.
		public virtual void
		RDraw (Graphics context)
		{
		}
	}

	// A container for other Widgets. Similar to a Gtk.Container.
	internal abstract class Container : Widget
	{
		int				x_pad;
		int				y_pad;
		int				x_spacing;
		int				y_spacing;

		System.Collections.ArrayList	start;
		System.Collections.ArrayList	end;

		// Default Container has no padding and no spacing.
		public
		Container () : this (0, 0, 0, 0)
		{
		}

		public
		Container (int xpad, int ypad, int xspace, int yspace) : base (0, 0, 0, 0)
		{
			x_pad     = xpad;
			y_pad     = ypad;
			x_spacing = xspace;
			y_spacing = yspace;

			position.Width = 2*xpad;
			position.Height = 2*ypad;

			start = new System.Collections.ArrayList ();
			end = new System.Collections.ArrayList ();
		}

		public void
		Draw (Graphics context)
		{
		}

		public void
		RDraw (Graphics context)
		{
		}

		public virtual void
		PackStart (Widget child)
		{
		}

		public virtual void
		PackEnd (Widget child)
		{
		}

		protected void
		Add (Widget child)
		{
			position.Width += child.Width + x_spacing;
			position.Height += child.Height + y_spacing;
		}
	}

	internal class HBox : Container
	{
		public void
		PackStart (Widget child)
		{
			child.X = position.X + x_pad;
			child.Y = position.Y + y_pad;

			if (start.Count > 0) {
				foreach (Widget w in start)
					child.Y += w.Height + y_spacing;
			}

			start.Add (child);
			Add (child);
		}

		public void
		PackEnd (Widget child)
		{
			child.X = position.X + position.Width - x_pad - child.Width;
			child.Y = position.Y + y_pad;
		
			if (end.Count > 0) {
				foreach (Widget w in end)
					child.Y += w.Height + y_spacing;
			}

			end.Add (child);
			Add (child);
		}
	}

	internal class VBox : Container
	{
		public void
		PackStart (Widget child)
		{
		}

		public void
		PackEnd (Widget child)
		{
		}
	}

	// Element Root is the base Widget for all Elements drawn on the canvas.
	// It contains a VBox for packing labels and pads into and handles drawing
	// the background of the Element.
	internal class ElementRoot : Widget
	{
		public VBox box;

		public
		ElementRoot ()
		{
		}

		public void
		Draw (Graphics context)
		{
		}

		public void
		RDraw (Graphics context)
		{
		}
	}

	// Represents a single pad on the canvas.
	internal class Pad : Widget
	{
		public
		Pad ()
		{
		}

		public void
		Draw (Graphics context)
		{
			PointF []	triangle = new PointF[3];
			Pen		pen = new Pen (Color.fg_color);
			Brush		brush = new SolidBrush (Color.element_bg_color);
			Brush		trifill = new SolidBrush (Color.element_fg_color);

			// The corners of the triangle.
			triangle[0] = new System.Drawing.PointF (position.X+8,position.Y+5);
			triangle[1] = new System.Drawing.PointF (position.X+8,position.Y+15);
			triangle[2] = new System.Drawing.PointF (position.X+13,position.Y+10);

			// Draw a white circle with a black border
			context.FillEllipse (brush, box);
			context.DrawEllipse (pen, box);

			// Draw the triangle.
			context.FillPolygon (trifill, triangle);
		}

		public void
		RDraw (Graphics context)
		{
			PointF []	triangle = new PointF[3];
			Pen		pen = new Pen (Color.fg_color);
			Brush		brush = new SolidBrush (Color.element_bg_color);
			Brush		trifill = new SolidBrush (Color.element_fg_color);

			// The corners of the triangle.
			triangle[0] = new System.Drawing.PointF (position.X+position.Width-8,position.Y+5);
			triangle[1] = new System.Drawing.PointF (position.X+position.Width-8,position.Y+15);
			triangle[2] = new System.Drawing.PointF (position.X+position.Width-13,position.Y+10);

			// Draw a white circle with a black border
			context.FillEllipse (brush, box);
			context.DrawEllipse (pen, box);

			// Draw the triangle.
			context.FillPolygon (trifill, triangle);

		}
	}

	// Represents a string drawn on the canvas.
	internal class Label : Widget
	{
		public
		Label ()
		{
		}

		public void
		Draw (Graphics context)
		{
		}

		public void
		RDraw (Graphics context)
		{
		}
	}

}
