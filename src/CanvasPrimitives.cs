/*
 * CanvasPrimitives - drawing primitives for Fyre's canvas
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

namespace Fyre.Canvas
{
	// Our fonts.
	// FIXME Might be a good idea to have something to fall back on if they
	// don't have Bitstream Vera fonts.
	static class
	Font
	{
		static public System.Drawing.Font plain = new System.Drawing.Font (new FontFamily ("Bitstream Vera Sans"), 10, FontStyle.Regular);
		static public System.Drawing.Font bold  = new System.Drawing.Font (new FontFamily ("Bitstream Vera Sans"), 10, FontStyle.Bold);
	}

	// Colors for drawing.
	// FIXME Need to rename these members to something more sensible.
	static class
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
			bg_color          = SDColorFromGdk (style.Background (Gtk.StateType.Normal));
			fg_color          = SDColorFromGdk (style.Foreground (Gtk.StateType.Normal));
			bg_color_prelight = SDColorFromGdk (style.Background (Gtk.StateType.Prelight));
			fg_color_prelight = SDColorFromGdk (style.Foreground (Gtk.StateType.Prelight));
			element_bg_color  = SDColorFromGdk (style.Background (Gtk.StateType.Insensitive));
			element_fg_color  = SDColorFromGdk (style.Dark (Gtk.StateType.Selected));
		}

		static System.Drawing.Color
		SDColorFromGdk (Gdk.Color c)
		{
			return System.Drawing.Color.FromArgb (c.Red / 256, c.Green / 256, c.Blue / 256);
		}
	}


	// Abstract base class for everything drawn on a Fyre Canvas.
	public abstract class Widget
	{
		protected Rectangle			position;

		bool					selected;
		bool					hover;

		public event System.EventHandler	SizeChanged;

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

		// All widgets should be able to draw themselves on a 1-bit drawable for a
		// transparency mask.
		public virtual void
		Mask (Gdk.Drawable drawable)
		{
		}

		/*** Protected Methods ***/
		protected void
		OnSizeChanged (System.EventArgs e)
		{
			if (SizeChanged != null)
				SizeChanged (this, e);
		}
	}

	// A container for other Widgets. Similar to a Gtk.Container.
	public abstract class Container : Widget
	{
		protected int				x_pad;
		protected int				y_pad;
		protected int				spacing;
		protected System.Collections.ArrayList	children;

		/*** Properties ***/
		// Containers need to propagate changes to their coordinates.
		new public int
		X
		{
			get { return position.X; }
			set
			{
				int dx = value - position.X;

				foreach (Widget w in children)
					w.X += dx;

				position.X = value;
			}
		}

		new public int
		Y
		{
			get { return position.Y; }
			set
			{
				int dy = value - position.Y;

				foreach (Widget w in children)
					w.Y += dy;

				position.Y = value;
			}
		}

		/*** Constructors ***/
		// Default Container has no padding and no spacing.
		public
		Container () : this (0, 0, 0)
		{
		}

		public
		Container (int xpad, int ypad, int space) : base ()
		{
			children = new System.Collections.ArrayList ();

			x_pad   = xpad;
			y_pad   = ypad;
			spacing = space;
		}

		/*** Public Methods ***/
		public override void
		Draw (Graphics context)
		{
			foreach (Widget w in children)
				w.Draw (context);
		}

		public override void
		RDraw (Graphics context)
		{
			foreach (Widget w in children)
				w.RDraw (context);
		}

		public override bool
		Remove (Widget child)
		{
			return false;
		}

		public virtual void
		PackStart (Widget child)
		{
			// Append the widget.
			children.Add (child);

			// If this is the only widget in the container, the container's height and width
			// are deteremined only by X and Y padding, and the height and width of the child.
			if (children.Count == 1) {
				position.Width = 2*x_pad + child.Width;
				position.Height = 2*y_pad + child.Height;
			}

			// Notify the container when the child's size changes.
			child.SizeChanged += new System.EventHandler (Resize);

			OnSizeChanged (new System.EventArgs ());
		}

		/*** Protected Methods ***/
		protected virtual void
		Resize (object o, System.EventArgs args)
		{
			OnSizeChanged (args);
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
		VBox (int xpad, int ypad, int space) : base (xpad, ypad, space)
		{
		}

		/*** Public Methods ***/
		public override void
		PackStart (Widget child)
		{
			child.X = position.X + x_pad;
			child.Y = position.Y + y_pad;

			foreach (Widget w in children)
				child.Y += w.Height + spacing;

			if (children.Count > 0)
				position.Height += child.Height + spacing;

			int W = 2 * x_pad + child.Width;

			if (W > position.Width)
				position.Width = W;

			base.PackStart (child);
		}

		/*** Protected Methods ***/
		protected override void
		Resize (object o, System.EventArgs args)
		{
			position.Width = 2*x_pad;
			position.Height = 2*y_pad;

			foreach (Widget w in children) {
				int W = 2 * x_pad + w.Width;
				if (W > position.Width)
					position.Width = W;

				position.Height += w.Height + spacing;
			}

			// The previous loop always adds spacing one more time than we need.
			position.Height -= spacing;

			// Propogate the change.
			OnSizeChanged (args);
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
		HBox (int xpad, int ypad, int space) : base (xpad, ypad, space)
		{
		}

		/*** Public Methods ***/
		public override void
		PackStart (Widget child)
		{
			child.X = position.X + x_pad;
			child.Y = position.Y + y_pad;

			foreach (Widget w in children)
				child.X += w.Width + spacing;

			if (children.Count > 0)
				position.Width += child.Width + spacing;

			int h = 2 * y_pad + child.Height;

			if (h > position.Height)
				position.Height = h;

			base.PackStart (child);
		}

		/*** Protected Methods ***/
		protected override void
		Resize (object o, System.EventArgs args)
		{
			position.Width = 2*x_pad;
			position.Height = 2*y_pad;

			foreach (Widget w in children) {
				int h = 2 * y_pad + w.Height;
				if (h > position.Height)
					position.Height = h;

				position.Width += w.Width + spacing;
			}

			// The previous loop always adds spacing one more time than we need.
			position.Width -= spacing;

			// Propogate the change.
			OnSizeChanged (args);
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
			box = new VBox (15, 5, 6);
			box.SizeChanged += new System.EventHandler (Resize);
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
			Pen	border = new Pen (Color.fg_color);
			Brush	background = new SolidBrush (Color.element_bg_color);
			Brush	black = new SolidBrush (System.Drawing.Color.Black);

			context.FillRectangle (black, 0, 0, position.Width, position.Height);
			context.FillRectangle (background, 10, 0, position.Width-21, position.Height-1);
			context.DrawRectangle (border, 10, 0, position.Width-21, position.Height-1);

			box.RDraw (context);
		}

		/*** Protected Methods ***/
		protected void
		Resize (object o, System.EventArgs args)
		{
			position.Width = box.Width + 20;
			position.Height = box.Height;
		}
	}

	// Represents a single pad on the canvas.
	public class Pad : Widget
	{
		public
		Pad () : base (0, 0, 20, 20)
		{
		}

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
		string			text;
		System.Drawing.Font	style;

		/*** Constructors ***/
		public
		Label (string s, System.Drawing.Font style, Graphics context)
		{
			SizeF	sz = context.MeasureString (s, style);
			position.X = 0; position.Y = 0;
			position.Width = (int) sz.Width;
			position.Height = (int) sz.Height;

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
