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
		public VBox		box;

		Fyre.Element		element;

		// Selection state
		public bool		Selected;
		bool			flipped;

		/*** Constructors ***/
		public
		Element (Fyre.Element e, Gdk.Drawable drawable)
		{
			Graphics	graphics = Gtk.DotNet.Graphics.FromDrawable (drawable);

			Label		name = new Label (e.Name (), Font.bold, graphics);
			VBox		in_box = new VBox (0, 0, 10);
			VBox		out_box = new VBox (0, 0, 10);
			HBox		pad_box = new HBox (0, 0, 50);
			HBox		pad;

			box = new VBox (0, 0, 7);

			if (e.inputs != null) {
				foreach (Fyre.InputPad i in e.inputs) {
					pad = new HBox (0, 0, 7);
					in_box.PackStart (box);
					pad.PackStart (new Pad ());
					pad.PackStart (new Label (i.Name, Font.plain, graphics));
				}
			}

			if (e.outputs != null) {
				foreach (Fyre.OutputPad o in e.outputs) {
					pad = new HBox (0, 0, 7);
					out_box.PackStart (box);
					pad.PackStart (new Label (o.Name, Font.plain, graphics));
					pad.PackStart (new Pad ());
				}
			}

			pad_box.PackStart (in_box);
			pad_box.PackStart (out_box);

			box.PackStart (name);
			box.PackStart (pad_box);

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
			Pen	border = new System.Drawing.Pen (Color.fg_color);
			Brush	background = new System.Drawing.SolidBrush (Color.element_bg_color);

			context.FillRectangle (background, 10, 0, Position.Width-21, Position.Height-1);
			context.DrawRectangle (border, 10, 0, Position.Width-21, Position.Height-1);

			box.Draw (context);
		}

		public virtual void
		RDraw (System.Drawing.Graphics context)
		{
			Pen	border = new Pen (Color.fg_color);
			Brush	background = new SolidBrush (Color.element_bg_color);
			Brush	black = new SolidBrush (System.Drawing.Color.Black);

			context.FillRectangle (black, 0, 0, Position.Width, Position.Height);
			context.FillRectangle (background, 10, 0, Position.Width-21, Position.Height-1);
			context.DrawRectangle (border, 10, 0, Position.Width-21, Position.Height-1);

			box.RDraw (context);
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
}
