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
		// Width and height are maintained by the public layout system.
		// X & Y are maintained by the global layout system. We'll probably
		// want get/set operators on this to trigger redraws, etc.
		public Rectangle	Position;
		ElementRoot		root;

		Fyre.Element		element;

		// Selection state
		public bool		Selected;
		bool			flipped;

		/*** Properties ***/
		public int
		Width
		{
			get { return root.Width; }
		}

		public int
		Height
		{
			get { return root.Height; }
		}

		public int
		X
		{
			get { return root.X; }
		}

		public int
		Y
		{
			get { return root.Y; }
		}

		/*** Constructors ***/
		public
		Element (Fyre.Element e, Gdk.Drawable drawable)
		{
			Graphics	graphics = Gtk.DotNet.Graphics.FromDrawable (drawable);

			VBox		in_box = new VBox (0, 0, 10);
			VBox		out_box = new VBox (0, 0, 10);
			HBox		box;

			if (e.inputs != null) {
				foreach (Fyre.InputPad i in e.inputs) {
					box = new HBox (0, 0, 7);
					box.PackStart (new Pad ());
					box.PackStart (new Label (i.Name, Font.plain, graphics));
					in_box.PackStart (box);
				}
			}

			if (e.outputs != null) {
				foreach (Fyre.OutputPad o in e.outputs) {
					box = new HBox (0, 0, 7);
					box.PackStart (new Label (o.Name, Font.plain, graphics));
					box.PackStart (new Pad ());
					out_box.PackStart (box);
				}
			}

			HBox	pad_box = new HBox (0, 0, 50);
			pad_box.PackStart (in_box);
			pad_box.PackStart (out_box);

			Label	name = new Label (e.Name (), Font.bold, graphics);

			root = new ElementRoot (name);
			root.box.PackStart (pad_box);

			// Store a reference to the element we're drawing.
			element = e;
		}

		/*** Public Methods ***/
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
			root.Draw (context);
		}

		public virtual void
		DrawMask (Gdk.Pixmap mask)
		{
			// FIXME
		}

		public virtual void
		Serialize (XmlTextWriter writer)
		{
			// TODO: Implement
		}

		public ElementHover
		GetHover (int x, int y)
		{
			// FIXME
			return ElementHover.None;
		}
	}


	/*** Element Drawing Primitives ***/

	// FIXME Should these two structs maybe be private? Do we need them outside
	// of drawing stuff on the canvas?

	// Our fonts.
	// FIXME Might be a good idea to have something to fall back on if they
	// don't have Bitstream Vera fonts.
	public struct
	Font
	{
		static public System.Drawing.Font plain = new System.Drawing.Font (new FontFamily ("Bitstream Vera Sans"), 10, FontStyle.Regular);
		static public System.Drawing.Font bold  = new System.Drawing.Font (new FontFamily ("Bitstream Vera Sans"), 10, FontStyle.Bold);
	}

	// Colors for drawing.
	// FIXME Need to rename these members to something more sensible.
	public struct 
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
	public abstract class Widget
	{
		protected Rectangle	position;

		bool			selected;
		bool			hover;

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

		/*** Constructors ***/
		public
		Widget ()
		{
			position = new Rectangle (0, 0, 0, 0);
		}

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

		/*** Public Methods ***/
		public virtual bool
		Remove (Widget child)
		{
			return false;
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
	public abstract class Container : Widget
	{
		protected int				x_pad;
		protected int				y_pad;
		protected int				spacing;

		protected System.Collections.ArrayList	start;
		protected System.Collections.ArrayList	end;

		/*** Constructors ***/
		// Default Container has no padding and no spacing.
		public
		Container () : this (0, 0, 0)
		{
		}

		public
		Container (int xpad, int ypad, int space) : base ()
		{
			x_pad     = xpad;
			y_pad     = ypad;
			spacing = space;

			position.Width = 2*xpad;
			position.Height = 2*ypad;

			start = new System.Collections.ArrayList ();
			end = new System.Collections.ArrayList ();
		}

		/*** Public Methods ***/
		public override void
		Draw (Graphics context)
		{
			if (start.Count > 0) {
				foreach (Widget w in start)
					w.Draw (context);
			}

			if (end.Count > 0) {
				foreach (Widget w in start)
					w.Draw (context);
			}
		}

		public override void
		RDraw (Graphics context)
		{
			if (start.Count > 0) {
				foreach (Widget w in start)
					w.RDraw (context);
			}

			if (end.Count > 0) {
				foreach (Widget w in start)
					w.RDraw (context);
			}
		}

		public override bool
		Remove (Widget child)
		{
			if (start.Contains (child)) {
				position.Width -= child.Width;
				position.Height -= child.Height;
				RemoveStart (child);
				return true;
			}
			if (end.Contains (child)) {
				position.Width -= child.Width;
				position.Height -= child.Height;
				RemoveEnd (child);
				return true;
			}

			foreach (Widget w in start) {
				if (w.Remove (child))
					return true;
			}

			foreach (Widget w in end) {
				if (w.Remove (child))
					return true;
			}

			return false;
		}

		public virtual void
		PackStart (Widget child)
		{
		}

		public virtual void
		PackEnd (Widget child)
		{
		}

		/*** Protected Methods ***/
		protected virtual void
		RemoveStart (Widget child)
		{
		}

		protected virtual void
		RemoveEnd (Widget child)
		{
		}

		protected void
		Add (Widget child)
		{
			position.Width += child.Width + spacing;
			position.Height += child.Height + spacing;
		}


	}

	public class VBox : Container
	{
		/*** Constructors ***/
		public
		VBox () : base ()
		{
		}

		public
		VBox (int x, int y, int space) : base (x, y, space)
		{
		}

		/*** Public Methods ***/
		public override void
		PackStart (Widget child)
		{
			child.X = position.X + x_pad;
			child.Y = position.Y + y_pad;

			if (start.Count > 0) {
				foreach (Widget w in start)
					child.Y += w.Height + spacing;
			}

			start.Add (child);
			Add (child);
		}

		public override void
		PackEnd (Widget child)
		{
			child.X = position.X + x_pad;
			child.Y = position.Y + position.Height - y_pad - child.Height;

			if (end.Count > 0) {
				foreach (Widget w in end)
					child.Y -= w.Height - spacing;
			}

			end.Add (child);
			Add (child);
		}

		/*** Protected Methods ***/
		protected override void
		RemoveStart (Widget child)
		{
			int y = position.Y + y_pad;

			start.Remove (child);

			if (start.Count > 0)
				position.Width -= spacing;

			foreach (Widget w in start) {
				w.Y = y;
				y += w.Height + spacing;
			}
		}

		protected override void
		RemoveEnd (Widget child)
		{
			int y = position.Y + position.Height - y_pad;

			end.Remove (child);

			if (end.Count > 0)
				position.Width -= spacing;

			foreach (Widget w in end) {
				w.Y = y - w.Height;
				y -= w.Height - spacing;
			}
		}
	}

	public class HBox : Container
	{
		/*** Constructors ***/
		public
		HBox () : base ()
		{
		}

		public
		HBox (int x, int y, int space) : base (x, y, space)
		{
		}

		/*** Public Methods ***/
		public override void
		PackStart (Widget child)
		{
			child.X = position.X + x_pad;
			child.Y = position.Y + y_pad;

			if (start.Count > 0) {
				foreach (Widget w in start)
					child.X += w.Width + spacing;
			}

			start.Add (child);
			Add (child);
		}

		public override void
		PackEnd (Widget child)
		{
			child.X = position.X + position.Width - x_pad - child.Width;
			child.Y = position.Y + y_pad;

			if (end.Count > 0) {
				foreach (Widget w in end)
					child.X -= w.Width - spacing;
			}

			end.Add (child);
			Add (child);
		}

		/*** Protected Methods ***/
		protected override void
		RemoveStart (Widget child)
		{
			int x = position.X + x_pad;

			start.Remove (child);

			if (start.Count > 0)
				position.Height -= spacing;

			foreach (Widget w in start) {
				w.X = x;
				x += w.Width + spacing;
			}
		}

		protected override void
		RemoveEnd (Widget child)
		{
			int x = position.X + position.Width - x_pad;

			end.Remove (child);

			if (end.Count > 0 )
				position.Height -= spacing;

			foreach (Widget w in end) {
				w.X = x - w.Width;
				x -= w.Width - spacing;
			}
		}
	}

	// Element Root is the base Widget for all Elements drawn on the canvas.
	// It contains a VBox for packing labels, pads, and other boxes into and 
	// handles drawing the background of the Element.
	public class ElementRoot : Widget
	{
		public VBox box;

		/*** Constructors ***/
		public
		ElementRoot ()
		{
			box = new VBox();
		}

		public
		ElementRoot (Label name) : this ()
		{
			box.PackStart (name);
		}

		/*** Public Methods ***/
		public override void
		Draw (Graphics context)
		{
			Pen	border = new System.Drawing.Pen (Color.fg_color);
			Brush	background = new System.Drawing.SolidBrush (Color.element_bg_color);

			context.FillRectangle (background, 10, 0, position.Width-21, position.Height-1);
			context.DrawRectangle (border, 10, 0, position.Width-21, position.Height-1);

			box.Draw (context);
		}

		public override void
		RDraw (Graphics context)
		{
			Pen	border = new System.Drawing.Pen (Color.fg_color);
			Brush	background = new System.Drawing.SolidBrush (Color.element_bg_color);

			context.FillRectangle (background, 10, 0, position.Width-21, position.Height-1);
			context.DrawRectangle (border, 10, 0, position.Width-21, position.Height-1);

			box.RDraw (context);
		}
	}

	// Represents a single pad on the canvas.
	public class Pad : Widget
	{
		/*** Public Methods ***/
		public override void
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
			context.FillEllipse (brush, position);
			context.DrawEllipse (pen, position);

			// Draw the triangle.
			context.FillPolygon (trifill, triangle);
		}

		public override void
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
			context.FillEllipse (brush, position);
			context.DrawEllipse (pen, position);

			// Draw the triangle.
			context.FillPolygon (trifill, triangle);

		}
	}

	// Represents a string drawn on the canvas.
	public class Label : Widget
	{
		new RectangleF		position;
		string			text;
		System.Drawing.Font	style;

		/*** Constructors ***/
		public
		Label (string s, System.Drawing.Font style, Graphics context)
		{
			SizeF	sz = context.MeasureString (s, style);
			PointF	pt = new PointF (0, 0);
			position = new RectangleF (pt, sz);

			this.style = style;
			text = s;
		}

		/*** Public Methods ***/
		public override void
		Draw (Graphics context)
		{
			Brush	brush = new SolidBrush (Color.fg_color);

			context.DrawString (text, style, brush, position);
		}

		// Labels don't change when drawn backwards.
		public override void
		RDraw (Graphics context)
		{
			Draw (context);
		}
	}

}
